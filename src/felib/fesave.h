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


#include "feerror.h"
#include "festring.h"
#include "fearray.h"


// ////////////////////////////////////////////////////////////////////////// //
#define RAW_SAVE_LOAD(type) \
inline outputfile &operator << (outputfile &SaveFile, type Value) { \
  SaveFile.Write(reinterpret_cast<char *>(&Value), sizeof(Value)); \
  return SaveFile; \
} \
 \
inline inputfile &operator >> (inputfile &SaveFile, type &Value) { \
  SaveFile.Read(reinterpret_cast<char *>(&Value), sizeof(Value)); \
  return SaveFile; \
}


// ////////////////////////////////////////////////////////////////////////// //
class inputfile;
class outputfile;


typedef std::map<festring, sLong> valuemap;


// ////////////////////////////////////////////////////////////////////////// //
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


// ////////////////////////////////////////////////////////////////////////// //
class inputfile {
public:
  inputfile ();
  inputfile (cfestring &aFileName, truth AbortOnErr=true);
  virtual ~inputfile ();

  bool Open (cfestring &aFileName, truth AbortOnErr=true);

  int Get (); // returns EOF on eof
  void Read (char *Offset, sLong Size);
  truth IsOpen () { return (Buffer != 0); }
  truth Eof ();
  void ClearFlags ();
  void SeekPosBegin (sLong Offset);
  void SeekPosCurrent (sLong Offset);
  void SeekPosEnd (sLong Offset);
  sLong TellPos ();
  cfestring &GetFileName () const { return FileName; }
  void Close ();

  static truth fileExists (const festring &fname);
  static festring GetMyDir (void);

protected:
#ifdef USE_ZLIB
  gzFile Buffer;
#else
  FILE *Buffer;
#endif
  festring FileName;
#ifdef USE_ZLIB
  int mFileSize;
#endif
};


// ////////////////////////////////////////////////////////////////////////// //
/* Reads a binary form variable of type type and returns it.
 * An inputfile template member function would be far more elegant,
 * but VC doesn't seem to understand it. */
template <class type> inline type _ReadType_ (inputfile &SaveFile, const char *tag) {
  type Variable;
  //
  //fprintf(stderr, "ReadType: [%s]\n", tag);
  SaveFile >> Variable;
  return Variable;
}

#define ReadType(typename,fl)  _ReadType_<typename>(fl, #typename)


// ////////////////////////////////////////////////////////////////////////// //
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
  Vector.resize(ReadType(feuLong, SaveFile), type());
  for (feuLong c = 0; c < Vector.size(); ++c) SaveFile >> Vector[c];
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const std::deque<type> &Deque) {
  SaveFile << feuLong(Deque.size());
  for (feuLong c = 0; c < Deque.size(); ++c) SaveFile << Deque[c];
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, std::deque<type> &Deque) {
  Deque.resize(ReadType(feuLong, SaveFile), type());
  for (feuLong c = 0; c < Deque.size(); ++c) SaveFile >> Deque[c];
  return SaveFile;
}

template <class type> inline outputfile &operator << (outputfile &SaveFile, const std::list<type> &List) {
  SaveFile << feuLong(List.size());
  for (typename std::list<type>::const_iterator i = List.begin(); i != List.end(); ++i) SaveFile << *i;
  return SaveFile;
}

template <class type> inline inputfile &operator >> (inputfile &SaveFile, std::list<type> &List) {
  List.resize(ReadType(feuLong, SaveFile), type());
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
