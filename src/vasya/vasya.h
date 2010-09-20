/* coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 * Understanding is not required. Only obedience.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
// VASYA: Very Agile Scripting Yarn Architecture
#ifndef VASYA_H
#define VASYA_H

#include <stdio.h>

#include <fstream>
#include <istream>
#include <map>
#include <string>
#include <vector>


#define VASYA_NO_RETURN            __attribute__((noreturn))
#define VASYA_LIKE_PRINTF(p1, p2)  __attribute__((format(printf, p1, p2)))


struct VasyaVMOp {
  unsigned char op;
  union {
    struct { short a, b, c; } __attribute__((__packed__));
    short args[3];
  } __attribute__((__packed__));
  int line;
};


typedef std::vector<VasyaVMOp *> VasyaBCodeVector;

class VasyaCC;
class VasyaVM;

class VasyaObject;
class VasyaFunction;
class VasyaValue;


typedef std::map<std::string, VasyaValue *> VasyaFieldMap;
typedef std::map<std::string, int> VasyaLVarMap;
typedef std::vector<std::pair<int, int> > VasyaCVList;


class VasyaRefCount {
public:
  VasyaRefCount () : mRefCnt(0) {}
  virtual ~VasyaRefCount () {};

  inline void incRef () { mRefCnt++; }
  inline void decRef () { if (--mRefCnt <= 0) delete this; }
  inline void decrementRef () { mRefCnt--; }
  inline int refCount () const { return mRefCnt; }

private:
  VasyaRefCount (const VasyaRefCount &);
  VasyaRefCount &operator = (const VasyaRefCount &);

private:
  int mRefCnt;
};


class VasyaObject : public VasyaRefCount {
public:
  VasyaObject () : VasyaRefCount() {}
  virtual ~VasyaObject () {}

  virtual VasyaValue *getField (VasyaVM *vm, const std::string &name) = 0;
  virtual VasyaValue *getField (VasyaVM *vm, long idx) = 0;
  virtual void setField (VasyaVM *vm, const std::string &name, VasyaValue *value) = 0;
  virtual void setField (VasyaVM *vm, long idx, VasyaValue *value) = 0;

  virtual std::string toString () {
    char buf[64];
    sprintf(buf, "%p", this);
    return buf;
  }

private:
  VasyaObject (const VasyaObject &);
  VasyaObject &operator = (const VasyaObject &);
};


class VasyaSimpleObject : public VasyaObject {
public:
  VasyaSimpleObject ();
  virtual ~VasyaSimpleObject ();

  virtual VasyaValue *getField (VasyaVM *vm, const std::string &name);
  virtual VasyaValue *getField (VasyaVM *vm, long idx);

  virtual void setField (VasyaVM *vm, const std::string &name, VasyaValue *v);
  virtual void setField (VasyaVM *vm, long idx, VasyaValue *v);

  virtual std::string toString () { return "SimpleObject"; }

protected:
  VasyaFieldMap mFields;
};


class VasyaPInhObject : public VasyaSimpleObject {
public:
  VasyaPInhObject () : VasyaSimpleObject() {}
  virtual ~VasyaPInhObject () {}

  virtual VasyaValue *getField (VasyaVM *vm, const std::string &name);

  virtual std::string toString () { return "Object"; }
};


class VasyaValue : public VasyaRefCount {
  friend class VasyaCC;

public:
  enum vtype {
    vvoid,
    number,
    string,
    vector,
    function,
    object
    //field
    //ref // reference to VasyaValue
  };

public:
  VasyaValue () : VasyaRefCount() { init(); }
  VasyaValue (double v) : VasyaRefCount() { init(); type = number; nval = v; }
  VasyaValue (const std::string &v) : VasyaRefCount() { init(); type = string; sval = v; }
  VasyaValue (double v0, double v1) : VasyaRefCount() { init(); type = vector; vval[0] = v0; vval[1] = v1; }
  VasyaValue (VasyaFunction *fn);
  VasyaValue (VasyaObject *obj);

  ~VasyaValue ();

  void init ();

  //VasyaValue *clone () const;

  std::string toString () const;

  inline bool isTrue () {
    switch (type) {
      case vvoid: return false;
      case number: return nval != 0.0;
      case string: return !sval.empty();
      default: ;
    }
    return true;
  }

  inline bool isRO () const { return mRO; }
  inline void setRO (bool v) { mRO = v; }

public:
  vtype type;
  std::string sval;
  double nval;
  double vval[2];
  VasyaFunction *vfunc;
  std::vector<VasyaValue *> clos;
  VasyaObject *vobj; // obj for object, field or reference
  VasyaValue *vref; // ref for field or reference
  bool mRO;
  //
  VasyaValue *prev;
};


class VasyaFunction : public VasyaRefCount {
public:
  VasyaFunction ();
  //VasyaFunction (const VasyaFunction &src);
  virtual ~VasyaFunction ();

  virtual bool isNative () const { return false; }
  virtual VasyaValue *exec (VasyaVM *vm, int argv, int argc, VasyaValue *self) { return 0; }

public:
  bool fvararg;
  int locCount;
  int locTemps;
  int argc;
  std::vector<VasyaValue *> lits;
  VasyaBCodeVector bcode;
  // closures
  VasyaCVList clos; // first: level; second: litNo
};

class VasyaFDef;

class VasyaVar {
public:
  VasyaVar (VasyaFDef *owner);

  inline bool isVar () const { return var; }
  inline bool isField () const { return var && field; }
  inline bool isLocal () const { return var && !field && name.empty(); }
  inline bool isGlobal () const { return var && !field && !name.empty(); }
  inline bool isLit () const { return !var; }
  inline bool isThis () const { return mThis; }

public:
  //VasyaValue::vtype type;
  bool var;
  bool field;
  bool mThis;
  std::string name; // not empty: global
  int locIdx;
  int litIdx; // literal
  VasyaVar *obj; // for fields
  VasyaFDef *func;
};


class VasyaFDef {
public:
  VasyaFDef ();
  ~VasyaFDef ();

  short getTemp ();
  bool freeTemp (short idx);
  void setTempGlob (short idx, const std::string &name);
  short findTempGlob (const std::string &name); // <0: not found
  void dirtyTempGlob (const std::string &name);
  void tempMoved (short dst, short src); // frees src
  void dirtyTempGlob (short idx);

  short allocVars (unsigned int cnt);

public:
  std::string mFName;
  VasyaFunction *func;
  std::vector<VasyaVar *> vars;
  std::vector<bool> ltused; // used temp vars
  VasyaLVarMap ltgm; // globals in temps
  VasyaLVarMap locs; // first is args
  VasyaLVarMap clos;
  VasyaLVarMap closDepth;
  VasyaLVarMap closIdx;
  VasyaFDef *prev;
};


class VasyaCCError {};

class VasyaCC {
  friend class VasyaVM;

public:
  //VasyaCC (const std::string &fname, const valuemap *vm=0);
  //VasyaCC (inputfile *fl, const valuemap *vm=0);
  VasyaCC (const std::string &fname);
  VasyaCC (std::istream *fl);
  ~VasyaCC ();

  inline void setLineNo (int lno) { mCurLine = lno; }

  VasyaFunction *compileBody ();

protected:
  virtual void showDieMsg (int line, const char *msg);
  virtual void showWarningMsg (int line, const char *msg);

  virtual void die (const char *msg, ...) VASYA_NO_RETURN VASYA_LIKE_PRINTF(2, 3);
  virtual void dieAtLine (int line, const char *msg, ...) VASYA_NO_RETURN VASYA_LIKE_PRINTF(3, 4);
  virtual void warning (const char *msg, ...) VASYA_LIKE_PRINTF(2, 3);
  virtual void warningAtLine (int line, const char *msg, ...) VASYA_LIKE_PRINTF(3, 4);

  void dieExpected (const char *extxt);

  void init ();

  inline static bool isLit (int idx) { return idx<0; }

  /*virtual*/ char getChar (bool eofFail=true); // fail on eof
  /*virtual*/ void ungetChar (char ch);

  void parseString (char qch=0);
  void parseNumber (char ch);
  void parseBasedNumber (int base);
  void parseVector (char ch);

  int skipBlanks (bool dieAtEOF=false);
  void nextToken ();

  void matchSemi ();

  void compileFuncBody (bool cbTaken); // will not take out 'scope'
  void compileStatement ();
  void compileBlock (); // token should be "{"

  void compileLocal ();
  void compileReturn ();
  void compileIf ();
  void compileDo ();
  void compileWhile ();
  void compileFor ();
  void compileBreak ();
  void compileContinue ();
  int compileFuncDX (const std::string &fname); // returns literal number

  int compileExpression ();

  int findLocal (const std::string &n, bool addClos);
  int addLocal (const std::string &n);

  virtual int termConst (); // returns 0 or result of newLiteral()
  VasyaVar *term (bool allowPM);
  VasyaVar *expr (int priority=-1);

  VasyaVar *newVar ();
  void newExpr ();
  void endExpr ();

  // returns pos
  int emit (unsigned char opc, short a=0, short b=0, short c=0);
  void patchJumpChain (int cpc, int dest=-1);
  inline int getPC () const { return (int)(scope->func->bcode.size()); }

  int newLiteral (VasyaValue *v);
  int addNumLiteral (double v);
  int addStrLiteral (const std::string &s);
  int addVecLiteral (const double d0, const double d1);
  int addVoidLiteral ();
  int addLiteral ();

  VasyaValue *getLit (VasyaVar *v);
  VasyaValue *getLit (int idx);

  short giveTemp ();
  void freeTemp (VasyaVar *v);
  void freeTemp (short idx);
  void freeTempIfNot (VasyaVar *v, short idx);
  short loadValue (VasyaVar *v);
  void storeValue (VasyaVar *v, short idx);
  void loadValueTo (short locIdx, VasyaVar *v);
  VasyaVar *loadVar (VasyaVar *v);

  void removeScope ();

  void removeCode (int oldPC);

  void removeUnusedLits ();

  static void dumpBC (const VasyaFunction *fn, int pc=0, int len=-1);

protected:
  enum ttype {
    ttEOF, // end of file reached
    ttID,  // an alphanumeric name token
    ttDEL, // code punctuation
    ttIMM  // string, double, vector
  };

  std::string mFName;
  std::istream *mInFile;
  std::filebuf *mFBuf;
  int mCurLine;
  char mUngot;
  //
  std::string tStr; // or id
  double tNum;
  double tVec[2];
  ttype tType;
  VasyaValue::vtype tImmType;
  int brkPC;
  int contPC;
  bool allowBC;
  bool cpcValid;
  //
  VasyaFDef *scope;
};


class VasyaVMError {};
class VasyaVMBreak {};

class VasyaVM {
public:
  VasyaVM ();
  ~VasyaVM ();

  // args will be destroyed if there are no more refs
  VasyaValue *execute (VasyaFunction *fn);
  VasyaValue *apply (VasyaFunction *fn, VasyaValue *aThis, int argc, VasyaValue **args);

  VasyaValue *getLocal (int ln, int depth=0); // it desn't copy value, so take care
  void setLocal (int ln, VasyaValue *v); // destroys old VasyaValue

  void addFunc (const std::string &name, VasyaFunction *fn);

  VasyaValue *getGlobal (const std::string &name);
  void setGlobal (const std::string &name, VasyaValue *v);

/*
  VasyaValue *newValue ();
  VasyaValue *newNumber (const double v);
  VasyaValue *newString (const std::string &v);
  VasyaValue *newFunction (VasyaFunction *fn); // owns fn
  VasyaValue *newVector (const double v0, const double v1);
*/

  inline VasyaValue *getNaNValue () const { return mNaN; }

  virtual void die (const char *msg, ...) VASYA_NO_RETURN VASYA_LIKE_PRINTF(2, 3);

  virtual bool shouldBreak () { return false; }

public:
  static bool str2num (const std::string &s, double *d);
  static VasyaValue *bmath (const unsigned char op, const VasyaValue *op0, const VasyaValue *op1, VasyaValue *vNaN);
  static bool cmpmath (const unsigned char op, const VasyaValue *op0, const VasyaValue *op1);

protected:
  virtual void showDieMsg (const char *msg);

  void addRStop ();
  void addRFunc (VasyaValue *fn, int resDL, VasyaValue *aThis=0);
  VasyaValue *run ();

  VasyaValue *dropR ();

protected:
  struct RStackFrame {
    RStackFrame () : func(0), resDestLoc(-1), pc(0), vThis(0), prev(0) {}
    //
    VasyaValue *func;
    int resDestLoc;
    int pc;
    VasyaValue *vThis;
    std::vector<VasyaValue *>locs;
    std::vector<VasyaValue *>locTemps;
    std::vector<VasyaValue *>args;
    struct RStackFrame *prev;
  };

protected:
  RStackFrame *mRStack;
  VasyaSimpleObject *mWorld;
  VasyaValue *mWorldVV;
  VasyaValue *mZero;
  VasyaValue *mOne;
  VasyaValue *mNaN;
  VasyaValue *mVoid;
  int mBreakCheckCount;
  int mLastLine;
  int mLastPC;
};


class VasyaFunctionApply : public VasyaFunction {
public:
  VasyaFunctionApply () : VasyaFunction() {}

  virtual bool isNative () const { return true; }
  virtual VasyaValue *exec (VasyaVM *vm, int argv, int argc, VasyaValue *self);
};


class VasyaFunctionCall : public VasyaFunction {
public:
  VasyaFunctionCall () : VasyaFunction() {}

  virtual bool isNative () const { return true; }
  virtual VasyaValue *exec (VasyaVM *vm, int argv, int argc, VasyaValue *self);
};


#endif
