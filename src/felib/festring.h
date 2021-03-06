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
#ifndef __FELIB_FESTRING_H__
#define __FELIB_FESTRING_H__

#include <cctype>
#include <vector>
#include <time.h>

#include "felibdef.h"

#define LONG_LONG_PFORMAT  "%lld"

class festring {
private:
  enum { FESTRING_PAGE = 0x7F };

public:
  typedef unsigned int sizetype;
  typedef const unsigned int csizetype;

public:
  inline festring () : Data(0), Size(0), Reserved(0), OwnsData(false) {}

  explicit inline festring (sizetype N) : Size(N), Reserved(N|FESTRING_PAGE), OwnsData(true) {
    if (N != 0) {
      char *Ptr = sizeof(rcint)+new char[Reserved+sizeof(rcint)+1];
      REFS(Ptr) = 0;
      Data = Ptr;
      memset(Data, 0, N);
    } else {
      Data = nullptr;
      Reserved = 0;
      OwnsData = false;
    }
  }

  inline festring (sizetype N, char C) : Size(N), Reserved(N|FESTRING_PAGE), OwnsData(true) {
    if (N != 0) {
      char *Ptr = sizeof(rcint)+new char[Reserved+sizeof(rcint)+1];
      REFS(Ptr) = 0;
      Data = Ptr;
      memset(Data, C, N);
    } else {
      Data = nullptr;
      Reserved = 0;
      OwnsData = false;
    }
  }

  inline festring (cchar *CStr) : Data(const_cast<char *>(CStr)), Size(CStr ? strlen(CStr) : 0), Reserved(0), OwnsData(false) {}

  inline festring (cchar *CStr, sizetype N) : Data(const_cast<char *>(CStr)), Size(N), Reserved(0), OwnsData(false) {
    //if (N > 0x7ffff000) ABORT("String too big (or invalid)");
    if (N == 0) { Data = 0; return; }
    sizetype slen = (CStr ? (sizetype)strlen(CStr) : 0);
    if (slen != N) {
      if (slen > N) slen = N;
      Reserved = N|FESTRING_PAGE;
      char *Ptr = sizeof(rcint)+new char[Reserved+sizeof(rcint)+1];
      REFS(Ptr) = 0;
      Data = Ptr;
      memset(Data, 0, N+1);
      if (slen) memcpy(Data, CStr, slen);
      OwnsData = true;
    }
  }

  inline festring (cfestring &Str) : Data(Str.Data), Size(Str.Size), Reserved(Str.Reserved), OwnsData(Str.OwnsData) {
    if (Data && OwnsData) {
      ++REFS(Data);
    } else if (!Data) {
      // just in case
      Size = 0;
      Reserved = 0;
      OwnsData = false;
    }
  }

  inline ~festring () {
    if (OwnsData && Data) {
      char *Ptr = Data;
      if (!REFS(Ptr)--) delete [] REFSA(Ptr);
    }
    Data = 0;
    Size = 0;
    Reserved = 0;
    OwnsData = false;
  }

  void ResetToLength (sizetype N) {
    Empty(); // everything is zero after this
    if (N != 0) {
      Size = N;
      Reserved = N|FESTRING_PAGE;
      char *Ptr = sizeof(rcint)+new char[Reserved+sizeof(rcint)+1];
      REFS(Ptr) = 0;
      Data = Ptr;
      memset(Data, 0, N);
      OwnsData = true;
    }
  }
  festring &Capitalize ();
  festring CapitalizeCopy () const { return festring(*this).Capitalize(); }
  festring &operator = (cchar *);
  festring &operator = (cfestring &);
  festring &operator << (char);
  festring &operator << (cchar *);
  festring &operator << (cfestring &);
  festring &operator << (short Int) { return Append(Int); }
  festring &operator << (uShort Int) { return Append(Int); }
  festring &operator << (int Int) { return Append(Int); }
  festring &operator << (uInt Int) { return Append((int)Int); } //k8:64
  festring &operator += (char);
  festring &operator += (cchar *);
  festring &operator += (cfestring &);
  //festring &operator << (sLong Int) { return Append(Int); } //k8:64
  //festring &operator << (feuLong Int) { return Append(Int); } //k8:64
  festring &operator << (time_t Int) { return Append((int64_t)Int); } //k8:64
  bool operator < (cfestring &) const;
  bool operator <= (cfestring &) const;
  bool operator > (cfestring &) const;
  bool operator >= (cfestring &) const;
  truth operator == (cfestring &) const;
  truth operator != (cfestring &) const;
  truth operator == (cchar*) const;
  truth operator != (cchar*) const;
  int Compare (cfestring &) const;
  int CompareIgnoreCase (cfestring &) const;
  int Compare (cchar *) const;
  int CompareIgnoreCase (cchar *) const;
  cchar *CStr () const;
  sizetype GetSize () const { return Size; }
  void Empty ();
  void Assign (sizetype, char);
  void Resize (sizetype, char=' ');
  sizetype Find (char, sizetype=0) const;
  sizetype Find (cchar *CStr, sizetype Pos=0) const { return Find(CStr, Pos, strlen(CStr)); }
  sizetype Find (cchar *, sizetype, sizetype) const;
  sizetype Find (cfestring &S, sizetype Pos=0) const { return Find(S.Data, Pos, S.Size); }
  sizetype FindLast (char, sizetype=NPos) const;
  sizetype FindLast (const char *CStr, sizetype Pos=NPos) const { return FindLast(CStr, Pos, strlen(CStr)); }
  sizetype FindLast (const char *, sizetype, sizetype) const;
  sizetype FindLast (const festring& S, sizetype Pos=NPos) const { return FindLast(S.Data, Pos, S.Size); }
  void Erase (sizetype Pos, sizetype Length);
  void Insert (sizetype Pos, cchar *CStr) { Insert(Pos, CStr, strlen(CStr)); }
  void Insert (sizetype Pos, cchar *CStr, sizetype N);
  void Insert (sizetype Pos, cfestring& S) { Insert(Pos, S.Data, S.Size); }
  void Insert (sizetype Pos, char ch) { Insert(Pos, &ch, 1); }
  festring &Append (cfestring &Str, sizetype N) { return Append(Str.Data, N); }

  truth startsWith (cchar *str, int slen=-1) const;
  truth endsWith (cchar *str, int slen=-1) const;
  truth startsWithCI (cchar *str, int slen=-1) const;
  truth endsWithCI (cchar *str, int slen=-1) const;

  truth startsWith (cfestring &str) const;
  truth endsWith (cfestring &str) const;
  truth startsWithCI (cfestring &str) const;
  truth endsWithCI (cfestring &str) const;

  static csizetype NPos;
  static void SplitString (festring &, festring &, sizetype);
  static void SplitStringColored (festring &, festring &, sizetype);
  static int SplitString (cfestring &, std::vector<festring> &, sizetype, sizetype=0);
  static int SplitStringColored (cfestring &, std::vector<festring> &, sizetype, sizetype=0);
  static sizetype IgnoreCaseFind (cfestring &, cfestring &, sizetype=0);
  static void SearchAndReplace (festring &, cfestring &, cfestring &, sizetype=0);
  static bool IgnoreCaseCompare (cfestring &, cfestring &);
  truth IsEmpty () const { return (Size == 0); }
  /* HORRIBLE ERROR!!!! */
  char &operator [] (sizetype Index) const { return Data[Index]; }
  void SwapData (festring &);
  sLong GetCheckSum () const;
  void ensureUniqueOwned (bool allocEmpty=false);

  truth hasCtlCodes () const;
  sizetype rawLength () const; // without color codes

  char *getDataBuffer () { return Data; } // DON'T USE!
  cchar *getConstDataBuffer () const { return Data; } // DON'T USE!

private:
  static void InstallIntegerMap ();
  static void DeInstallIntegerMap ();
  void CreateOwnData (cchar *, sizetype);
  festring &Append (int64_t);
  festring &Append (sLong);
  festring &Append (cchar *, sizetype);
  void SlowAppend (char);
  void SlowAppend (cchar *, sizetype);

private:
  static char **IntegerMap;
  static const char *EmptyString;
  char *Data;
  sizetype Size;
  sizetype Reserved;
  bool OwnsData;
};


class festringpile {
public:
  festringpile (cfestring &String) : String(String) {}
  template <class type> festringpile &operator + (type What) { String << What; return *this; }
  festringpile &operator + (cfestring &What) { String << What; return *this; }
  festringpile &operator + (const festringpile& What) { String << What.String; return *this; }
  operator festring () const { return String; }
private:
  festring String;
};


template <class type> inline festringpile operator + (cfestring& S, type What) { return festringpile(S)+What; }
inline festringpile operator + (cfestring &S, cfestring &What) { return festringpile(S)+What; }
inline festringpile operator + (cfestring &S, const festringpile &What) { return festringpile(S)+What; }


inline bool festring::operator < (cfestring &Str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = Str.Size;
  if (ThisSize && StrSize) {
    int Comp = memcmp(Data, Str.Data, StrSize > ThisSize ? ThisSize : StrSize);
    return Comp < 0 || (!Comp && StrSize > ThisSize);
  }
  return (!ThisSize && StrSize);
}


inline bool festring::operator <= (cfestring &Str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = Str.Size;
  if (ThisSize && StrSize) {
    int Comp = memcmp(Data, Str.Data, StrSize > ThisSize ? ThisSize : StrSize);
    return Comp <= 0 || (StrSize > ThisSize);
  }
  return !ThisSize && StrSize;
}


inline bool festring::operator > (cfestring &Str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = Str.Size;
  if (ThisSize && StrSize) {
    int Comp = memcmp(Data, Str.Data, StrSize > ThisSize ? ThisSize : StrSize);
    return Comp > 0 || (!Comp && ThisSize > StrSize);
  }
  return !ThisSize && StrSize;
}


inline bool festring::operator >= (cfestring &Str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = Str.Size;
  if (ThisSize && StrSize) {
    int Comp = memcmp(Data, Str.Data, StrSize > ThisSize ? ThisSize : StrSize);
    return Comp >= 0 || (ThisSize > StrSize);
  }
  return !ThisSize && StrSize;
}


inline truth festring::operator == (cfestring &Str) const {
  sizetype StrSize = Str.Size;
  return Size == StrSize && (!StrSize || !memcmp(Data, Str.Data, StrSize));
}


inline truth festring::operator != (cfestring &Str) const {
  sizetype StrSize = Str.Size;
  return Size != StrSize || (StrSize && memcmp(Data, Str.Data, StrSize));
}


inline truth festring::operator == (cchar *CStr) const {
  sizetype StrSize = strlen(CStr);
  return Size == StrSize && (!StrSize || !memcmp(Data, CStr, StrSize));
}


inline truth festring::operator != (cchar *CStr) const {
  sizetype StrSize = strlen(CStr);
  return Size != StrSize || (StrSize && memcmp(Data, CStr, StrSize));
}


/* Returns -1 if this is before Str in alphabetical order, zero
   if strings are identical, else 1 */
inline int festring::Compare (cfestring &Str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = Str.Size;
  if (ThisSize && StrSize) {
    int Comp = memcmp(Data, Str.Data, (StrSize > ThisSize ? ThisSize : StrSize));
    if (Comp) return Comp;
  }
  return (ThisSize < StrSize ? -1 : ThisSize != StrSize);
}


inline int festring::CompareIgnoreCase (cfestring &Str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = Str.Size;
  if (ThisSize && StrSize) {
    for (sizetype Pos = 0; Pos < GetSize(); ++Pos) {
      char Char1 = toupper(Data[Pos]);
      char Char2 = toupper(Str[Pos]);
      if (Char1 != Char2) return (Char1 < Char2 ? -1 : 1);
    }
    return 0;
  }
  return (ThisSize < StrSize ? -1 : ThisSize != StrSize);
}


inline int festring::Compare (cchar *str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = (sizetype)(str ? strlen(str) : 0);
  if (ThisSize && StrSize) {
    int Comp = memcmp(Data, str, (StrSize > ThisSize ? ThisSize : StrSize));
    if (Comp) return Comp;
  }
  return (ThisSize < StrSize ? -1 : ThisSize != StrSize);
}


inline int festring::CompareIgnoreCase (cchar *str) const {
  sizetype ThisSize = Size;
  sizetype StrSize = (sizetype)(str ? strlen(str) : 0);
  if (ThisSize && StrSize) {
    for (sizetype Pos = 0; Pos < GetSize(); ++Pos) {
      char Char1 = toupper(Data[Pos]);
      char Char2 = toupper(str[Pos]);
      if (Char1 != Char2) return (Char1 < Char2 ? -1 : 1);
    }
    return 0;
  }
  return (ThisSize < StrSize ? -1 : ThisSize != StrSize);
}


inline cchar *festring::CStr () const {
  if (Data) {
    // it is always safe to write zero into owned data
    if (OwnsData) Data[Size] = 0;
    return Data;
  }
  return EmptyString;
}


inline void festring::Empty () {
  if (OwnsData) {
    char *Ptr = Data;
    if (Ptr && REFS(Ptr)) {
      if (!REFS(Ptr)--) delete [] REFSA(Ptr);
    }
  }
  Data = 0;
  Size = 0;
  Reserved = 0;
  OwnsData = false;
}


inline festring &festring::operator << (char Char) {
  if (OwnsData && Data && !REFS(Data) && Size < Reserved) {
    Data[Size++] = Char;
  } else {
    SlowAppend(Char);
  }
  return *this;
}


inline festring &festring::operator << (cchar *CStr) {
  sizetype N = (CStr ? strlen(CStr) : 0);
  if (N == 0) return *this;
  sizetype NewSize = Size+N;
  if (OwnsData && Data && !REFS(Data) && NewSize <= Reserved) {
    if (N > 0) memmove(Data+Size, CStr, N);
    Size = NewSize;
  } else {
    SlowAppend(CStr, N);
  }
  return *this;
}


inline festring &festring::operator << (cfestring &Str) {
  sizetype N = Str.Size;
  if (N == 0) return *this;
  sizetype NewSize = Size+N;
  char *OtherPtr = Str.Data;
  if (OwnsData && Data && !REFS(Data) && NewSize <= Reserved) {
    if (N > 0) memmove(Data+Size, OtherPtr, N);
    Size = NewSize;
  } else {
    SlowAppend(OtherPtr, N);
  }
  return *this;
}


inline festring &festring::operator += (char Char) { return *this << Char; }
inline festring &festring::operator += (cchar *CStr) { return *this << CStr; }
inline festring &festring::operator += (cfestring &Str) { return *this << Str; }


struct charcomparer {
  bool operator () (cchar *const &S1, cchar *const &S2) const { return strcmp(S1, S2) < 0; }
};


struct ignorecaseorderer {
  bool operator () (cfestring &S1, cfestring &S2) const { return festring::IgnoreCaseCompare(S1, S2); }
};


//#define CONST_S(str) festring(str, sizeof(str)-1)
#define CONST_S(str) festring(str)


/*
 * This macro doesn't evaluate with if what
 * is not found so it's often faster
 */
#define SEARCH_N_REPLACE(where, what, with) \
  if (where.Find(what) != festring::NPos) festring::SearchAndReplace(where, what, with);


// ////////////////////////////////////////////////////////////////////////// //
#include <string>
#include <cstdlib>
#include <cxxabi.h>

template<typename T> festring getCPPTypeName () {
  int status;
  std::string name = typeid(T).name();
  char *demangled = abi::__cxa_demangle(name.c_str(), 0/*NULL*/, 0/*NULL*/, &status);
  if (status == 0) {
    name = demangled;
    std::free(demangled);
  }
  festring res;
  res << name.c_str();
  return res;
}


#endif
