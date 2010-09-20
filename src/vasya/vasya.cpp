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
#ifndef _ISOC99_SOURCE
# define _ISOC99_SOURCE
#endif
#include <math.h>
#include <fenv.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include <cctype>
#include <cstring>

#include <set>

#include "vasya.h"


#ifdef DEBUG_BUILD
# define vlog(...)  fprintf(stderr, __VA_ARGS__)
#else
# define vlog(...)
#endif


//NOT WORKING YET!
//#define VASYA_GEN_TEMPCLR


// opcodes
enum {
  VOP_RETURN,

  // a: loc-res; b,c: loc-arg
  VOP_ADD,
  VOP_SUB,
  VOP_MUL,
  VOP_DIV,
  VOP_MOD,

  VOP_EQ,
  VOP_NE,
  VOP_LE,
  VOP_GE,
  VOP_LT,
  VOP_GT,

  VOP_BITAND,
  VOP_BITOR,
  VOP_BITXOR,

  // a: loc-res; b: loc-var
  VOP_NOT,
  VOP_BITNOT,

  // a: loc-res; b: loc-arg
  VOP_STORE,
  // a: loc-res; b: string literal (name)
  VOP_LOADGLOB,
  // a: string literal (name); b: loc-arg
  VOP_STOREGLOB,

  // a: loc-res; b: object; c: loc-var/lit (fld)
  VOP_FLOAD, // load field value (value can be object itself)
  // a: loc-val (will NOT be changed); b: object; c: loc-var/lit (fld)
  VOP_FSTORE, // store field value; a can be literal

  // b: addr
  VOP_JUMP,
  // a: temp; b: addr
  VOP_JMPT,
  VOP_JMPF,

  // a: loc-argnf; b: argc; c: this/0
  // loc-argnf: func, args; result: in a
  VOP_CALL,

  VOP_THIS,
  VOP_NEWOBJ,

  VOP_TYPEOF,

  // a: dest, b: string literal
  //VOP_STRLEN,

#ifdef VASYA_GEN_TEMPCLR
  VOP_TEMPCLR,
#endif

  VOP_LAST,

  // temp opcodes for compiler, not used in VM
  VOPX_FREF, // load field ref; a: res; b: object; c: loc-var/lit (fld)
  VOPX_AND,
  VOPX_OR,
};


#ifdef DEBUG_BUILD
static const char *opNames[VOP_LAST] = {
  "VOP_RETURN",
  "VOP_ADD",
  "VOP_SUB",
  "VOP_MUL",
  "VOP_DIV",
  "VOP_MOD",
  "VOP_EQ",
  "VOP_NE",
  "VOP_LE",
  "VOP_GE",
  "VOP_LT",
  "VOP_GT",
  "VOP_BITAND",
  "VOP_BITOR",
  "VOP_BITXOR",
  "VOP_NOT",
  "VOP_BITNOT",
  "VOP_STORE",
  "VOP_LOADGLOB",
  "VOP_STOREGLOB",
  "VOP_FLOAD",
  "VOP_FSTORE",
  "VOP_JUMP",
  "VOP_JMPT",
  "VOP_JMPF",
  "VOP_CALL",
  "VOP_THIS",
  "VOP_NEWOBJ",
  "VOP_TYPEOF"
#ifdef VASYA_GEN_TEMPCLR
  ,"VOP_TEMPCLR"
#endif
};
#endif


struct OpcodeDef {
  const char *name;
  unsigned char op;
  int priority;
  bool rassoc;
};


static const OpcodeDef opcodes[] = {
  {"*", VOP_MUL, 2, false},
  {"/", VOP_DIV, 2, false},
  {"%", VOP_MOD, 2, false},
  {"&", VOP_BITAND, 2, false},
  {"+", VOP_ADD, 3, false},
  {"-", VOP_SUB, 3, false},
  {"|", VOP_BITOR, 3, false},
  {"^", VOP_BITXOR, 3, false},
  {"==", VOP_EQ, 4, false},
  {"!=", VOP_NE, 4, false},
  {"<=", VOP_LE, 4, false},
  {">=", VOP_GE, 4, false},
  {"<", VOP_LT, 4, false},
  {">", VOP_GT, 4, false},
  {"=", VOP_STORE, 5, true},
  {"+=", VOP_ADD, 5, true},
  {"-=", VOP_SUB, 5, true},
  {"*=", VOP_MUL, 5, true},
  {"/=", VOP_DIV, 5, true},
  {"%=", VOP_MOD, 5, true},
  {"|=", VOP_BITOR, 5, true},
  {"&=", VOP_BITAND, 5, true},
  {"^=", VOP_BITXOR, 5, true},
  {"&&", VOPX_AND, 6, false},
  {"||", VOPX_OR, 7, false},
  {0}
};
#define TOP_PRIORITY  7


#define LOCS_MAX      1000

#define FIRST_TEMP    1024
#define MAX_LOC_TEMP  10000

#define LOC_CLOS      (MAX_LOC_TEMP+FIRST_TEMP+1)
#define MAX_CLOS      5000

#define LOC_THIS      1023
#define LOC_CALLEE    1022
#define LOC_ARGS      1021


////////////////////////////////////////////////////////////////////////////////
VasyaValue::VasyaValue (VasyaFunction *fn) :
  VasyaRefCount()
{
  init();
  type = function;
  vfunc = fn;
  if (fn) fn->incRef();
}


VasyaValue::VasyaValue (VasyaObject *obj) :
  VasyaRefCount()
{
  init();
  type = object;
  vobj = obj;
  if (obj) obj->incRef();
}


VasyaValue::~VasyaValue () {
  if (vfunc) vfunc->decRef();
  if (vobj) vobj->decRef();
  for (std::vector<VasyaValue *>::iterator i = clos.begin(); i != clos.end(); ++i) (*i)->decRef();
}


void VasyaValue::init () {
  type = vvoid;
  sval.clear();
  nval = vval[0] = vval[1] = 0.0;
  vobj = 0;
  vref = 0;
  vfunc = 0;
}


std::string VasyaValue::toString () const {
  char buf[256];
  switch (type) {
    case vvoid: return "void";
    case number:
      sprintf(buf, "%.15g", nval);
      break;
    case string: return sval;
    case vector:
      sprintf(buf, "`%.15g %.15g`", vval[0], vval[1]);
      break;
    case object: {
      std::string s("{");
      s += vobj->toString()+"}";
      return s; }
    case function:
      //sprintf(buf, "<%p>", vfunc);
      sprintf(buf, "<function>");
    default: ;
  }
  return buf;
}


////////////////////////////////////////////////////////////////////////////////
VasyaSimpleObject::VasyaSimpleObject () :
  VasyaObject()
{
}


VasyaSimpleObject::~VasyaSimpleObject () {
  for (VasyaFieldMap::iterator i = mFields.begin(); i != mFields.end(); ++i) i->second->decRef();
}


VasyaValue *VasyaSimpleObject::getField (VasyaVM *vm, const std::string &name) {
  VasyaFieldMap::iterator i = mFields.find(name);
  if (i == mFields.end()) return 0;
  return i->second;
}


VasyaValue *VasyaSimpleObject::getField (VasyaVM *vm, long idx) {
  char buf[128];
  sprintf(buf, "%ld", idx);
  return getField(vm, buf);
}


void VasyaSimpleObject::setField (VasyaVM *vm, const std::string &name, VasyaValue *v) {
  VasyaFieldMap::iterator i = mFields.find(name);
  if (v) v->incRef();
  if (i != mFields.end()) {
    if (i->second) i->second->decRef();
    if (!v) mFields.erase(i); else i->second = v;
  } else {
    mFields.insert(std::make_pair(name, v));
  }
}


void VasyaSimpleObject::setField (VasyaVM *vm, long idx, VasyaValue *v) {
  char buf[128];
  sprintf(buf, "%ld", idx);
  setField(vm, buf, v);
}


////////////////////////////////////////////////////////////////////////////////
VasyaValue *VasyaPInhObject::getField (VasyaVM *vm, const std::string &name) {
  VasyaValue *res = VasyaSimpleObject::getField(vm, name);
  if (res) return res;
  VasyaObject *curO = this;
  const std::string prt("prototype");
  for (;;) {
    {
      VasyaFieldMap::iterator i = mFields.find(prt);
      if (i == mFields.end()) break;
      res = i->second;
    }
    if (!res || res->type != VasyaValue::object) break;
    curO = res->vobj;
    if (!curO) break;
    res = curO->getField(vm, name);
    if (res) return res;
  }
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
VasyaFunction::VasyaFunction () :
  VasyaRefCount(),
  fvararg(false),
  locCount(1),
  locTemps(0),
  argc(0)
{
}


VasyaFunction::~VasyaFunction () {
  for (unsigned int f = 0; f < lits.size(); f++) lits[f]->decRef();
  for (unsigned int f = 0; f < bcode.size(); f++) delete bcode[f];
}


////////////////////////////////////////////////////////////////////////////////
VasyaFDef::VasyaFDef () :
  mFName(), func(0), vars(0), prev(0)
{
  locs.insert(std::make_pair("<RETURN>", 0));
}


VasyaFDef::~VasyaFDef () {
  for (unsigned int f = 0; f < vars.size(); f++) delete vars[f];
  if (func) func->decRef();
}


short VasyaFDef::getTemp () {
  unsigned int f, len = ltused.size();
  for (f = 0; f < len; f++) {
    if (!ltused[f]) {
      ltused[f] = true;
      return (short)f+FIRST_TEMP;
    }
  }
  if (ltused.size() >= MAX_LOC_TEMP) return -1;
  f = len;
  ltused.push_back(true); // new used local
  return (short)f+FIRST_TEMP;
}


bool VasyaFDef::freeTemp (short idx) {
  bool res = false;
  if (idx >= FIRST_TEMP && idx-FIRST_TEMP < (short)ltused.size()) {
    res = ltused[idx-FIRST_TEMP];
    ltused[idx-FIRST_TEMP] = false;
    dirtyTempGlob(idx);
  }
  return res;
}


void VasyaFDef::setTempGlob (short idx, const std::string &name) {
  if (idx < 0 || idx-FIRST_TEMP >= (short)ltused.size() || name.empty()) return;
  if (idx >= FIRST_TEMP && !ltused[idx-FIRST_TEMP]) return;
  ltgm.erase(name);
  ltgm.insert(std::make_pair(name, idx));
}


short VasyaFDef::findTempGlob (const std::string &name) {
  std::map<std::string, int>::const_iterator i = ltgm.find(name);
  if (i != ltgm.end()) return i->second;
  return -1;
}


void VasyaFDef::dirtyTempGlob (const std::string &name) {
  ltgm.erase(name);
}


void VasyaFDef::dirtyTempGlob (short idx) {
  if (idx < 0 || idx-FIRST_TEMP >= (short)ltused.size()) return;
  bool done;
  do {
    done = true;
    std::map<std::string, int>::iterator i;
    for (i = ltgm.begin(); i != ltgm.end(); ++i) {
      if (i->second == idx) {
        ltgm.erase(i);
        done = false;
        break;
      }
    }
  } while (!done);
}


void VasyaFDef::tempMoved (short dst, short src) {
  short len = (short)ltused.size()+FIRST_TEMP;
  if (dst < FIRST_TEMP || dst >= len) return;
  if (dst != src) {
    dirtyTempGlob(dst);
    ltused[dst] = false;
    const std::string s;
    if (src >= 0 && src < len) {
      std::map<std::string, int>::const_iterator i;
      for (i = ltgm.begin(); i != ltgm.end(); ++i) if (i->second == src) break;
      if (i != ltgm.end()) {
        std::string n = i->first;
        freeTemp(src);
        ltgm.erase(n);
        ltgm.insert(std::make_pair(n, dst));
      } else {
        freeTemp(src);
      }
    }
  }
  ltused[dst-FIRST_TEMP] = true;
}


short VasyaFDef::allocVars (unsigned int cnt) {
  if (cnt < 1) return -1;
  unsigned int f, c, len = ltused.size();
  if (len >= cnt) {
    for (f = 0; f < len-cnt; f++) {
      if (!ltused[f]) {
        bool ok = true;
        for (c = f+1; c < f+cnt; c++) if (ltused[c]) { ok = false; break; }
        if (ok) {
          for (c = f; c < f+cnt; c++) {
            ltused[c] = true;
          }
          return (short)f+FIRST_TEMP;
        }
      }
    }
  }
  f = len;
  if (len > 0) {
    // find last free chunk
    for (f = len; f > 0; f--) if (ltused[f-1]) break;
  }
  int res = (int)f;
  for (; cnt > 0; cnt--, f++) {
    if (f >= ltused.size()) {
      if (ltused.size() >= MAX_LOC_TEMP-cnt) return -1;
      ltused.push_back(true); // new used local
    } else {
      ltused[f] = true;
    }
  }
  return res+FIRST_TEMP;
}


////////////////////////////////////////////////////////////////////////////////
VasyaVar::VasyaVar (VasyaFDef *owner) : var(false), field(false), mThis(false), name(""), locIdx(0), litIdx(0), func(owner) {
  if (owner) owner->vars.push_back(this);
}


////////////////////////////////////////////////////////////////////////////////
VasyaCC::VasyaCC (const std::string &fname) : scope(0) {
  mFBuf = new std::filebuf();
  mFBuf->open(fname.c_str(), std::ios::in);
  /*
  if (!mFBuf->opened()) {
    delete mFBuf;
    throw VasyaCCError();
  }
  */
  mInFile = new std::istream(mFBuf);
  mFName = fname;
  init();
}


VasyaCC::VasyaCC (std::istream *fl) : scope(0) {
  mInFile = fl;
  mFBuf = 0;
  mFName.clear();
  init();
}


VasyaCC::~VasyaCC () {
  if (mFBuf) mFBuf->close();
  delete mFBuf;
  delete mInFile;
}


void VasyaCC::init () {
  mCurLine = 1;
  tStr.clear();
  tNum = 0.0;
  tVec[0] = tVec[1] = 0.0;
  tType = ttEOF;
  tImmType = VasyaValue::vvoid;
  scope = 0;
  mUngot = 0;
}


#ifdef DEBUG_BUILD
static void dumpVV (char cn, const VasyaValue *v) {
  if (!v) {
    fprintf(stderr, "<NULL>");
    return;
  }
  switch (v->type) {
    case VasyaValue::number: fprintf(stderr, "%c=(%.15g)\t", cn, v->nval); break;
    case VasyaValue::string: fprintf(stderr, "%c=[%s]\t", cn, v->sval.c_str()); break;
    case VasyaValue::function: fprintf(stderr, "%c={func:%p}\t", cn, v->vfunc); break;
    default: ;
  }
}


#define DUMPLIT(cn, n) \
  if ((n) < 0 && (-(n))-1 < (int)fn->lits.size()) dumpVV(cn, fn->lits[(-(n))-1]); \
  else if ((n) == LOC_THIS) fprintf(stderr, "%c=THIS\t", cn); \
  else if ((n) == LOC_CALLEE) fprintf(stderr, "%c=CALLEE\t", cn); \
  else if ((n) >= LOC_CLOS) fprintf(stderr, "%c=[%d]\t", cn, (n)); \
  else if ((n) > 0 && (n) < fn->locCount) fprintf(stderr, "%c=<%d>\t", cn, (n));


void VasyaCC::dumpBC (const VasyaFunction *fn, int pc, int len) {
  fprintf(stderr, "===============\n");
  if (pc == 0 && len < 0) {
    fprintf(stderr, " literals: %u\n", fn->lits.size());
    fprintf(stderr, " locals: %u\n", fn->locCount);
    fprintf(stderr, " temps: %d\n", fn->locTemps);
  }
  for (unsigned int f = pc; f < fn->bcode.size(); f++) {
    const VasyaVMOp *s = fn->bcode[f];
    char buf[128];
    sprintf(buf, "%05u: %-16s  %d,%d,%d", f, opNames[s->op], s->a, s->b, s->c);
    fprintf(stderr, "%-42s| ", buf);
    if (s->op != VOP_RETURN) {
      DUMPLIT('a', s->a)
      DUMPLIT('b', s->b)
      DUMPLIT('c', s->c)
    }
    fputc('\n', stderr);
    if (len > 0) {
      if (--len == 0) break;
    }
  }
  fprintf(stderr, "---------------\n");
}
#else
void VasyaCC::dumpBC (const VasyaFunction *fn, int pc, int len) {}
#endif


int VasyaCC::emit (unsigned char opc, short a, short b, short c) {
  //fprintf(stderr, ">>%04X: %-16s  %d,%d,%d\n", bcode.size(), opNames[opc], a, b, c);
  VasyaVMOp *s = new VasyaVMOp;
  s->op = opc;
  s->a = a;
  s->b = b;
  s->c = c;
  s->line = mCurLine;
  scope->func->bcode.push_back(s);
  return (int)(scope->func->bcode.size())-1;
}


void VasyaCC::patchJumpChain (int cpc, int dest) {
  if (dest < 0) dest = getPC();
  while (cpc >= 0) {
    int ppc = scope->func->bcode[cpc]->b; // prev
    scope->func->bcode[cpc]->b = dest;
    cpc = ppc;
  }
}


void VasyaCC::showDieMsg (int line, const char *msg) {
  fprintf(stderr, "VERROR at file %s, line %d: %s\n", mFName.c_str(), line, msg);
}


void VASYA_NO_RETURN VASYA_LIKE_PRINTF(2, 3) VasyaCC::die (const char *msg, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(buf, sizeof(buf)-1, msg, ap);
  va_end(ap);
  showDieMsg(mCurLine, buf);
  throw VasyaCCError();
}


void VASYA_NO_RETURN VASYA_LIKE_PRINTF(3, 4) VasyaCC::dieAtLine (int line, const char *msg, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(buf, sizeof(buf)-1, msg, ap);
  va_end(ap);
  showDieMsg(line, buf);
  throw VasyaCCError();
}


void VasyaCC::showWarningMsg (int line, const char *msg) {
  fprintf(stderr, "VWARNING at file %s, line %d: %s\n", mFName.c_str(), line, msg);
}


void VASYA_LIKE_PRINTF(2, 3) VasyaCC::warning (const char *msg, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(buf, sizeof(buf)-1, msg, ap);
  va_end(ap);
  showWarningMsg(mCurLine, buf);
}


void VASYA_LIKE_PRINTF(3, 4) VasyaCC::warningAtLine (int line, const char *msg, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(buf, sizeof(buf)-1, msg, ap);
  va_end(ap);
  showWarningMsg(line, buf);
}


void VasyaCC::dieExpected (const char *extxt) {
  die("'%s' expected", extxt);
}


// fail on eof
char VasyaCC::getChar (bool eofFail) {
  if (mUngot) {
    int ch = mUngot;
    mUngot = 0;
    return ch;
  }
  int ch = mInFile->get();
  if (eofFail && ch == EOF) die("unexpected EOF");
  return ch;
}


void VasyaCC::ungetChar (char ch) {
  if (ch == EOF) return;
  if (mUngot) die("double ungetChar()");
  mUngot = ch;
}


void VasyaCC::parseString (char qch) {
  tStr.clear();
  if (!qch) qch = getChar();
  if (qch != '"') die("invalid string quote");
  for (;;) {
    char ch = getChar();
    if (ch == qch) break;
    if (ch == '\\') {
      ch = getChar();
      switch (ch) {
        case 't': tStr += '\t'; break;
        case 'r': tStr += '\r'; break;
        case 'n': tStr += '\n'; break;
        case '"': case '\'': case '\\': tStr += ch; break;
        default: die("invalid escape");
      }
    }
    if (ch == '\n') mCurLine++;
    tStr += ch; // SLOOOW!
  }
  tType = ttIMM;
  tImmType = VasyaValue::string;
}


void VasyaCC::parseNumber (char ch) {
  double frc = 0.1;
  bool inFrac = false, first = false;
  tNum = 0.0;
  if (!ch) ch = getChar();
  for (;;) {
    if (ch != '.' && (ch < '0' || ch > '9')) {
      if (first || isalpha(ch)) die("invalid number");
      ungetChar(ch);
      break;
    }
    if (ch == '.') {
      if (inFrac) die("invalid number");
      inFrac = true;
    } else {
      if (inFrac) {
        tNum += (ch-'0')*frc;
        frc /= 10.0;
      } else {
        tNum *= 10.0;
        tNum += (ch-'0');
      }
    }
    first = false;
    ch = getChar(false);
  }
  tType = ttIMM;
  tImmType = VasyaValue::number;
}


static inline int digitInBase (int ch, int base) {
  if (ch >= 'a' && ch <= 'z') ch -= 32; // upcase
  if (ch < '0' || (ch > '9' && ch < 'A') || ch > 'Z') return -2;
  ch -= '0'+(ch>'9'?7:0); // to digit
  return ch>=base ? -1 : ch;
}


void VasyaCC::parseBasedNumber (int base) {
  long n = 0;
  bool first = true;
  for (;;) {
    char ch = getChar(first);
    if (ch == EOF) break;
    int d = digitInBase(ch, base);
    if (d < 0) {
      if (first || d == -1) die("invalid number");
      ungetChar(ch);
      break;
    }
    first = false;
    n *= base;
    n += d;
  }
  tType = ttIMM;
  tImmType = VasyaValue::number;
  tNum = (double)n;
}


void VasyaCC::parseVector (char ch) {
  tVec[0] = tVec[1] = 0.0;
  if (!ch) ch = getChar();
  if (ch != '`') die("vector should start with '`'");
  ch = skipBlanks(true);
  for (int f = 0; f <= 1; f++) {
    bool neg = false;
    if (ch == '`') break;
    if (ch == '-') {
      neg = true;
      ch = skipBlanks(true);
    } else if (ch == '+') {
      ch = skipBlanks(true);
    }
    parseNumber(ch);
    if (neg) tNum = -tNum;
    tVec[f] = tNum;
    ch = getChar();
    if ((unsigned char)ch > ' ') break;
    ch = skipBlanks(true);
    if (ch != '`' && f) die("extra elements in vector");
  }
  if (ch != '`') die("vector should end with '`'");
  tType = ttIMM;
  tImmType = VasyaValue::vector;
}


int VasyaCC::skipBlanks (bool dieAtEOF) {
  for (;;) {
    int ch = getChar(false);
    if (ch == EOF) {
      if (dieAtEOF) die("unexpected EOF");
      return ch;
    }
    if (ch == '/') {
      int c1 = getChar(false);
      if (c1 == '/') {
        // comment till eol
        for (;;) {
          ch = getChar(false);
          if (ch == '\n') mCurLine++;
          if (ch == EOF || ch == '\n') break;
        }
      } else if (c1 == '*') {
        // comment till '*/'
        ch = ' ';
        int ncnt = 1;
        bool inString = false;
        for (;;) {
          c1 = ch;
          ch = getChar(false);
          if (ch == '\n') mCurLine++;
          if (ch == EOF) die("unclosed comment");
          if (inString) {
            if (ch == '\\') {
              ch = getChar(false);
              if (ch == EOF) die("unclosed comment");
            } else if (ch == '"') inString = false;
          } else {
            if (ch == '"') {
              inString = true;
            } else if (c1 == '*' && ch == '/') {
              if (--ncnt == 0) break;
            } else if (c1 == '/' && ch == '*') {
              ncnt++;
              ch = ' '; // to prevent /*/ comment closing
            } else if (c1 == '/' && ch == '/') {
              for (;;) {
                ch = getChar(false);
                if (ch == '\n') mCurLine++;
                if (ch == EOF || ch == '\n') break;
              }
              if (ch == EOF) die("unclosed comment");
              ch = ' ';
            }
          }
        }
      } else {
        if (c1 != EOF) ungetChar(c1);
        return ch;
      }
    } else {
      if (ch < 0 || ch > ' ') return ch;
      if (ch == '\n') mCurLine++;
    }
  }
}


void VasyaCC::nextToken () {
  tType = ttEOF;
  tStr.clear();
  int cc = skipBlanks(false);
  if (cc == EOF) return;
  char ch = cc;
  if (ch == '0') {
    int base = 0;
    char c1 = getChar(false);
    if (c1 == 'x' || c1 == 'X') base = 16;
    else if (c1 == 'o' || c1 == 'O') base = 8;
    else if (c1 == 'b' || c1 == 'B') base = 2;
    if (base) {
      parseBasedNumber(16);
      return;
    }
    ungetChar(c1);
  }
  if (ch >= '0' && ch <= '9') {
    parseNumber(ch);
    return;
  }
  if (ch == '.') {
    ch = getChar();
    if (ch >= '0' && ch <= '9') {
      ungetChar(ch);
      parseNumber('.');
      return;
    }
    tType = ttDEL;
    if (ch == '.') {
      ch = getChar();
      if (ch != '.') dieExpected("...");
      tStr = "...";
    } else {
      ungetChar(ch);
      tStr = ".";
    }
    return;
  }
  if (ch == '"') {
    parseString(ch);
    return;
  }
  if (ch == '`') {
    parseVector(ch);
    return;
  }
  if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch < 0) {
    tType = ttID;
    tStr += ch;
    for (;;) {
      ch = getChar(false);
      if (ch == EOF) break;
      if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch < 0) {
        tStr += ch;
        continue;
      }
      ungetChar(ch);
      break;
    }
    return;
  }
  if (mInFile->eof()) return;
  tType = ttDEL;
  tStr += ch;
  if (strchr("&|+-*=/<>!", ch)) {
    char c1 = getChar();
    if (c1 == '=') {
      tStr += c1;
      return;
    }
    ungetChar(c1);
  }
  if (ch == '&' || ch == '|' || ch == '+' || ch == '-') {
    char c1 = getChar();
    if (c1 == ch) {
      tStr += c1;
      return;
    }
    ungetChar(c1);
  }
}


int VasyaCC::newLiteral (VasyaValue *v) {
  int res = (int)scope->func->lits.size();
  scope->func->lits.push_back(v);
  v->incRef();
  return -1-res;
}


int VasyaCC::addNumLiteral (double v) {
  unsigned int f, len = scope->func->lits.size();
  for (f = 0; f < len; f++) {
    VasyaValue *vv = scope->func->lits[f];
    if (vv->type != VasyaValue::number) continue;
    if (vv->nval == v) break;
  }
  if (f >= len) {
    VasyaValue *vv = new VasyaValue(v);
    return newLiteral(vv);
  }
  return -1-((int)f);
}


int VasyaCC::addStrLiteral (const std::string &s) {
  unsigned int f, len = scope->func->lits.size();
  for (f = 0; f < len; f++) {
    VasyaValue *vv = scope->func->lits[f];
    if (vv->type != VasyaValue::string) continue;
    if (vv->sval == s) break;
  }
  if (f >= len) {
    VasyaValue *vv = new VasyaValue(s);
    return newLiteral(vv);
  }
  return -1-((int)f);
}


int VasyaCC::addVecLiteral (const double d0, const double d1) {
  unsigned int f, len = scope->func->lits.size();
  for (f = 0; f < len; f++) {
    VasyaValue *vv = scope->func->lits[f];
    if (vv->type != VasyaValue::vector) continue;
    if (vv->vval[0] == d0 && vv->vval[1] == d1) break;
  }
  if (f >= len) {
    VasyaValue *vv = new VasyaValue(d0, d1);
    return newLiteral(vv);
  }
  return -1-((int)f);
}


int VasyaCC::addVoidLiteral () {
  unsigned int f, len = scope->func->lits.size();
  for (f = 0; f < len; f++) {
    VasyaValue *vv = scope->func->lits[f];
    if (vv->type != VasyaValue::vvoid) continue;
  }
  if (f >= len) {
    VasyaValue *vv = new VasyaValue();
    return newLiteral(vv);
  }
  return -1-((int)f);
}


int VasyaCC::addLiteral () {
  switch (tImmType) {
    case VasyaValue::number: return addNumLiteral(tNum);
    case VasyaValue::string: return addStrLiteral(tStr);
    case VasyaValue::vector: return addVecLiteral(tVec[0], tVec[1]);
    default: ;
  }
  die("invalid immediate type");
  return 0; // shut up!
}


VasyaValue *VasyaCC::getLit (VasyaVar *v) {
  if (v->var) die("can't get literal out of variable");
  return scope->func->lits[(-v->litIdx)-1];
}


VasyaValue *VasyaCC::getLit (int idx) {
  if (idx >= 0) die("can't get literal out of variable");
  return scope->func->lits[(-idx)-1];
}


int VasyaCC::findLocal (const std::string &n, bool addClos) {
  VasyaFDef *fd = scope;
  // local?
  {
    VasyaLVarMap::const_iterator i = fd->locs.find(n);
    if (i != fd->locs.end()) return i->second;
  }
  // closed?
  {
    VasyaLVarMap::const_iterator i = fd->closIdx.find(n);
    if (i != fd->closIdx.end()) return i->second;
  }
  if (!addClos) return 0;
  // add new closed var
  int depth = 0; // -1 to compensate self
  for (fd = scope->prev; fd; fd = fd->prev, depth++) {
    VasyaLVarMap::const_iterator i = fd->locs.find(n);
    if (i == fd->locs.end()) continue;
    // add it
    int nn = (int)scope->closIdx.size();
    if (nn >= LOC_CLOS) die("too many closed variables");
    scope->closIdx.insert(std::make_pair(n, nn+LOC_CLOS));
    scope->clos.insert(std::make_pair(n, i->second));
    scope->closDepth.insert(std::make_pair(n, depth));
    return nn+LOC_CLOS;
  }
  return 0;
}


int VasyaCC::addLocal (const std::string &n) {
  int res = findLocal(n, false);
  if (res) return res;
  res = (int)scope->locs.size();
  if (res >= LOCS_MAX) die("too many local variables");
  scope->locs.insert(std::make_pair(n, res));
  return res;
}


short VasyaCC::giveTemp () {
  short res = scope->getTemp();
  if (res < 0) die("too many temporary locals");
  return res;
}


void VasyaCC::freeTemp (VasyaVar *v) {
  if (v->isLocal()) freeTemp(v->locIdx);
  else if (v->isField() && !v->litIdx) freeTemp(v->locIdx);
}


void VasyaCC::freeTemp (short idx) {
#ifdef VASYA_GEN_TEMPCLR
  if (scope->freeTemp(idx) && idx >= FIRST_TEMP && idx < LOC_CLOS) emit(VOP_TEMPCLR, idx);
#else
  scope->freeTemp(idx);
#endif
}


void VasyaCC::freeTempIfNot (VasyaVar *v, short idx) {
  if (v->locIdx != idx) freeTemp(v->locIdx);
}


short VasyaCC::loadValue (VasyaVar *v) {
  if (v->var) {
    if (v->isThis()) return LOC_THIS;
    if (v->field) {
      int t = giveTemp();
      int to = loadValue(v->obj);
      emit(VOP_FLOAD, t, to, v->litIdx ? v->litIdx : v->locIdx);
      freeTemp(to);
      return t;
    }
    if (v->name.empty()) return v->locIdx;
    // global
    int t = scope->findTempGlob(v->name);
    if (t < 0) {
      t = giveTemp();
      emit(VOP_LOADGLOB, t, addStrLiteral(v->name));
      scope->setTempGlob(t, v->name);
    }
    return t;
  }
  return v->litIdx;
}


void VasyaCC::storeValue (VasyaVar *v, short idx) {
  if (v->var) {
    if (v->field) {
      int to = loadValue(v->obj);
      emit(VOP_FSTORE, idx, to, v->litIdx ? v->litIdx : v->locIdx);
      freeTemp(to);
      return;
    }
    if (v->name.empty()) {
      if (v->locIdx != idx) emit(VOP_STORE, v->locIdx, idx);
      return;
    }
    // global
    int t = scope->findTempGlob(v->name);
    if (t < 0) {
      emit(VOP_STOREGLOB, addStrLiteral(v->name), idx);
      scope->setTempGlob(idx, v->name);
    }
    return;
  }
}


void VasyaCC::loadValueTo (short locIdx, VasyaVar *v) {
  if (v->var) {
    if (v->isThis()) {
      if (locIdx != LOC_THIS) emit(VOP_STORE, locIdx, LOC_THIS);
      scope->dirtyTempGlob(locIdx);
      return;
    }
    if (v->field) {
      int to = loadValue(v->obj);
      emit(VOP_FLOAD, locIdx, to, v->litIdx ? v->litIdx : v->locIdx);
      freeTemp(to);
    } else if (v->name.empty()) {
      if (locIdx != v->locIdx) emit(VOP_STORE, locIdx, v->locIdx);
    } else {
      // global
      int t = scope->findTempGlob(v->name);
      if (t < 0) {
        emit(VOP_LOADGLOB, locIdx, addStrLiteral(v->name));
        scope->setTempGlob(locIdx, v->name);
      } else {
        if (t != locIdx) emit(VOP_STORE, locIdx, t);
      }
    }
  } else {
    emit(VOP_STORE, locIdx, v->litIdx);
  }
}


VasyaVar *VasyaCC::loadVar (VasyaVar *v) {
  if (v->var) {
    int t;
    if (v->field) {
      t = loadValue(v);
    } else if (!v->name.empty()) {
      t = loadValue(v);
    } else {
      return v;
    }
    v = newVar();
    v->locIdx = t;
    v->var = true;
  }
  return v;
}


int VasyaCC::termConst () {
  return 0;
}


VasyaVar *VasyaCC::term (bool allowPM) {
  VasyaVar *e;
  if (tType == ttIMM) {
    int l = addLiteral();
    e = newVar();
    e->litIdx = l;
    nextToken();
    return e;
  }
  //
  if (tType == ttID) {
    bool allowID = true;
    if (tStr == "true" || tStr == "false") {
      int l = addNumLiteral(tStr=="true" ? 1.0 : 0.0);
      e = newVar();
      e->litIdx = l;
      nextToken();
    } else if (tStr == "void") {
      e = newVar();
      e->litIdx = addVoidLiteral();
      nextToken();
      allowID = false;
    } else if (tStr == "this") {
      nextToken();
      e = newVar();
      e->var = true;
      e->mThis = true;
      allowID = false;
    } else if (tStr == "function") {
      nextToken();
      std::string name;
      if (tType == ttID) {
        // can be named
        name = tStr;
        nextToken();
      }
      int lno = compileFuncDX(name);
      e = newVar();
      e->litIdx = lno;
      allowID = false;
    } else if (tStr == "typeof") {
      nextToken();
      if (tType != ttDEL || tStr != "(") dieExpected("(");
      VasyaVar *v = expr();
      if (v->isLit()) {
        VasyaValue *l = getLit(v);
        v->var = v->field = v->mThis = false;
        v->locIdx = 0;
        switch (l->type) {
          case VasyaValue::vvoid: v->litIdx = addStrLiteral("void"); break;
          case VasyaValue::number: v->litIdx = addStrLiteral("number"); break;
          case VasyaValue::string: v->litIdx = addStrLiteral("string"); break;
          case VasyaValue::vector: v->litIdx = addStrLiteral("vector"); break;
          case VasyaValue::function: v->litIdx = addStrLiteral("function"); break;
          case VasyaValue::object: v->litIdx = addStrLiteral("object"); break;
          default: die("internal error in 'typeof'");
        }
        e = v;
      } else {
        int t = giveTemp();
        loadValueTo(t, v);
        emit(VOP_TYPEOF, t, t);
        freeTempIfNot(v, t);
        e = v;
        e->var = true;
        e->field = false;
        e->locIdx = t;
        e->litIdx = 0;
        e->mThis = false;
      }
      allowID = false;
    } else if (!scope->mFName.empty() && tStr == scope->mFName) {
      // self
      nextToken();
      e = newVar();
      e->litIdx = LOC_CALLEE;
      allowID = false;
    } else {
      e = newVar();
      int lt = termConst();
      if (lt) {
        e->litIdx = lt;
        allowID = false;
      } else {
        e->var = true;
        e->locIdx = findLocal(tStr, true);
        if (e->locIdx == 0) e->name = tStr;
      }
      nextToken();
    }
    if (allowPM) {
      // postfix
      if (tType == ttDEL && (tStr == "++" || tStr == "--")) {
        if (!e->isVar() || !allowID) die("invalid inc/dec");
        bool doInc = tStr=="++";
        nextToken();
        if (e->isLocal()) {
          int ov = giveTemp();
          emit(VOP_STORE, ov, e->locIdx); // save old value
          emit(VOP_SUB, e->locIdx, e->locIdx, addNumLiteral(doInc ? -1.0 : 1.0));
          freeTemp(e->locIdx);
          e->locIdx = ov;
        } else {
          int nv = giveTemp();
          int t = loadValue(e);
          emit(VOP_SUB, nv, t, addNumLiteral(doInc ? -1.0 : 1.0));
          storeValue(e, nv);
          freeTemp(nv);
          e->locIdx = t;
        }
      }
    }
    return e;
  }
  //
  if (tType == ttDEL) {
    if (tStr == "(") {
      // cast?
      nextToken();
      if (tType == ttID) {
        if (tStr == "float" || tStr == "number") {
          nextToken();
          if (tStr != ")") dieExpected(")");
          nextToken();
          e = term(true);
          int nv = giveTemp();
          int t = loadValue(e);
          emit(VOP_SUB, nv, addNumLiteral(0.0), t);
          freeTemp(t);
          freeTempIfNot(e, t);
          e = newVar();
          e->var = true;
          e->locIdx = nv;
          return e;
        } else if(tStr == "string") {
          nextToken();
          if (tStr != ")") dieExpected(")");
          nextToken();
          e = term(true);
          int nv = giveTemp();
          int t = loadValue(e);
          emit(VOP_ADD, nv, addStrLiteral(""), t);
          freeTemp(t);
          freeTempIfNot(e, t);
          e = newVar();
          e->var = true;
          e->locIdx = nv;
          return e;
        }
      }
      e = expr();
      if (tStr != ")") dieExpected(")");
      nextToken();
      return e;
    }
    if (tStr == "{") {
      // object creation
      //TODO: json
      nextToken();
      int t = giveTemp();
      e = newVar();
      e->var = true;
      e->locIdx = t;
      emit(VOP_NEWOBJ, t);
      for (;;) {
        if (tType == ttDEL && tStr == "}") break;
        if (tType != ttID && tType != ttIMM) die("field name expected");
        std::string name = tStr;
        if (tType == ttIMM) {
          if (tImmType == VasyaValue::number) {
            char buf[128];
            sprintf(buf, "%ld", (long)tNum);
            tStr.clear();
            tStr += buf;
            tImmType = VasyaValue::string;
          }
          if (tImmType != VasyaValue::string) die("invalid field name");
        }
        nextToken();
        if (tType != ttDEL || (tStr != ":" && tStr != "=")) dieExpected(":");
        nextToken();
        VasyaVar *v = expr();
        //
        int fnl = addStrLiteral(name);
        int t = loadValue(v);
        emit(VOP_FSTORE, t, e->locIdx, fnl);
        freeTemp(t);
        //
        for (;;) {
          if (tType != ttDEL || (tStr != "," && tStr != ";")) break;
          nextToken();
        }
      }
      if (tType != ttDEL || tStr != "}") dieExpected("}");
      nextToken();
      return e;
    }
    if (allowPM) {
      if (tStr == "+" || tStr == "-") {
        int l = addNumLiteral(0.0);
        e = newVar();
        e->litIdx = l;
        return e;
      }
      // prefix
      if (tStr == "++" || tStr == "--") {
        bool doInc = tStr=="++";
        nextToken();
        e = term(false);
        if (!e->isVar() || e->isThis()) die("inc/dec expects variable");
        if (e->isLocal()) {
          // fast way
          emit(VOP_SUB, e->locIdx, e->locIdx, addNumLiteral(doInc ? -1.0 : 1.0));
        } else {
          // slow way
          int nv = giveTemp();
          int t = loadValue(e);
          emit(VOP_SUB, nv, t, addNumLiteral(doInc ? -1.0 : 1.0));
          storeValue(e, nv);
          freeTemp(t);
          e = newVar();
          e->var = true;
          e->locIdx = nv;
        }
        return e;
      }
    }
  }
  die("invalid term: %s", tStr.c_str());
  return 0; // shut up!
}


VasyaVar *VasyaCC::expr (int priority) {
  const OpcodeDef *opd;
  VasyaVar *op0, *op1;
  int endChain = -1, logRes = -1;
  if (priority < 0) priority = TOP_PRIORITY;
  if (priority == 0) return term(true);
  // unaries (move 'em out of here?)
  if (priority == 1 && tType == ttDEL) {
    if (tStr == "!") {
      nextToken();
      op0 = expr(priority);
      if (op0->isLit()) {
        op0->litIdx = addNumLiteral(getLit(op0)->isTrue() ? 0.0 : 1.0);
      } else {
        int t = (op0->isLocal() && op0->locIdx >= FIRST_TEMP) ? op0->locIdx : giveTemp();
        int t0 = loadValue(op0);
        emit(VOP_NOT, t, t0);
        scope->dirtyTempGlob(t);
        if (t0 != t) freeTemp(t0);
        if (op0->isLocal() && op0->locIdx == t) {
          // do nothing
        } else {
          op0 = newVar();
          op0->var = true;
          op0->locIdx = t;
        }
      }
      return op0;
    }
    if (tStr == "~") {
      int ln = mCurLine;
      nextToken();
      op0 = expr(priority);
      if (op0->isLit()) {
        long n = 0; double d = 0.0;
        VasyaValue *l = getLit(op0);
        switch (l->type) {
          case VasyaValue::number: n = (long)l->nval; break;
          case VasyaValue::string:
            if (VasyaVM::str2num(l->sval, &d) && isfinite(d)) {
              n = (long)d;
              break;
            }
          default:
            dieAtLine(ln, "invalid operand for '~'");
        }
        op0->litIdx = addNumLiteral(~n);
      } else {
        int t = (op0->isLocal() && op0->locIdx >= FIRST_TEMP) ? op0->locIdx : giveTemp();
        int t0 = loadValue(op0);
        emit(VOP_BITNOT, t, t0);
        scope->dirtyTempGlob(t);
        if (t0 != t) freeTemp(t0);
        if (op0->isLocal() && op0->locIdx == t) {
          // do nothing
        } else {
          op0 = newVar();
          op0->var = true;
          op0->locIdx = t;
        }
      }
      return op0;
    }
  }
  //
  op0 = expr(priority-1);
  for (;;) {
    if (tType != ttDEL) break; // not an operator
    if (priority == 1 && tStr == "(") {
      // function call
      if (!op0->var && op0->litIdx != LOC_CALLEE) {
        if (scope->func->lits[(-op0->litIdx)-1]->type != VasyaValue::function) die("function expected");
      }
      nextToken();
      // collect arguments
      std::vector<int> atmp;
      if (tType != ttDEL || tStr != ")") {
        for (;;) {
          op1 = expr();
          if (tType != ttDEL || (tStr != ")" && tStr != ",")) dieExpected(")");
          int tt = loadValue(op1);
          atmp.push_back(tt);
          freeTempIfNot(op1, tt);
          if (tStr == ",") nextToken(); else break;
        }
      }
      if (tType != ttDEL || tStr != ")") dieExpected(")");
      nextToken();
      int rt = scope->allocVars(atmp.size()+1); // res
      if (rt < 0) die("out of temp vars");
      // set function
      int self = 0;
      if (op0->var && op0->field) {
        // method call
        self = loadValue(op0->obj);
        emit(VOP_FLOAD, rt, self, op0->litIdx ? op0->litIdx : op0->locIdx);
      } else {
        // normal call
        if (!op0->var && op0->litIdx == LOC_CALLEE) self = LOC_THIS; // keep 'this'
        loadValueTo(rt, op0);
        if (op0->var && op0->locIdx != rt) freeTemp(op0);
      }
      int art = rt+1;
      for (unsigned int f = 0; f < atmp.size(); f++, art++) {
        int src = atmp[f];
        if (art != src) {
          emit(VOP_STORE, art, src); // copy it
          freeTemp(src);
        }
        // can free it right here
        freeTemp(art);
      }
      //
      emit(VOP_CALL, rt, (int)atmp.size(), self);
      if (self && self >= FIRST_TEMP) freeTemp(self);
      scope->dirtyTempGlob(rt);
      op0 = newVar();
      op0->var = true;
      op0->locIdx = rt;
      //
      continue;
    }
    if (priority == 1 && tStr == ".") {
      // op0: object
      if (!op0->var) {
        VasyaValue *lit = getLit(op0);
        if (lit->type != VasyaValue::string && lit->type != VasyaValue::vector) die("this literal has no fields");
      }
      nextToken();
      if (tType != ttID) die("field name expected");
      op1 = newVar();
      op1->var = true;
      op1->field = true;
      op1->obj = loadVar(op0);
      op1->litIdx = addStrLiteral(tStr);
      op0 = op1;
      nextToken();
      continue;
    }
    if (priority == 1 && tStr == "[") {
      nextToken();
      op1 = expr();
      if (tType != ttDEL || tStr != "]") dieExpected("]");
      int t0 = loadValue(op1);
      op1 = newVar();
      op1->var = true;
      op1->field = true;
      op1->obj = loadVar(op0);
      op1->locIdx = t0;
      op0 = op1;
      nextToken();
      continue;
    }
    //
    if (tStr == "?") {
      // ternary
      if (priority != 6) break;
      int t = loadValue(op0), tr;
      freeTempIfNot(op0, t);
      op0 = newVar();
      op0->var = true;
      op0->locIdx = t;
      int fpc = emit(VOP_JMPF, t, -1);
      //freeTemp(t); // we'll store result here
      nextToken(); // skip '?'
      // true node
      op1 = expr();
      // copy this to t
      tr = loadValue(op1);
      if (tr != t) {
        emit(VOP_STORE, t, tr);
        freeTemp(tr);
      }
      freeTempIfNot(op1, t);
      int epc = -1;
      if (fpc == getPC()-1) {
        // no code at all
        scope->func->bcode[fpc]->op = VOP_JMPT;
        epc = fpc;
      } else {
        epc = emit(VOP_JUMP, 0, -1);
        patchJumpChain(fpc);
      }
      //
      if (tType != ttDEL || tStr != ":") dieExpected(":");
      nextToken(); // skip ':'
      // false node
      op1 = expr();
      // copy this to t
      tr = loadValue(op1);
      if (tr != t) {
        fpc = emit(VOP_STORE, t, tr);
        freeTemp(tr);
      }
      freeTempIfNot(op1, t);
      patchJumpChain(epc);
      scope->dirtyTempGlob(t);
      continue;
    }
    for (opd = opcodes; opd->name; opd++) if (opd->priority == priority && tStr == opd->name) break;
    if (!opd->name) break;
    // FIXME: a = 5; a += (a += 2); should give 12, not 14
    //        i can do this, but not sure if it is right: it will complicate the code
    if (opd->rassoc) {
      // assignment
      if (!op0->var) die("invalid lvalue");
    }
    nextToken(); // skip operator
    if (opd->op == VOP_STORE && tType == ttDEL && tStr == "...") {
      nextToken();
      op1 = newVar();
      op1->var = true;
      op1->locIdx = LOC_ARGS;
    } else {
      op1 = expr(priority-(opd->rassoc?0:1));
    }
    if (opd->rassoc) {
      // assignment
      int tvx = 0;
      if (opd->op != VOP_STORE) {
        // math-assign
        if (!op0->field && op0->name.empty()) {
          // op0: temp/local
          if ((op1->var && !op1->field && op1->name.empty()) || !op1->var) {
            // op1: local/literal
            emit(opd->op, op0->locIdx, op0->locIdx, op1->var?op1->locIdx:op1->litIdx);
            freeTempIfNot(op1, op0->locIdx);
          } else {
            // op1: other
            int ov = loadValue(op1);
            emit(opd->op, op0->locIdx, op0->locIdx, ov);
            freeTemp(ov);
          }
          scope->dirtyTempGlob(op0->locIdx); // just in case
          continue;
        }
        // global/field
        tvx = giveTemp();
        int ov = loadValue(op0);
        int t1 = loadValue(op1);
        emit(opd->op, tvx, ov, t1);
        freeTemp(ov);
        freeTemp(t1);
        // don't make dirty, 'cause next assignment will fix the value
      }
      if (op0->field) {
        // field
        if (!tvx) tvx = loadValue(op1);
        int to = loadValue(op0->obj);
        emit(VOP_FSTORE, tvx, to, op0->litIdx ? op0->litIdx : op0->locIdx);
        freeTemp(to);
      } else {
        if (!op0->name.empty()) {
          // global
          if (!tvx) tvx = loadValue(op1);
          scope->dirtyTempGlob(op0->name);
          emit(VOP_STOREGLOB, addStrLiteral(op0->name), tvx);
          op0 = newVar();
          op0->var = true;
          op0->locIdx = tvx;
          scope->setTempGlob(tvx, op0->name);
          tvx = 0;
        } else {
          if (tvx) die("internal error 100");
          // local
          if ((op1->var && !op1->field && op1->name.empty()) || !op1->var) {
            // op1: local/literal
            if (op0->locIdx != op1->locIdx) {
              emit(VOP_STORE, op0->locIdx, op1->var?op1->locIdx:op1->litIdx);
              if (op1->var) scope->tempMoved(op0->locIdx, op1->locIdx);
            }
          } else {
            // op1: other
            loadValueTo(op0->locIdx, op1);
            scope->dirtyTempGlob(op0->locIdx);
            freeTemp(op1);
          }
        }
      }
      if (tvx) {
        op0 = newVar();
        op0->var = true;
        op0->locIdx = tvx;
      }
    } else {
      if (opd->op == VOPX_AND || opd->op == VOPX_OR) {
        // boolean; do short-circuit evaluation
        if (logRes < 0) {
          // first result
          int t0 = loadValue(op0);
          if (t0 < FIRST_TEMP) {
            logRes = giveTemp();
            emit(VOP_STORE, logRes, t0);
          } else {
            logRes = t0;
          }
        } else {
          // other results
          loadValueTo(logRes, op0);
          freeTempIfNot(op0, logRes);
        }
        // short-circuit jump
        endChain = emit(opd->op==VOPX_AND ? VOP_JMPF : VOP_JMPT, logRes, endChain);
        op0 = op1;
      } else {
        // normal math
        bool opDone = false;
        if (op0->isLit() && op1->isLit()) {
          // we can do math right here
          switch (opd->op) {
            case VOP_ADD:
            case VOP_SUB:
            case VOP_MUL:
            case VOP_DIV:
            case VOP_MOD:
            case VOP_BITAND:
            case VOP_BITOR:
            case VOP_BITXOR: {
              int nidx = addNumLiteral(NAN);
              VasyaValue *vr = VasyaVM::bmath(opd->op, getLit(op0->litIdx), getLit(op1->litIdx), getLit(nidx));\
              if (!vr) die("invalid math");
              switch (vr->type) {
                case VasyaValue::string:
                  op0 = newVar();
                  op0->litIdx = addStrLiteral(vr->sval);
                  opDone = true;
                  break;
                case VasyaValue::number:
                  op0 = newVar();
                  op0->litIdx = addNumLiteral(vr->nval);
                  opDone = true;
                  break;
                case VasyaValue::vector:
                  op0 = newVar();
                  op0->litIdx = addVecLiteral(vr->vval[0], vr->vval[1]);
                  opDone = true;
                  break;
                default:
                  vr->decRef();
                  break;
              }
              vr->decRef();
              } break;
            case VOP_EQ:
            case VOP_NE:
            case VOP_LE:
            case VOP_GE:
            case VOP_LT:
            case VOP_GT: {
              bool cr = VasyaVM::cmpmath(opd->op, getLit(op0->litIdx), getLit(op1->litIdx));
              op0 = newVar();
              op0->litIdx = addNumLiteral(cr ? 1.0 : 0.0);
              opDone = true;
              } break;
            default: ;
          }
        }
        if (!opDone) {
          int t =
            (op0->isLocal() && op0->locIdx >= FIRST_TEMP) ?
              op0->locIdx :
              (op1->isLocal() && op1->locIdx >= FIRST_TEMP) ?
                op1->locIdx :
                giveTemp();
          int t0 = loadValue(op0);
          int t1 = loadValue(op1);
          emit(opd->op, t, t0, t1);
          scope->dirtyTempGlob(t);
          if (t0 != t) freeTemp(t0);
          if (t1 != t) freeTemp(t1);
          if (op0->isLocal() && op0->locIdx == t) {
            // do nothing
          } else if (op1->isLocal() && op1->locIdx == t) {
            op0 = op1;
          } else {
            op0 = newVar();
            op0->var = true;
            op0->locIdx = t;
          }
        }
      }
    }
  }
  if (logRes > 0) {
    // was some short-circuit jumps
    // store final value
    int t0 = loadValue(op0);
    if (logRes != t0) {
      emit(VOP_STORE, logRes, t0);
      freeTemp(t0);
    }
    op0 = newVar();
    op0->locIdx = logRes;
    op0->var = true;
    // patch jump chain
    patchJumpChain(endChain);
  }
  return op0;
}


VasyaVar *VasyaCC::newVar () {
  return new VasyaVar(scope);
}


void VasyaCC::newExpr () {
  scope->vars.push_back(0);
}


void VasyaCC::endExpr () {
  while (scope->vars.size() > 0) {
    VasyaVar *v = scope->vars[scope->vars.size()-1];
    scope->vars.pop_back();
    if (v) delete v; else break;
  }
}


int VasyaCC::compileExpression () {
  newExpr();
  int t = loadValue(expr());
  endExpr();
  return t;
}


void VasyaCC::compileFuncBody (bool cbTaken) {
  if (!cbTaken) {
    if (tType != ttDEL || tStr != "{") dieExpected("{");
    nextToken();
  }
  for (;;) {
    if (tType == ttEOF) {
      if (cbTaken) break;
      die("unexpected end of file");
    }
    if (tType == ttDEL && tStr == "}") break;
    compileStatement();
  }
  emit(VOP_RETURN, -1);
  removeUnusedLits();
  VasyaFunction *fn = scope->func;
  fn->locTemps = (int)scope->ltused.size();
  // closed vars
  fn->locCount = (int)scope->locs.size();
  for (VasyaLVarMap::const_iterator i = scope->clos.begin(); i != scope->clos.end(); ++i) {
    VasyaLVarMap::const_iterator in = scope->closDepth.find(i->first);
    if (in == scope->closDepth.end()) die("internal error with closures");
    fn->clos.push_back(std::make_pair(in->second, i->second));
  }
  dumpBC(scope->func);
  nextToken();
}


void VasyaCC::compileBlock () {
  if (tType != ttDEL || tStr != "{") dieExpected("{");
  nextToken();
  for (;;) {
    if (tType == ttDEL && tStr == "}") break;
    compileStatement();
  }
  nextToken();
}


// returns literal number
int VasyaCC::compileFuncDX (const std::string &fname) {
  // function
  nextToken(); // skip '('
  VasyaFDef *fd = new VasyaFDef();
  fd->mFName = fname;
  fd->prev = scope;
  scope = fd;
  fd->func = new VasyaFunction();
  // collect argument names
  if (tType != ttDEL || tStr != ")") {
    for (;;) {
      if (tType == ttDEL && tStr == "...") {
        nextToken();
        break;
      }
      if (tType != ttID) dieExpected(")");
      if (findLocal(tStr, false)) die("argument redefinition");
      addLocal(tStr);
      scope->func->argc++;
      nextToken(); // skip name
      if (tType != ttDEL) dieExpected(")");
      if (tStr == ")") break;
      if (tStr != ",") dieExpected(")");
      nextToken(); // skip comma
    }
  }
  if (tType != ttDEL || tStr != ")") dieExpected(")");
  nextToken();
  compileFuncBody(false);
  VasyaValue *vv = new VasyaValue(scope->func);
  scope->func = 0;
  removeScope();
  // register it
  return newLiteral(vv);
}


// new local(s)
void VasyaCC::compileLocal () {
  nextToken(); // skip 'local'
  for (;;) {
    if (tType != ttID) die("variable name expected");
    int ln = findLocal(tStr, false);
    if (ln) warning("local redefinition: %s", tStr.c_str());
    std::string name = tStr;
    nextToken();
    if (tType == ttDEL) {
      int t = 0;
      bool setT = true;
      if (tStr == "(") {
        // function
        t = compileFuncDX(name);
      } else if (tStr == "=") {
        // initializer
        nextToken(); // skip '='
        if (tType == ttDEL && tStr == "...") {
          nextToken();
          t = LOC_ARGS;
        } else {
          t = compileExpression();
        }
      } else setT = false;
      if (!ln) ln = addLocal(name);
      if (setT) emit(VOP_STORE, ln, t);
      freeTemp(t);
    }
    if (tType != ttDEL) dieExpected(";");
    if (tStr == ";") break;
    if (tStr != ",") dieExpected(";");
    nextToken();
  }
  matchSemi();
}


void VasyaCC::compileReturn () {
  nextToken(); // skip 'return'
  if (tType != ttDEL || tStr != ";") {
    int t = compileExpression();
    emit(VOP_STORE, 0, t);
    freeTemp(t);
    emit(VOP_RETURN, 0);
  } else {
    emit(VOP_RETURN, -1);
  }
  matchSemi();
}


void VasyaCC::compileIf () {
  nextToken(); // skip 'if'
  if (tType != ttDEL || tStr != "(") dieExpected("(");
  int t = compileExpression();
  freeTemp(t);
  int cond = isLit(t) ? (getLit(t)->isTrue() ? 1 : -1) : 0; // -1: false; 1: true; 0: undefined
  int fpc = -1, spc = getPC(), wasTN = 0;
  if (!cond) fpc = emit(VOP_JMPF, t, -1); // only for undefined conditions
  // true node
  compileStatement();
  if (cond < 0) removeCode(spc); // remove code for known condition
  else {
    if (!(wasTN = getPC() > fpc+1)) {
      // there were no code for 'true' node, remove jump
      removeCode(fpc);
    }
  }
  if (tType == ttID && tStr == "else") {
    // false node
    spc = getPC();
    int tpc = wasTN ? emit(VOP_JUMP, 0, -1) : -1;
    patchJumpChain(fpc);
    fpc = tpc;
    nextToken();
    compileStatement();
    if (cond > 0) {
      removeCode(spc);
      fpc = -1;
    }
  }
  patchJumpChain(fpc);
}


void VasyaCC::compileBreak () {
  if (!allowBC) die("'break' without loop");
  nextToken(); // skip 'break'
  matchSemi();
  brkPC = emit(VOP_JUMP, 0, brkPC);
}


void VasyaCC::compileContinue () {
  if (!allowBC) die("'continue' without loop");
  nextToken(); // skip 'continue'
  matchSemi();
  if (cpcValid) {
    emit(VOP_JUMP, 0, contPC);
  } else {
    contPC = emit(VOP_JUMP, 0, contPC);
  }
}


void VasyaCC::compileDo () {
  int oBPC = brkPC, oCPC = contPC;
  bool oABC = allowBC, oCV = cpcValid;
  int stLine = mCurLine;
  nextToken(); // skip 'do'
  brkPC = -1;
  contPC = -1;
  allowBC = true;
  cpcValid = false;
  int startPC = getPC();
  // body
  compileStatement();
  // condition
  patchJumpChain(contPC);
  if (tType != ttID || tStr != "while") dieExpected("while");
  nextToken();
  if (tType != ttDEL || tStr != "(") dieExpected("(");
  //
  int t = compileExpression();
  freeTemp(t);
  if (isLit(t)) {
    if (getLit(t)->isTrue()) {
      if (brkPC == -1) warningAtLine(stLine, "endless loop detected");
      emit(VOP_JUMP, 0, startPC);
    }
  } else {
    emit(VOP_JMPT, t, startPC);
  }
  // fix jumps
  patchJumpChain(brkPC);
  brkPC = oBPC;
  contPC = oCPC;
  allowBC = oABC;
  cpcValid = oCV;
}


void VasyaCC::compileWhile () {
  int oBPC = brkPC, oCPC = contPC;
  bool oABC = allowBC, oCV = cpcValid;
  int stLine = mCurLine;
  nextToken(); // skip 'while'
  brkPC = -1;
  contPC = getPC();
  allowBC = true;
  cpcValid = true;
  // condition
  if (tType != ttDEL || tStr != "(") dieExpected("(");
  int spc = getPC();
  int t = compileExpression();
  freeTemp(t);
  int cond = isLit(t) ? (getLit(t)->isTrue() ? 1 : -1) : 0; // -1: false; 1: true; 0: undefined
  if (!cond) brkPC = emit(VOP_JMPF, t, brkPC); else removeCode(spc);
  // body
  compileStatement();
  // again
  switch (cond) {
    case -1:
      removeCode(spc);
      break;
    case 1:
      if (brkPC == -1) warningAtLine(stLine, "endless loop detected");
    default:
      emit(VOP_JUMP, 0, contPC);
      break;
  }
  // fix jumps
  patchJumpChain(brkPC);
  brkPC = oBPC;
  contPC = oCPC;
  allowBC = oABC;
  cpcValid = oCV;
}


void VasyaCC::compileFor () {
  int oBPC = brkPC, oCPC = contPC;
  bool oABC = allowBC, oCV = cpcValid;
  int stLine = mCurLine;
  nextToken(); // skip 'for'
  brkPC = -1;
  //contPC = getPC();
  allowBC = true;
  cpcValid = true;
  if (tType != ttDEL || tStr != "(") dieExpected("(");
  nextToken();
  // initializer
  if (tType == ttDEL && tStr == ";") {
    // nothing to initialize
    nextToken();
  } else {
    if (tType == ttID && tStr == "local") {
      compileLocal();
    } else {
      for (;;) {
        freeTemp(compileExpression());
        if (tType == ttDEL && tStr == ",") {
          nextToken();
          continue;
        }
        break;
      }
      matchSemi();
    }
  }
  // condition
  bool noCond = false;
  contPC = getPC();
  if (tType == ttDEL && tStr == ";") {
    // nothing to check
    nextToken();
    noCond = true;
  } else {
    int lastT = -1;
    for (;;) {
      lastT = compileExpression();
      freeTemp(lastT);
      if (tType == ttDEL && tStr == ",") {
        nextToken();
        continue;
      }
      break;
    }
    matchSemi();
    brkPC = emit(VOP_JMPF, lastT, brkPC); // exit if false
  }
  // increment
  int ccJump = -1;
  int incrPC = getPC();
  if (tType == ttDEL && tStr == ")") {
    // nothing to increment
    ccJump = contPC;
  } else {
    int tJP = -1;
    if (incrPC != contPC) {
      // skip it for now
      tJP = emit(VOP_JUMP, 0, -1);
    }
    ccJump = getPC();
    for (;;) {
      freeTemp(compileExpression());
      if (tType == ttDEL && tStr == ",") {
        nextToken();
        continue;
      }
      break;
    }
    if (tType != ttDEL || tStr != ")") dieExpected(")");
    if (incrPC != contPC) emit(VOP_JUMP, 0, contPC);
    patchJumpChain(tJP);
    contPC = ccJump;
  }
  nextToken();
  // body
  compileStatement();
  if (noCond && brkPC == -1) warningAtLine(stLine, "endless loop detected");
  // again
  emit(VOP_JUMP, 0, contPC);
  // fix jumps
  patchJumpChain(brkPC);
  brkPC = oBPC;
  contPC = oCPC;
  allowBC = oABC;
  cpcValid = oCV;
}


void VasyaCC::compileStatement () {
  if (tType == ttDEL) {
    if (tStr == "{") {
      compileBlock();
      return;
    }
    if (tStr == ";") {
      matchSemi();
      return;
    }
  }
  if (tType != ttID) {
    if (tType == ttDEL && tStr == "(") {
      freeTemp(compileExpression());
      matchSemi();
      return;
    }
    die("identifier expected");
  }
  if (tStr == "local") compileLocal();
  else if (tStr == "return") compileReturn();
  else if (tStr == "if") compileIf();
  else if (tStr == "else") die("unexpected 'else'");
  else if (tStr == "do") compileDo();
  else if (tStr == "while") compileWhile();
  else if (tStr == "for") compileFor();
  else if (tStr == "break") compileBreak();
  else if (tStr == "continue") compileContinue();
  else if (tStr == "function") {
    nextToken();
    if (tType != ttID) die("function name expected");
    std::string name = tStr;
    int ln = findLocal(name, false);
    if (ln) {
      warning("local redefinition");
    } else {
      ln = addLocal(name);
    }
    nextToken();
    int lno = compileFuncDX(name);
    emit(VOP_STORE, ln, lno);
  } else {
    freeTemp(compileExpression());
    matchSemi();
  }
}


void VasyaCC::matchSemi () {
  if (tType != ttDEL || tStr != ";") dieExpected(";");
  nextToken();
}


void VasyaCC::removeScope () {
  VasyaFDef *fd = scope;
  scope = fd->prev;
  delete fd;
}


void VasyaCC::removeCode (int oldPC) {
  if (oldPC < 0) oldPC = 0;
  while (getPC() > oldPC) {
    VasyaVMOp *o = scope->func->bcode.back();
    delete o;
    scope->func->bcode.pop_back();
  }
}


// remove unused literals, fix the code
void VasyaCC::removeUnusedLits () {
  VasyaFunction *fn = scope->func;
  std::set<int> used;
  // register all used literals, build map
  //vlog("old # of literals: %u\n", fn->lits.size());
  std::map<int, int> o2n; // src, dst
  std::vector<VasyaValue *> newlits;
  for (unsigned int f = 0; f < fn->bcode.size(); f++) {
    const VasyaVMOp *op = fn->bcode[f];
    if (op->op == VOP_RETURN) continue;
    for (int c = 0; c < 3; c++) {
      int ln = op->args[c];
      if (ln >= 0) continue;
      std::set<int>::iterator i = used.find(ln);
      if (i != used.end()) continue;
      used.insert(ln);
      int oln = -(ln+1), nln = -((int)newlits.size()+1);
      newlits.push_back(fn->lits[oln]);
      fn->lits[oln]->incRef();
      o2n.insert(std::make_pair(oln, nln));
    }
  }
  if (newlits.size() == fn->lits.size()) {
    //vlog("all literals used\n");
    for (unsigned int f = 0; f < newlits.size(); f++) newlits[f]->decRef();
    return;
  }
  // now fix the code
  for (unsigned int f = 0; f < fn->bcode.size(); f++) {
    VasyaVMOp *op = fn->bcode[f];
    if (op->op == VOP_RETURN) continue;
    for (int c = 0; c < 3; c++) {
      int ln = op->args[c];
      if (ln >= 0) continue;
      int oln = -(ln+1);
      std::map<int, int>::const_iterator i = o2n.find(oln);
      if (i == o2n.end()) die("internal error in literal optimizer");
      op->args[c] = i->second;
    }
  }
  // now move literals
  for (unsigned int f = 0; f < fn->lits.size(); f++) fn->lits[f]->decRef();
  fn->lits.clear();
  fn->lits = newlits;
  //vlog("new # of literals: %u\n", fn->lits.size());
}


VasyaFunction *VasyaCC::compileBody () {
  brkPC = -1;
  contPC = -1;
  allowBC = false;
  cpcValid = false;
  VasyaFunction *res;
  VasyaFDef *fd = new VasyaFDef();
  fd->prev = scope;
  scope = fd;
  res = fd->func = new VasyaFunction();
  try {
    nextToken();
    compileFuncBody(tType!=ttDEL || tStr != "{");
  } catch (VasyaCCError) {
    while (scope) removeScope();
    throw;
  }
  fd->func = 0;
  removeScope();
  return res;
}


////////////////////////////////////////////////////////////////////////////////
#define BCC  1024
VasyaVM::VasyaVM () :
  mRStack(0),
  mWorld(new VasyaSimpleObject()),
  mWorldVV(new VasyaValue(mWorld)),
  mZero(new VasyaValue(0.0)),
  mOne(new VasyaValue(1.0)),
  mNaN(new VasyaValue(NAN)),
  mVoid(new VasyaValue()),
  mBreakCheckCount(BCC),
  mLastLine(0),
  mLastPC(0)
{
  mWorldVV->incRef();
  mZero->incRef();
  mOne->incRef();
  mNaN->incRef();
  mVoid->incRef();
  addFunc("apply", new VasyaFunctionApply());
  addFunc("call", new VasyaFunctionCall());
}


VasyaVM::~VasyaVM () {
  mWorldVV->decRef();
  while (mRStack) dropR();
  mZero->decRef();
  mOne->decRef();
  mNaN->decRef();
  mVoid->decRef();
}


void VasyaVM::showDieMsg (const char *msg) {
  fprintf(stderr, "VVMERROR at line %d, PC=%04d: %s\n", mLastLine, mLastPC, msg);
}


void VASYA_NO_RETURN VASYA_LIKE_PRINTF(2, 3) VasyaVM::die (const char *msg, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(buf, sizeof(buf)-1, msg, ap);
  va_end(ap);
  showDieMsg(buf);
  throw VasyaVMError();
}


void VasyaVM::addFunc (const std::string &name, VasyaFunction *fn) {
  if (!fn) setGlobal(name, 0);
  else {
    VasyaValue *v = new VasyaValue(fn);
    setGlobal(name, v);
  }
}


VasyaValue *VasyaVM::getGlobal (const std::string &name) {
  return mWorld->getField(this, name);
}


void VasyaVM::setGlobal (const std::string &name, VasyaValue *v) {
  mWorld->setField(this, name, v);
}


VasyaValue *VasyaVM::execute (VasyaFunction *fn) {
  return apply(fn, NULL, 0, NULL);
}


VasyaValue *VasyaVM::apply (VasyaFunction *fn, VasyaValue *aThis, int argc, VasyaValue **args) {
  if (!fn) die("VasyaVM: calling non-function");
  VasyaValue *ff = new VasyaValue(fn);
  addRStop();
  addRFunc(ff, -1, aThis?aThis:mWorldVV);
  RStackFrame *fr = mRStack;
  if (fn->isNative()) for (int f = 0; f <= argc; f++) fr->locs.push_back(0);
  for (int f = 0; f < argc; f++) {
    VasyaValue *vb = args[f];
    if (fn->isNative() || f < fn->argc) setLocal(f+1, vb);
    vb->incRef();
    fr->args.push_back(vb);
  }
  if (!fn->isNative()) {
    for (int f = argc; f < fn->argc; f++) setLocal(f+1, mVoid);
  }
  try {
    if (fn->isNative()) {
      VasyaValue *res = fn->exec(this, 1, argc, aThis?aThis:mWorldVV);
      if (!res) res = mVoid;
      res->incRef();
      dropR();
      dropR();
      res->decrementRef();
      return res;
    } else {
      return run();
    }
  } catch (...) {
    for (bool done = false; mRStack && !done; ) {
      done = !mRStack->func;
      dropR();
    }
    throw;
  }
}


VasyaValue *VasyaVM::dropR () {
  VasyaValue *res = 0;
  RStackFrame *fr = mRStack;
  if (!fr) die("VasyaVM: internal error 000");
  mRStack = fr->prev;
  if (fr->locs.size() > 0) res = fr->locs[0]; //fr->locs[0] = 0;
  for (unsigned int f = 1; f < fr->locs.size(); f++) if (fr->locs[f]) fr->locs[f]->decRef();
  for (unsigned int f = 0; f < fr->locTemps.size(); f++) if (fr->locTemps[f]) fr->locTemps[f]->decRef();
  for (unsigned int f = 0; f < fr->args.size(); f++) fr->args[f]->decRef();
  if (fr->vThis) fr->vThis->decRef();
  if (fr->func) fr->func->decRef();
  //fr->func = 0;
  delete fr;
  return res;
}


void VasyaVM::addRFunc (VasyaValue *fn, int resDL, VasyaValue *aThis) {
  if (!fn || fn->type != VasyaValue::function) die("can't execute non-function");
  RStackFrame *fr = new RStackFrame;
  fr->prev = mRStack;
  mRStack = fr;
  fn->incRef();
  fr->func = fn;
  fr->resDestLoc = resDL;
  fr->pc = 0;
  if (aThis) {
    fr->vThis = aThis;
    aThis->incRef();
  } else fr->vThis = 0;
  if (!fn->vfunc->isNative()) {
    for (int f = fn->vfunc->locTemps; f > 0; f--) fr->locTemps.push_back(0);
    for (int f = fn->vfunc->locCount; f > 0; f--) fr->locs.push_back(0);
  }
}


void VasyaVM::addRStop () {
  RStackFrame *fr = new RStackFrame;
  fr->func = 0;
  fr->prev = mRStack;
  mRStack = fr;
}


VasyaValue *VasyaVM::getLocal (int ln, int depth) {
  RStackFrame *fr = mRStack;
  for (; depth > 0; fr = fr->prev, depth--) ;
  if (ln < 0) {
    // literal
    VasyaValue *res = fr->func->vfunc->lits[(-ln)-1];
    if (res->type == VasyaValue::function && res->vfunc->clos.size() > 0) {
      // instantiate function with closed vars
      VasyaFunction *fn = res->vfunc;
      VasyaValue *r2 = new VasyaValue(fn);
      unsigned int cl = fn->clos.size();
      for (unsigned int f = 0; f < cl; f++) {
        int depth = fn->clos[f].first;
        int lidx = fn->clos[f].second;
        RStackFrame *fr1;
        for (fr1 = fr; fr1 && depth > 0; depth--, fr1 = fr1->prev) ;
        if (depth > 0) die("VM internal error in function instantiation");
        VasyaValue *cl = fr1->locs[lidx];
        cl->incRef();
        r2->clos.push_back(cl);
      }
      res = r2;
    }
    return res;
  }
  if (ln >= LOC_CLOS) return fr->func->clos[ln-LOC_CLOS];
  if (ln >= FIRST_TEMP) return fr->locTemps[ln-FIRST_TEMP];
  // specials
  switch (ln) {
    case LOC_THIS: {
      VasyaValue *r = fr->vThis;
      return r ? r : mVoid; //FIXME
      }
    case LOC_CALLEE: return fr->func;
    case LOC_ARGS: { // 'arguments' object
      VasyaSimpleObject *so = new VasyaSimpleObject();
      int argc = (int)fr->args.size();
      //vlog("args: %d\n", argc);
      so->setField(this, "length", new VasyaValue(argc));
      for (int f = 0; f < argc; f++) so->setField(this, f, fr->args[f]);
      return new VasyaValue(so);
      }
    default: ;
  }
  return fr->locs[ln];
}


void VasyaVM::setLocal (int ln, VasyaValue *v) {
  VasyaValue *old = 0;
  if (ln < 0) die("VasyaVM: internal error 001");
  if (v) v->incRef();
  if (ln >= FIRST_TEMP) {
    old = mRStack->locTemps[ln-FIRST_TEMP];
    mRStack->locTemps[ln-FIRST_TEMP] = v;
  } else {
    old = mRStack->locs[ln];
    mRStack->locs[ln] = v;
  }
  if (old) old->decRef();
}


bool VasyaVM::str2num (const std::string &s, double *d) {
  const char *str = s.c_str();
  char *eptr;
  errno = 0;
  *d = strtod(str, &eptr);
  if (errno || eptr == str || (eptr-str) != (int)s.length()) {
    // can't convert, return NaN
    *d = NAN;
    return false;
  }
  return true;
}


VasyaValue *VasyaVM::bmath (const unsigned char op, const VasyaValue *op0, const VasyaValue *op1, VasyaValue *vNaN) {
  if (op == VOP_ADD && (op0->type == VasyaValue::string || op1->type == VasyaValue::string)) {
    char buf[256];
    std::string s;
    if (op0->type == VasyaValue::string) s = op0->sval;
    else {
      switch (op0->type) {
        case VasyaValue::number: sprintf(buf, "%.15g", op0->nval); break;
        case VasyaValue::vector: sprintf(buf, "[%.15g, %.15g]", op0->vval[0], op0->vval[1]); break;
        default: return 0;
      }
      s += buf;
    }
    if (op1->type == VasyaValue::string) s += op1->sval;
    else {
      switch (op1->type) {
        case VasyaValue::number: sprintf(buf, "%.15g", op1->nval); break;
        case VasyaValue::vector: sprintf(buf, "[%.15g, %.15g]", op1->vval[0], op1->vval[1]); break;
        default: return 0;
      }
      s += buf;
    }
    return new VasyaValue(s);
  }
  bool isVect0 = false, isVect1 = false;
  double d0 = 0.0, d1 = 0.0;
  switch (op0->type) {
    case VasyaValue::number: d0 = op0->nval; break;
    case VasyaValue::string:
      if (!str2num(op0->sval, &d0)) return vNaN;
      break;
    case VasyaValue::vector: isVect0 = true; break;
    default: return 0;
  }
  switch (op1->type) {
    case VasyaValue::number: d1 = op1->nval; break;
    case VasyaValue::string:
      if (!str2num(op1->sval, &d1)) return vNaN;
      break;
    case VasyaValue::vector:
      if (!isVect0) return 0;
      isVect1 = true;
      break;
    default: return 0;
  }
  if (isVect0) {
    if (isVect1) {
      switch (op) {
        case VOP_ADD:
          d0 = op0->vval[0]+op1->vval[0];
          d1 = op0->vval[1]+op1->vval[1];
          break;
        case VOP_SUB:
          d0 = op0->vval[0]-op1->vval[0];
          d1 = op0->vval[1]-op1->vval[1];
          break;
        case VOP_MUL:
          d0 = op0->vval[0]*op1->vval[0];
          d1 = op0->vval[1]*op1->vval[1];
          break;
        case VOP_DIV:
          feclearexcept(FE_ALL_EXCEPT);
          d0 = op1->vval[0]==0.0 ? NAN : op0->vval[0]/op1->vval[0];
          feclearexcept(FE_ALL_EXCEPT);
          d1 = op1->vval[1]==0.0 ? NAN : op0->vval[1]/op1->vval[1];
          break;
        case VOP_MOD:
          return 0;
        default: ;
      }
    } else {
      switch (op) {
        case VOP_ADD:
          d0 = op0->vval[0]+d1;
          d1 = op0->vval[1]+d1;
          break;
        case VOP_SUB:
          d0 = op0->vval[0]-d1;
          d1 = op0->vval[1]-d1;
          break;
        case VOP_MUL:
          d0 = op0->vval[0]*d1;
          d1 = op0->vval[1]*d1;
          break;
        case VOP_DIV:
          feclearexcept(FE_ALL_EXCEPT);
          d0 = d1==0.0 ? NAN : op0->vval[0]/d1;
          feclearexcept(FE_ALL_EXCEPT);
          d1 = d1==0.0 ? NAN : op0->vval[1]/d1;
          break;
        case VOP_MOD:
          return 0;
        default: ;
      }
    }
    return new VasyaValue(d0, d1);
  } else {
    switch (op) {
      case VOP_ADD: d0 = d0+d1; break;
      case VOP_SUB: d0 = d0-d1; break;
      case VOP_MUL: d0 = d0*d1; break;
      case VOP_DIV:
        feclearexcept(FE_ALL_EXCEPT);
        d0 = d1==0.0 ? NAN : d0/d1;
        break;
      case VOP_MOD:
        feclearexcept(FE_ALL_EXCEPT);
        d0 = d1==0.0 ? NAN : fmod(d0, d1);
        break;
      case VOP_BITAND: d0 = (double)(((long)d0) & ((long)d1)); break;
      case VOP_BITOR: d0 = (double)(((long)d0) | ((long)d1)); break;
      case VOP_BITXOR: d0 = (double)(((long)d0) ^ ((long)d1)); break;
      default: ;
    }
    if (isnan(d0)) return vNaN;
    return new VasyaValue(d0);
  }
}


bool VasyaVM::cmpmath (const unsigned char op, const VasyaValue *op0, const VasyaValue *op1) {
  int i1 = 0;
  double d0 = 0.0;
  switch (op0->type) {
    case VasyaValue::vvoid:
      if (op1->type == VasyaValue::vvoid) return op==VOP_EQ;
      return op==VOP_NE;
    case VasyaValue::string:
      switch (op1->type) {
        case VasyaValue::string:
          i1 = op0->sval.compare(op1->sval);
          break;
        case VasyaValue::number:
          if (str2num(op0->sval, &d0)) {
            if (d0 < op1->nval) i1 = -1;
            else if (d0 > op1->nval) i1 = 1;
            else i1 = 0;
            break;
          }
          // fallthru
        default:
          return op==VOP_NE;
      }
      break;
    case VasyaValue::number:
      switch (op1->type) {
        case VasyaValue::string:
          if (str2num(op1->sval, &d0)) {
            if (op0->nval < d0) i1 = -1;
            else if (op0->nval > d0) i1 = 1;
            else i1 = 0;
            break;
          }
          return op==VOP_NE;
        case VasyaValue::number:
          if (op0->nval < op1->nval) i1 = -1;
          else if (op0->nval > op1->nval) i1 = 1;
          else i1 = 0;
          break;
        default:
          return op==VOP_NE;
      }
      break;
    case VasyaValue::vector:
      if (op1->type != VasyaValue::vector) return op==VOP_NE;
      //FIXME!
      if (op0->vval[0] == op1->vval[0] && op0->vval[1] == op1->vval[1]) return op==VOP_EQ;
      return op==VOP_NE;
    case VasyaValue::function:
      if (op1->type != VasyaValue::function) return op==VOP_NE;
      if (op0->vfunc == op1->vfunc) return op==VOP_EQ;
      return op==VOP_NE;
    case VasyaValue::object:
      if (op1->type != VasyaValue::object) return op==VOP_NE;
      if (op0->vobj == op1->vobj) return op==VOP_EQ;
      return op==VOP_NE;
    default:
      if (op0 == op1) return op==VOP_EQ;
      return op==VOP_NE;
  }
  switch (op) {
    case VOP_EQ: return i1==0; break;
    case VOP_NE: return i1!=0; break;
    case VOP_LE: return i1<=0; break;
    case VOP_GE: return i1>=0; break;
    case VOP_LT: return i1<0; break;
    case VOP_GT: return i1>0; break;
    default: ;
  }
  return false;
}


VasyaValue *VasyaVM::run () {
  VasyaValue *r, *va, *vb, *vc;
  RStackFrame *fr = mRStack;
  double d;
  long i0;
  int rr, opa;
  if (!fr) die("empty execution sequence");
  for (;;) {
    if (--mBreakCheckCount <= 0) {
      mBreakCheckCount = BCC;
      if (shouldBreak()) throw VasyaVMBreak();
    }
    //VasyaCC::dumpBC(fr->func->vfunc, fr->pc, 1);
    mLastPC = fr->pc;
    const VasyaVMOp *op = fr->func->vfunc->bcode[fr->pc++];
    mLastLine = op->line;
    switch (op->op) {
      case VOP_RETURN:
        opa = op->a;
        rr = fr->resDestLoc;
        r = dropR();
        if (opa < 0 || !r) {
          if (r) r->decRef();
          r = mVoid;
          r->incRef();
        }
        if (!mRStack) return r;
        fr = mRStack;
        if (!fr->func) {
          mRStack = fr->prev;
          delete fr;
          return r;
        }
        setLocal(rr, r);
        r->decRef();
        break;
      // a: loc-res; b,c: loc-arg
      case VOP_ADD:
      case VOP_SUB:
      case VOP_MUL:
      case VOP_DIV:
      case VOP_MOD:
      case VOP_BITAND:
      case VOP_BITOR:
      case VOP_BITXOR:
        va = getLocal(op->b);
        vb = getLocal(op->c);
        va = bmath(op->op, va, vb, mNaN);
        if (!va) die("invalid math");
        setLocal(op->a, va);
        break;
      // a: loc-res; b,c: loc-arg
      case VOP_EQ:
      case VOP_NE:
      case VOP_LE:
      case VOP_GE:
      case VOP_LT:
      case VOP_GT:
        va = getLocal(op->b);
        vb = getLocal(op->c);
        setLocal(op->a, cmpmath(op->op, va, vb) ? mOne : mZero);
        break;
      // a: loc-res; b: loc-var
      case VOP_NOT:
        va = getLocal(op->b);
        setLocal(op->a, va->isTrue() ? mZero : mOne);
        break;
      case VOP_BITNOT:
        va = getLocal(op->b);
        switch (va->type) {
          case VasyaValue::number: i0 = (long)va->nval; break;
          case VasyaValue::string:
            if (VasyaVM::str2num(va->sval, &d) && isfinite(d)) {
              i0 = (long)d;
              break;
            }
          default:
            die("invalid operand for '~'");
            i0 = 0;
        }
        setLocal(op->a, new VasyaValue((double)(~i0)));
        break;
      // a: loc-res; b: loc-arg
      case VOP_STORE:
        r = getLocal(op->b);
        setLocal(op->a, r);
        break;
      // a: loc-res; b: string literal (name)
      case VOP_LOADGLOB:
        va = getLocal(op->b);
        vb = getGlobal(va->sval);
        if (!vb) die("undefined global: '%s'", va->sval.c_str());
        setLocal(op->a, vb);
        break;
      // a: string literal (name); b: loc-arg
      case VOP_STOREGLOB:
        va = getLocal(op->a);
        vb = getLocal(op->b);
        setGlobal(vb->sval, va);
        break;
      // a: loc-res; b: object; c: loc-var/lit (fld)
      case VOP_FLOAD: // load field value (value can be object too)
        va = getLocal(op->b);
        vb = getLocal(op->c);
        switch (va->type) {
          case VasyaValue::object:
            switch (vb->type) {
              case VasyaValue::string:
                vb = va->vobj->getField(this, vb->sval);
                break;
              case VasyaValue::number:
                vb = va->vobj->getField(this, (long)vb->nval);
                break;
              default: die("field name is not string");
            }
            setLocal(op->a, vb ? vb : mVoid);
            break;
          case VasyaValue::string:
            switch (vb->type) {
              case VasyaValue::string:
                if (vb->sval == "length") setLocal(op->a, new VasyaValue((double)va->sval.length()));
                else die("invalid string field");
                break;
              case VasyaValue::number:
                i0 = (long)vb->nval;
                if (i0 < 0) i0 += (int)va->sval.length();
                if (i0 < 0 || i0 >= (int)va->sval.length()) setLocal(op->a, new VasyaValue(""));
                else {
                  std::string s;
                  s += (char)va->sval[i0];
                  setLocal(op->a, new VasyaValue(s));
                }
                break;
              default: die("field name is not string");
            }
            break;
          case VasyaValue::vector:
            switch (vb->type) {
              case VasyaValue::string:
                if (vb->sval == "x") setLocal(op->a, new VasyaValue(va->vval[0]));
                else if (vb->sval == "y") setLocal(op->a, new VasyaValue(va->vval[1]));
                else if (vb->sval == "length") {
                  die("TODO: add vector.length and other fields");
                  //setLocal(op->a, new VasyaValue((double)va->sval.length()));
                } else die("invalid string field");
                break;
              case VasyaValue::number:
                i0 = (long)vb->nval;
                if (i0 < 0 || i0 > 1) die("invalid vector indexing");
                setLocal(op->a, new VasyaValue(va->vval[i0]));
                break;
              default: die("field name is not string");
            }
            break;
          default:
            die("indexing non-object");
        }
        break;
      // a: loc-val (will NOT be changed); b: object; c: loc-var/lit (fld)
      case VOP_FSTORE: // store field value; a can be literal
        va = getLocal(op->a);
        vb = getLocal(op->b);
        vc = getLocal(op->c);
        switch (vb->type) {
          case VasyaValue::string:
            switch (vc->type) {
              case VasyaValue::string:
                /*if (vc->sval == "length") setLocal(op->a, new VasyaValue((double)vb->sval.length()));
                else*/ die("invalid string field");
                break;
              case VasyaValue::number:
                if (va->type != VasyaValue::string) die("string indexing: string value expected"); //FIXME: msg
                i0 = (long)vc->nval;
                if (i0 < 0) i0 += (int)vb->sval.length();
                if (i0 >= 0 && i0 < (int)vb->sval.length()) {
                  vb->sval.erase(i0, 1);
                  vb->sval.insert(i0, va->sval);
                } else if (i0 >= (int)vb->sval.length()) {
                  vb->sval += va->sval;
                }
                break;
              default: die("field name is not string");
            }
            break;
          case VasyaValue::object:
            switch (vc->type) {
              case VasyaValue::string:
                vb->vobj->setField(this, vc->sval, va);
                break;
              case VasyaValue::number:
                vb->vobj->setField(this, (long)vc->nval, va);
                break;
              default: die("field name is not string");
            }
            break;
          default:
            die("indexing non-object");
        }
        break;
      // b: addr
      case VOP_JUMP:
        fr->pc = op->b;
        break;
      // a: temp; b: addr
      case VOP_JMPT:
        if (getLocal(op->a)->isTrue()) fr->pc = op->b;
        break;
      case VOP_JMPF:
        if (!getLocal(op->a)->isTrue()) fr->pc = op->b;
        break;
      // a: loc-argnf; b: argc; c: this/0
      // loc-argnf: func, args; result: in a
      case VOP_CALL:
        va = getLocal(op->a);
        //vlog("va->type: %d\n", (int)va->type);
        if (va->type != VasyaValue::function) die("VasyaVM: calling non-function");
        if (va->vfunc->isNative()) {
          vb = va->vfunc->exec(this, op->a+1, op->b, op->c?getLocal(op->c):mWorldVV);
          setLocal(op->a, vb ? vb : mVoid);
        } else {
          addRFunc(va, op->a, op->c?getLocal(op->c):mWorldVV);
          fr = mRStack;
          // wow, so slow!
          for (int f = 0, c = op->a+1; f < op->b; f++, c++) {
            vb = getLocal(c, 1);
            if (f < va->vfunc->argc) setLocal(f+1, vb);
            vb->incRef();
            fr->args.push_back(vb);
          }
          for (int f = op->b; f < va->vfunc->argc; f++) setLocal(f+1, mVoid);
        }
        break;
      case VOP_NEWOBJ:
        va = new VasyaValue(new VasyaPInhObject());
        setLocal(op->a, va);
        break;
      case VOP_TYPEOF:
        vb = getLocal(op->b);
        switch (vb->type) {
          case VasyaValue::vvoid: va = new VasyaValue("void"); break;
          case VasyaValue::number: va = new VasyaValue("number"); break;
          case VasyaValue::string: va = new VasyaValue("string"); break;
          case VasyaValue::vector: va = new VasyaValue("vector"); break;
          case VasyaValue::function: va = new VasyaValue("function"); break;
          case VasyaValue::object: va = new VasyaValue("object"); break;
          default:
            die("internal error in 'typeof'");
            va = 0;
        }
        setLocal(op->a, va);
        break;
#ifdef VASYA_GEN_TEMPCLR
      case VOP_TEMPCLR:
        if (op->a >= FIRST_TEMP) setLocal(op->a, 0);
        if (op->b >= FIRST_TEMP) setLocal(op->b, 0);
        if (op->c >= FIRST_TEMP) setLocal(op->c, 0);
        break;
#endif
      default: die("VasyaVM: internal error 002");
    }
  }
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
VasyaValue *VasyaFunctionCall::exec (VasyaVM *vm, int argv, int argc, VasyaValue *self) {
  if (argc < 1) vm->die("out of args for call()");
  VasyaValue *vfn = vm->getLocal(argv++);
  if (vfn->type != VasyaValue::function) vm->die("can't apply non-function");
  VasyaValue *aThis = self;
  argc--;
  if (argc > 0) {
    aThis = vm->getLocal(argv++);
    argc--;
    if (aThis->type != VasyaValue::object) vm->die("'this' is not an object");
  }
  VasyaValue **args = new VasyaValue *[argc+1];
  try {
    for (int f = 0; f < argc; f++) args[f] = vm->getLocal(argv);
    VasyaValue *res = vm->apply(vfn->vfunc, aThis, argc, args);
    delete [] args;
    return res;
  } catch (...) {
    delete [] args;
    throw;
  }
}


VasyaValue *VasyaFunctionApply::exec (VasyaVM *vm, int argv, int argc, VasyaValue *self) {
  if (argc != 4) vm->die("invalid number of args for apply()");
  VasyaValue *vfn = vm->getLocal(argv++);
  if (vfn->type != VasyaValue::function) vm->die("can't apply non-function");
  VasyaValue *aThis = self;
  argc--;
  aThis = vm->getLocal(argv++);
  argc--;
  if (aThis->type != VasyaValue::object) vm->die("'this' is not an object");
  VasyaValue *acnt = vm->getLocal(argv++);
  switch (acnt->type) {
    case VasyaValue::number:
      argc = (int)acnt->nval;
      break;
    case VasyaValue::string: {
      double d;
      if (VasyaVM::str2num(acnt->sval, &d)) { argc = (int)d; break; }
      }
    default:
      vm->die("argument count is not a number");
  }
  if (argc < 0) argc = 0;
  if (argc > 127) vm->die("too many arguments to function");
  VasyaValue *alist = vm->getLocal(argv++);
  if (alist->type != VasyaValue::object) vm->die("argument array is not an object");
  VasyaValue **args = new VasyaValue *[argc+1];
  try {
    for (int f = 0; f < argc; f++) args[f] = alist->vobj->getField(vm, f);
    VasyaValue *res = vm->apply(vfn->vfunc, aThis, argc, args);
    delete [] args;
    return res;
  } catch (...) {
    delete [] args;
    throw;
  }
}
