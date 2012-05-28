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
#ifndef __FELIB_SAVE_H__
#define __FELIB_SAVE_H__

#include <ctime>
#include <cstdio>
#include <vector>
#include <stack>
#include <deque>
#include <list>
#include <map>
#include <set>

#ifdef USE_ZLIB
# include <zlib.h>
#endif


#include "error.h"
#include "festring.h"
#include "fearray.h"


#define RAW_SAVE_LOAD(type) \
inline outputfile& operator<<(outputfile& SaveFile, type Value) { \
  SaveFile.Write(reinterpret_cast<char*>(&Value), sizeof(Value)); \
  return SaveFile; \
} \
 \
inline inputfile& operator>>(inputfile& SaveFile, type& Value) { \
  SaveFile.Read(reinterpret_cast<char*>(&Value), sizeof(Value)); \
  return SaveFile; \
}


class inputfile;
class outputfile;


typedef std::map<festring, sLong> valuemap;


class outputfile {
public:
  outputfile (cfestring &FileName, truth maxcomp, truth AbortOnErr=true);
  ~outputfile ();

  void Put (char What);
  void Write (cchar *Offset, sLong Size);
  truth IsOpen () { return (Buffer != 0); }
  void Close ();
  void Flush ();

private:
#ifdef USE_ZLIB
  gzFile Buffer;
#else
  FILE *Buffer;
#endif
  festring FileName;
};


typedef festring (*InputFileGetVarFn) (inputfile *fl, cfestring &name);

class inputfile {
public:
  inputfile (cfestring &FileName, const valuemap *ValueMap=0, truth AbortOnErr=true);
  virtual ~inputfile ();

  void SkipSpaces ();

  festring ReadCode (truth AbortOnEOF=true);
  festring ReadWord (truth AbortOnEOF=true);
  void ReadWord (festring &str, truth AbortOnEOF=true, truth skipIt=false);
  char ReadLetter (truth AbortOnEOF=true);
  sLong ReadNumber (int CallLevel=HIGHEST, truth PreserveTerminator=false, truth *wasCloseBrc=0);
  festring ReadStringOrNumber (sLong *num, truth *isString, truth PreserveTerminator=false, truth *wasCloseBrc=0);
  v2 ReadVector2d ();
  rect ReadRect ();
  int Get ();
  void Unget (int ch);
  void Read (char *Offset, sLong Size);
  truth IsOpen () { return (Buffer != 0); }
  truth Eof ();
  void ClearFlags ();
  void SeekPosBegin (sLong Offset);
  void SeekPosCurrent (sLong Offset);
  void SeekPosEnd (sLong Offset);
  sLong TellPos ();
  //feuLong TellLine () { return TellLineOfPos(TellPos()); }
  //feuLong TellLineOfPos (sLong);
  cfestring &GetFileName () const { return FileName; }
  void Close ();

  static truth fileExists (const festring &fname);
  static festring GetMyDir (void);

  festring getVar (cfestring &name);
  void setVar (cfestring &name, cfestring &value);
  truth delVar (cfestring &name);

  void setGetVarCB (InputFileGetVarFn cb) { mGetVar = cb; }

  void die (cfestring &msg);

  inline int TokenLine () const { return mTokenLine; }

protected:
  festring ReadNumberIntr (int CallLevel, sLong *num, truth *isString, truth allowStr, truth PreserveTerminator, truth *wasCloseBrc);
  int HandlePunct (festring &String, int Char, int Mode);

  festring findVar (cfestring &name, truth *found) const;
  void readWordIntr (festring &String, truth AbortOnEOF);

  festring readCondition (festring &token, int prio, truth skipIt);

protected:
  typedef std::map<festring, festring> VarMap;
#ifdef USE_ZLIB
  gzFile Buffer;
#else
  FILE *Buffer;
#endif
  festring FileName;
  const valuemap *ValueMap;
  truth lastWordWasString;
  VarMap mVars;
  InputFileGetVarFn mGetVar;
  std::stack<int> mIfStack;
#ifdef USE_ZLIB
  int mFileSize;
#endif
  int mCharBuf[4];
  int mCharBufPos;
  int mCurrentLine;
  int mTokenLine;
};


class meminputfile : public inputfile {
public:
  meminputfile (cfestring &str, const valuemap *ValueMap=0);
  virtual ~meminputfile ();

protected:
  void *buf;
  int bufSize;
  festring tfname;
};


/* Reads a binary form variable of type type and returns it.
 * An inputfile template member function would be far more elegant,
 * but VC doesn't seem to understand it. */
template <class type> inline type ReadType (inputfile &SaveFile) {
  type Variable;
  SaveFile >> Variable;
  return Variable;
}

inline void ReadData (char &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); }
inline void ReadData (uChar &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); }
inline void ReadData (short &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); }
inline void ReadData (uShort &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); }
//inline void ReadData (sLong &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); } //k8:64
inline void ReadData (feuLong &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); }
inline void ReadData (int &Type, inputfile &SaveFile) { Type = SaveFile.ReadNumber(); }
inline void ReadData (packv2 &Type, inputfile &SaveFile) { Type = SaveFile.ReadVector2d(); }
inline void ReadData (v2 &Type, inputfile &SaveFile) { Type = SaveFile.ReadVector2d(); }
inline void ReadData (rect &Type, inputfile &SaveFile) { Type = SaveFile.ReadRect(); }

void ReadData (festring &, inputfile &);
void ReadData (fearray<sLong> &, inputfile &);
void ReadData (fearray<festring> &, inputfile &);

template <class type> inline void ReadData (fearray<type> &Array, inputfile &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  //if (Word == "=") SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    ReadData(Array.Data[0], SaveFile);
    return;
  }
  if (Word != "=") ABORT("Array syntax error: '=' or '==' expected in file %s, line %u!", SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  SaveFile.ReadWord(Word);
  if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %u!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  typedef typename fearray<type>::sizetype sizetype;
  sizetype Size = SaveFile.ReadNumber();
  Array.Allocate(Size);
  for (sizetype c = 0; c < Size; ++c) ReadData(Array.Data[c], SaveFile);
  if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %u!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
}


inline outputfile &operator << (outputfile &SaveFile, char Value) {
  SaveFile.Put(Value);
  return SaveFile;
}


inline inputfile &operator >> (inputfile &SaveFile, char &Value) {
  Value = SaveFile.Get();
  return SaveFile;
}


inline outputfile &operator << (outputfile &SaveFile, uChar Value) {
  SaveFile.Put(Value);
  return SaveFile;
}


inline inputfile &operator >> (inputfile &SaveFile, uChar &Value) {
  Value = SaveFile.Get();
  return SaveFile;
}


inline outputfile &operator << (outputfile &SaveFile, short Value) {
  SaveFile.Put(Value);
  SaveFile.Put(Value >> 8);
  return SaveFile;
}


inline inputfile &operator >> (inputfile &SaveFile, short &Value) {
  int LowWord = SaveFile.Get();
  Value = SaveFile.Get() << 8 | LowWord;
  return SaveFile;
}


inline outputfile &operator << (outputfile &SaveFile, uShort Value) {
  SaveFile.Put(Value);
  SaveFile.Put(Value >> 8);
  return SaveFile;
}


inline inputfile &operator >> (inputfile &SaveFile, uShort &Value) {
  int LowWord = SaveFile.Get();
  Value = SaveFile.Get() << 8 | LowWord;
  return SaveFile;
}


//RAW_SAVE_LOAD(sLong); //k8:64
//RAW_SAVE_LOAD(feuLong); //k8:64
RAW_SAVE_LOAD(int);
RAW_SAVE_LOAD(uInt);
RAW_SAVE_LOAD(double);
RAW_SAVE_LOAD(packv2);
RAW_SAVE_LOAD(v2);
RAW_SAVE_LOAD(rect);
RAW_SAVE_LOAD(time_t);


outputfile &operator << (outputfile &, cfestring &);
inputfile &operator >> (inputfile &, festring &);
outputfile &operator << (outputfile &, cchar *);
inputfile &operator >> (inputfile &, char *&);


template <class type1, class type2> inline outputfile &operator << (outputfile &SaveFile, const std::pair<type1, type2> &Pair) {
  SaveFile << Pair.first << Pair.second;
  return SaveFile;
}

template <class type1, class type2> inline inputfile &operator >> (inputfile &SaveFile, std::pair<type1, type2> &Pair) {
  SaveFile >> Pair.first >> Pair.second;
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const std::vector<type> &Vector) {
  SaveFile << feuLong(Vector.size());
  for (feuLong c = 0; c < Vector.size(); ++c) SaveFile << Vector[c];
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, std::vector<type> &Vector) {
  Vector.resize(ReadType<feuLong>(SaveFile), type());
  for (feuLong c = 0; c < Vector.size(); ++c) SaveFile >> Vector[c];
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const std::deque<type> &Deque) {
  SaveFile << feuLong(Deque.size());
  for (feuLong c = 0; c < Deque.size(); ++c) SaveFile << Deque[c];
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, std::deque<type> &Deque) {
  Deque.resize(ReadType<feuLong>(SaveFile), type());
  for (feuLong c = 0; c < Deque.size(); ++c) SaveFile >> Deque[c];
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const std::list<type> &List) {
  SaveFile << feuLong(List.size());
  for (typename std::list<type>::const_iterator i = List.begin(); i != List.end(); ++i) SaveFile << *i;
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, std::list<type> &List) {
  List.resize(ReadType<feuLong>(SaveFile), type());
  for (typename std::list<type>::iterator i = List.begin(); i != List.end(); ++i) SaveFile >> *i;
  return SaveFile;
}

template <class type1, class type2> inline outputfile &operator << (outputfile &SaveFile, const std::map<type1, type2> &Map) {
  SaveFile << feuLong(Map.size());
  for (typename std::map<type1, type2>::const_iterator i = Map.begin(); i != Map.end(); ++i) SaveFile << i->first << i->second;
  return SaveFile;
}

template <class type1, class type2> inline inputfile &operator >> (inputfile &SaveFile, std::map<type1, type2> &Map) {
  Map.clear();
  type1 First;
  feuLong Size;
  SaveFile >> Size;
  typename std::map<type1, type2>::iterator i;
  for (feuLong c = 0; c < Size; ++c) {
    SaveFile >> First;
    i = Map.insert(Map.end(), std::make_pair(First, type2()));
    SaveFile >> i->second;
  }
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const std::set<type> &Set) {
  SaveFile << feuLong(Set.size());
  for (typename std::set<type>::const_iterator i = Set.begin(); i != Set.end(); ++i) SaveFile << *i;
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, std::set<type> &Set) {
  Set.clear();
  feuLong Size;
  SaveFile >> Size;
  for (feuLong c = 0; c < Size; ++c) {
    type Value;
    SaveFile >> Value;
    Set.insert(Value);
  }
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const fearray<type> &Array) {
  typename fearray<type>::sizetype c, Size = Array.Size;
  SaveFile << Size;
  for (c = 0; c < Size; ++c) SaveFile << Array[c];
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, fearray<type> &Array) {
  typename fearray<type>::sizetype c, Size;
  SaveFile >> Size;
  Array.Allocate(Size);
  for (c = 0; c < Size; ++c) SaveFile >> Array[c];
  return SaveFile;
}

template <class type> inline outputfile &SaveLinkedList (outputfile &SaveFile, const type *Element) {
  for (const type* E = Element; E; E = E->Next) {
    SaveFile.Put(true);
    SaveFile << E;
  }
  SaveFile.Put(false);
  return SaveFile;
}

template <class type> inline inputfile &LoadLinkedList(inputfile &SaveFile, type *&Element) {
  if (SaveFile.Get()) {
    SaveFile >> Element;
    type *E;
    for (E = Element; SaveFile.Get(); E = E->Next) SaveFile >> E->Next;
    E->Next = 0;
  } else {
    Element = 0;
  }
  return SaveFile;
}

template <class type> inline outputfile &SaveArray (outputfile &SaveFile, const type *Array, int Count) {
  for (int c = 0; c < Count; ++c) SaveFile << Array[c];
  return SaveFile;
}

template <class type> inline inputfile &LoadArray (inputfile &SaveFile, type *Array, int Count) {
  for (int c = 0; c < Count; ++c) SaveFile >> Array[c];
  return SaveFile;
}


#endif
