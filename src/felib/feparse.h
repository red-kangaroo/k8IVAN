/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */
#ifndef __FELIB_PARSE_H__
#define __FELIB_PARSE_H__

#include "feerror.h"
#include "festring.h"
#include "fearray.h"
#include "fesave.h"
#include "femath.h"


// ////////////////////////////////////////////////////////////////////////// //
class TextInput;
class TextInputFile;
class MemTextFile;
struct InputFileSaved;


using valuemap = std::map<festring, sLong>;
using InputFileGetVarFn = festring (*) (TextInput *fl, cfestring &name);


// ////////////////////////////////////////////////////////////////////////// //
class TextInput {
protected:
  TextInput (const valuemap *aValueMap=0);

  void setup (const valuemap *aValueMap);

public:
  virtual ~TextInput ();

  virtual cfestring &GetFileName () const = 0;
  virtual truth IsOpen () = 0;
  truth Eof ();
  virtual void Close (); // for dtor

  int GetChar ();
  void UngetChar (int ch);

  festring ReadWord (truth abortOnEOF=true);
  truth ReadWord (festring &str, truth abortOnEOF=true); // returns `false` on EOF
  char ReadLetter (truth abortOnEOF=true);
  sLong ReadNumber (int CallLevel=HIGHEST, truth PreserveTerminator=false, truth *wasCloseBrc=0);
  festring ReadStringOrNumber (sLong *num, truth *isString, truth PreserveTerminator=false, truth *wasCloseBrc=0);
  sLong ReadNumberKeepStr (int CallLevel=HIGHEST, truth PreserveTerminator=false, truth *wasCloseBrc=0);
  festring ReadStringOrNumberKeepStr (sLong *num, truth *isString, truth PreserveTerminator=false, truth *wasCloseBrc=0);
  v2 ReadVector2d ();
  rect ReadRect ();
  float ReadFloat ();

  festring getVar (cfestring &name);
  void setVar (cfestring &name, cfestring &value);
  truth delVar (cfestring &name);

  void setGetVarCB (InputFileGetVarFn cb) { mGetVar = cb; }

  void die (cfestring &msg);

  void skipBlanks (); // comments too

  int countArrayItems (char echar);

  inline int TokenLine () const { return mTokenLine; }
  inline int CurrentLine () const { return mCurrentLine; }

  // when reading a number, and `mCollectingNumStr` flag is set, this will be filled
  inline cfestring& numStr () const { return mNumStr; }

  int gotCharSkipComment (int ch, truth allowSingleLineComments=true); // just read `ch`, skip possible comment; returns `ch` or -1

protected:
  template<typename numtype> festring ReadNumberIntr (int CallLevel, numtype *num, truth *isString, truth allowStr, truth PreserveTerminator, truth *wasCloseBrc, truth allowFloats);

  festring findVar (cfestring &name, truth *found) const;
  truth readWordIntr (festring &String, truth abortOnEOF); // returns `false` on EOF

  festring readCondition (festring &token, int prio, truth skipIt);

protected:
  virtual int realGetChar () = 0;
  virtual truth isRealEof () = 0;

  // used to save/restore parsing positions
  virtual sLong realGetPos () = 0;
  virtual void realSetPos (sLong apos) = 0;

protected:
  enum { MaxUngetChars = 4 };
  typedef std::map<festring, festring> VarMap;

protected:
  const valuemap *ValueMap;
  truth lastWasNL;
  truth lastWordWasString;
  VarMap mVars;
  InputFileGetVarFn mGetVar;
  std::stack<int> mIfStack;
  int mCharBuf[MaxUngetChars];
  int mCharBufPos;
  int mCurrentLine;
  int mTokenLine;
  festring mNumStr; // when reading a number, and `mCollectingNumStr` flag is set, this will be filled
  truth mCollectingNumStr;
  truth mAllowFloatNums;

  friend InputFileSaved;
};


// ////////////////////////////////////////////////////////////////////////// //
class TextInputFile : public TextInput {
public:
  TextInputFile (cfestring &FileName, const valuemap *aValueMap=0, truth AbortOnErr=true);
  virtual ~TextInputFile ();

  virtual cfestring &GetFileName () const override;
  virtual truth IsOpen () override;
  virtual void Close () override;

protected:
  virtual int realGetChar () override;
  virtual truth isRealEof () override;

  virtual sLong realGetPos () override;
  virtual void realSetPos (sLong apos) override;

protected:
  inputfile ifile;
};


// ////////////////////////////////////////////////////////////////////////// //
class MemTextFile : public TextInput {
public:
  MemTextFile (cfestring &afname, cfestring &str, const valuemap *aValueMap=0);
  MemTextFile (cfestring &afname, int stline, cfestring &str, const valuemap *aValueMap=0);
  virtual ~MemTextFile ();

  virtual cfestring &GetFileName () const override;
  virtual truth IsOpen () override;
  virtual void Close () override;

protected:
  virtual int realGetChar () override;
  virtual truth isRealEof () override;

  virtual sLong realGetPos () override;
  virtual void realSetPos (sLong apos) override;

protected:
  unsigned char *buf;
  int bufSize;
  int bufPos;
  festring tfname;
};


// ////////////////////////////////////////////////////////////////////////// //
// `rand()` returns `add+RAND_N(rnd)`
// `inRange()` returns `true` if `rand()` is in [rmin..rmax] (inclusive!)
// script syntax:
//   RCField = { add: num; rnd: num; rmin: num; rmax: num; }
//   any field may absent
//   or simplified:
//   RCField == rnd;
struct RandomChance {
  sLong add = 0;
  sLong rnd = 0;
  sLong rmin = 0, rmax = 0;

  inline void clear () { add = rnd = rmin = rmax = 0; }
  inline sLong rand () const { return add+RAND_N(rnd); }
  // if `rnd` is zero, assume "no chances"
  inline truth inRange () const { if (rnd > 0) { auto v = rand(); return (v >= rmin && v <= rmax); } return false; }
};


inline void ReadData (char &Type, TextInput &infile) { Type = infile.ReadNumber(); }
inline void ReadData (uChar &Type, TextInput &infile) { Type = infile.ReadNumber(); }
inline void ReadData (short &Type, TextInput &infile) { Type = infile.ReadNumber(); }
inline void ReadData (uShort &Type, TextInput &infile) { Type = infile.ReadNumber(); }
//inline void ReadData (sLong &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); } //k8:64
inline void ReadData (feuLong &Type, TextInput &infile) { Type = infile.ReadNumber(); }
inline void ReadData (int &Type, TextInput &infile) { Type = infile.ReadNumber(); }
inline void ReadData (packv2 &Type, TextInput &infile) { Type = infile.ReadVector2d(); }
inline void ReadData (v2 &Type, TextInput &infile) { Type = infile.ReadVector2d(); }
inline void ReadData (rect &Type, TextInput &infile) { Type = infile.ReadRect(); }
inline void ReadData (float &Type, TextInput &infile) { Type = infile.ReadFloat(); }

void ReadData (festring &, TextInput &);
void ReadData (fearray<sLong> &, TextInput &);
void ReadData (fearray<festring> &, TextInput &);
void ReadData (RandomChance &rc, TextInput &fl);

template <class type> void ReadData (fearray<type> &Array, TextInput &infile) {
  Array.Clear();
  festring Word;
  infile.ReadWord(Word);
  // one element?
  if (Word == "==") {
    Array.Allocate(1);
    ReadData(Array.Data[0], infile);
    return;
  }
  // more than one element
  typedef typename fearray<type>::sizetype sizetype;
  sizetype Size;
  // unknown number of elements?
  if (Word == ":=") {
    infile.ReadWord(Word);
    if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), infile.GetFileName().CStr(), infile.TokenLine());
    //fprintf(stderr, "counting array items; file: '%s'; line: %u\n", SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    int count = infile.countArrayItems('}');
    // HACK
    if (count < 1) ABORT("Array count error found in file %s, line %d!", infile.GetFileName().CStr(), infile.TokenLine());
    Size = count;
    //fprintf(stderr, "counted %u array items; file: '%s'; line: %u\n", Size, SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  } else {
    if (Word != "=") ABORT("Array syntax error: '=', '==' or ':=' expected in file %s, line %u!", infile.GetFileName().CStr(), infile.TokenLine());
    infile.ReadWord(Word);
    if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %u!", Word.CStr(), infile.GetFileName().CStr(), infile.TokenLine());
    Size = infile.ReadNumber();
    int count = infile.countArrayItems('}');
    if (count < 1) ABORT("Array count error found in file %s, line %d!", infile.GetFileName().CStr(), infile.TokenLine());
    //HACK: 2 for vectors, 4 for rects
    //if ((sizetype)count != Size && (sizetype)count/2 != Size && (sizetype)count/4 != Size) ABORT("Array count error (%u != %d) found in file %s, line %d!", Size, count, SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    if ((sizetype)count != Size) ABORT("Array count error (%u != %d) found in file %s, line %d!", Size, count, infile.GetFileName().CStr(), infile.TokenLine());
  }
  Array.Allocate(Size);
  for (sizetype c = 0; c < Size; ++c) ReadData(Array.Data[c], infile);
  if (infile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %u!", Word.CStr(), infile.GetFileName().CStr(), infile.TokenLine());
}


// ////////////////////////////////////////////////////////////////////////// //
struct EventHandlerSource {
  festring type;
  int startline;
  festring text;
  festring fname;
};


struct EventHandlerMap {
private:
  //EventHandlerMap (const EventHandlerMap &); // disable copying

public:
  EventHandlerMap ();
  virtual ~EventHandlerMap ();

  void clear ();

  // "on" skipped, expecting type
  void collectSource (std::stack<TextInput *> &infStack, TextInput **iff);

  // "on" skipped, expecting type
  void collectSource (TextInput &iff);

  // can return `nullptr`
  TextInput *openHandler (cfestring &atype, const valuemap *aValueMap=0) const;

private:
  std::map<festring, EventHandlerSource> mMap;
};


festring collectSourceCode (std::stack<TextInput *> &infStack, TextInput **iff);
EventHandlerSource collectEventHandlerSource (std::stack<TextInput *> &infStack, TextInput **iff);


#endif
