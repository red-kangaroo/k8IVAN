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
#include <cctype>
#include <cstdlib>
#include <cstdio>

#include "festring.h"
#include "allocate.h"
#include "feerror.h"


char **festring::IntegerMap = 0;
const char *festring::EmptyString = "";
festring::csizetype festring::NPos = festring::sizetype(-1);


festring &festring::Append (cchar *CStr, sizetype N) {
  sizetype OldSize = Size;
  sizetype NewSize = OldSize+N;
  char *OldPtr = Data;
  if (OwnsData && OldPtr && !REFS(OldPtr) && NewSize <= Reserved) {
    if (N > 0) memmove(OldPtr+OldSize, CStr, N);
    Size = NewSize;
  } else {
    SlowAppend(CStr, N);
  }
  return *this;
}


festring &festring::operator = (cchar *CStr) {
  sizetype NewSize = strlen(CStr);
  Size = NewSize;
  char *Ptr = Data;
  if (Ptr && OwnsData) {
    if(!REFS(Ptr) && NewSize <= Reserved) {
      if (NewSize > 0) memmove(Ptr, CStr, NewSize);
      return *this;
    }
    if (!REFS(Ptr)--) delete [] REFSA(Ptr);
  }
  Data = const_cast<char *>(CStr);
  OwnsData = false;
  return *this;
}


festring &festring::operator = (cfestring &Str) {
  sizetype NewSize = Str.Size;
  Size = NewSize;
  char *Ptr = Data;
  char *StrPtr = Str.Data;
  if (Ptr && OwnsData) {
    if (!REFS(Ptr) && NewSize <= Reserved) {
      if (StrPtr && NewSize > 0) memmove(Ptr, StrPtr, NewSize);
      return *this;
    }
    if (!REFS(Ptr)--) delete [] REFSA(Ptr);
  }
  if ((Data = StrPtr) && (OwnsData = Str.OwnsData)) {
    ++REFS(StrPtr);
    Reserved = Str.Reserved;
  }
  return *this;
}


/* Size must be > 0 */
festring &festring::Capitalize () {
  char *OldPtr = Data;
  if (*OldPtr > 0x60 && *OldPtr < 0x7B) {
    if (!OwnsData) CreateOwnData(OldPtr, Size);
    else if (REFS(OldPtr)) {
      --REFS(OldPtr);
      CreateOwnData(OldPtr, Size);
    }
    *Data ^= 0x20;
  }
  return *this;
}


void festring::CreateOwnData (cchar *CStr, sizetype N) {
  Size = N;
  Reserved = N|FESTRING_PAGE;
  char *Ptr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
  REFS(Ptr) = 0;
  Data = Ptr;
  if (N > 0) memmove(Ptr, CStr, N);
  OwnsData = true;
}


void festring::SlowAppend (char Char) {
  char *OldPtr = Data;
  if (OldPtr) {
    sizetype OldSize = Size++;
    sizetype NewSize = OldSize+1;
    feuLong *DeletePtr = 0;
    if (OwnsData && !REFS(OldPtr)--) DeletePtr = REFSA(OldPtr);
    Reserved = NewSize|FESTRING_PAGE;
    char *NewPtr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
    REFS(NewPtr) = 0;
    Data = NewPtr;
    if (OldSize > 0) memmove(NewPtr, OldPtr, OldSize);
    NewPtr[OldSize] = Char;
    NewPtr[OldSize+1] = 0;
    if (DeletePtr) delete [] DeletePtr;
  } else {
    Size = 1;
    Reserved = FESTRING_PAGE;
    char *Ptr = sizeof(int*)+new char[FESTRING_PAGE+sizeof(int*)+1];
    REFS(Ptr) = 0;
    Ptr[0] = Char;
    Data = Ptr;
  }
  OwnsData = true;
}


void festring::SlowAppend (cchar *CStr, sizetype N) {
  char *OldPtr = Data;
  //
  if (OldPtr) {
    sizetype OldSize = Size;
    sizetype NewSize = OldSize+N;
    Size = NewSize;
    feuLong *DeletePtr = 0;
    if (OwnsData && !REFS(OldPtr)--) DeletePtr = REFSA(OldPtr);
    Reserved = NewSize|FESTRING_PAGE;
    char *NewPtr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
    REFS(NewPtr) = 0;
    Data = NewPtr;
    if (OldSize > 0) memmove(NewPtr, OldPtr, OldSize);
    if (N > 0) memmove(NewPtr+OldSize, CStr, N);
    NewPtr[Size] = 0;
    OwnsData = true;
    if (DeletePtr) delete [] DeletePtr;
  } else {
    CreateOwnData(CStr, N);
  }
}


void festring::Assign (sizetype N, char C) {
  Size = N;
  char *Ptr = Data;
  if (OwnsData && Ptr) {
    if (!REFS(Ptr) && N <= Reserved) {
      memset(Ptr, C, N);
      return;
    }
    delete [] REFSA(Ptr);
  }
  Reserved = N|FESTRING_PAGE;
  Ptr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
  REFS(Ptr) = 0;
  Data = Ptr;
  memset(Ptr, C, N);
  OwnsData = true;
}


void festring::Resize (sizetype N, char C) {
  sizetype OldSize = Size;
  char *OldPtr = Data;
  char *NewPtr;
  Size = N;
  if (OldSize < N) {
    feuLong *DeletePtr = 0;
    if (OwnsData && OldPtr) {
      if (!REFS(OldPtr)) {
        if (N <= Reserved) {
          memset(OldPtr+OldSize, C, N-OldSize);
          return;
        }
        DeletePtr = REFSA(OldPtr);
      } else {
        --REFS(OldPtr);
      }
    }
    Reserved = N|FESTRING_PAGE;
    NewPtr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
    REFS(NewPtr) = 0;
    Data = NewPtr;
    if (OldSize > 0) memmove(NewPtr, OldPtr, OldSize);
    if (N > OldSize) memset(NewPtr+OldSize, C, N-OldSize);
    OwnsData = true;
    if (DeletePtr) delete [] DeletePtr;
  } else {
    if (OwnsData && OldPtr) {
      if (!REFS(OldPtr)) return;
      --REFS(OldPtr);
    }
    Reserved = N|FESTRING_PAGE;
    NewPtr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
    REFS(NewPtr) = 0;
    Data = NewPtr;
    if (N > 0) memmove(NewPtr, OldPtr, N);
    OwnsData = true;
  }
}


festring::sizetype festring::Find (char Char, sizetype Pos) const {
  char *Ptr = Data;
  if (Ptr) {
    char *Result = static_cast<char *>(memchr(Ptr+Pos, Char, Size-Pos));
    if (Result) return Result-Ptr;
  }
  return NPos;
}


festring::sizetype festring::Find (cchar *CStr, sizetype Pos, sizetype N) const {
  if (N && Pos < Size) {
    char *Ptr = Data;
    if (Ptr) {
      char Char = CStr[0];
      while (Size-Pos >= N) {
        if (Size-Pos < N) return NPos;
        char *Result = static_cast<char *>(memchr(Ptr+Pos, Char, Size-Pos));
        if (!Result) return NPos;
        if (!memcmp(Result, CStr, N)) return Result-Ptr;
        Pos = Result-Ptr+1;
      }
    }
  }
  return NPos;
}


festring::sizetype festring::FindLast (char Char, sizetype Pos) const {
  char *Ptr = Data;
  if (Ptr) {
    if (Pos >= Size) Pos = Size-1;
    sizetype c;
    for (c = Pos; c != NPos && Ptr[c] != Char; --c);
    return c;
  }
  return NPos;
}


festring::sizetype festring::FindLast (const char *CStr, sizetype Pos, sizetype N) const {
  if (N) {
    char *Ptr = Data;
    if (Ptr && Size >= N) {
      char Char = CStr[0];
      if (Pos > Size-N) Pos = Size-N;
      for (sizetype c = Pos; c != NPos; --c) if (Ptr[c] == Char && !memcmp(Ptr+c, CStr, N)) return c;
      return NPos;
    }
  }
  return NPos;
}


void festring::Erase (sizetype Pos, sizetype Length) {
  char *OldPtr = Data;
  if (OldPtr && Length) {
    sizetype OldSize = Size;
    if (Pos < OldSize) {
      truth MoveReq = Length < OldSize-Pos;
      if (OwnsData) {
        if (!REFS(OldPtr)) {
          if (MoveReq) {
            sizetype End = Pos+Length;
            memmove(OldPtr+Pos, OldPtr+End, OldSize-End);
          }
          Size -= Length;
          return;
        } else {
          --REFS(OldPtr);
        }
      }
      sizetype NewSize = MoveReq ? OldSize-Length : Pos;
      Size = NewSize;
      Reserved = NewSize|FESTRING_PAGE;
      char *Ptr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
      REFS(Ptr) = 0;
      Data = Ptr;
      OwnsData = true;
      if (Pos > 0) memmove(Ptr, OldPtr, Pos);
      if (MoveReq) {
        sizetype End = Pos+Length;
        if (OldSize > End) memmove(Ptr+Pos, OldPtr+End, OldSize-End);
      }
    }
  }
}


void festring::Insert (sizetype Pos, cchar *CStr, sizetype N) {
  if (N) {
    sizetype OldSize = Size;
    if (Pos < OldSize) {
      // this implies Data != 0
      char *OldPtr = Data;
      feuLong *DeletePtr = 0;
      sizetype NewSize = OldSize+N;
      Size = NewSize;
      if (OwnsData) {
        if (!REFS(OldPtr)) {
          if (NewSize <= Reserved) {
            char *Ptr = OldPtr+Pos;
            memmove(Ptr+N, Ptr, OldSize-Pos);
            if (N > 0) memmove(Ptr, CStr, N);
            return;
          } else {
            DeletePtr = REFSA(OldPtr);
          }
        } else {
          --REFS(OldPtr);
        }
      }
      Reserved = NewSize|FESTRING_PAGE;
      char* NewPtr = sizeof(int*)+new char[Reserved+sizeof(int*)+1];
      REFS(NewPtr) = 0;
      Data = NewPtr;
      if (Pos > 0) memmove(NewPtr, OldPtr, Pos);
      if (N > 0) memmove(NewPtr+Pos, CStr, N);
      if (OldSize > Pos) memmove(NewPtr+Pos+N, OldPtr+Pos, OldSize-Pos);
      OwnsData = true;
      if (DeletePtr) delete [] DeletePtr;
    } else if (Pos == OldSize) {
      Append(CStr, N);
    } else {
      ABORT("Illegal festring insertion detected!");
    }
  }
}


/* Creates map of char representations of numbers 0-999 used by
 * festring::Append(sLong). Due to automatization, you don't need
 * to explicitly call it. */
void festring::InstallIntegerMap () {
  Alloc2D(IntegerMap, 1000, 3);
  char Ones = '0', Tens = '0', Hundreds = '0';
  for (int c = 0; c < 1000; ++c) {
    IntegerMap[c][0] = Hundreds;
    IntegerMap[c][1] = Tens;
    IntegerMap[c][2] = Ones;
    if (++Ones > '9') {
      Ones = '0';
      if (++Tens > '9') {
        Tens = '0';
        ++Hundreds;
      }
    }
  }
  atexit(DeInstallIntegerMap);
}


/* Deletes the integer map used by festring::Append(sLong).
 * Due to automatization, you don't need to explicitly call it. */
void festring::DeInstallIntegerMap () {
  delete [] IntegerMap;
  IntegerMap = 0;
}


/* Displays numbers in the range [-2147483647, 2147483647].
 * Much faster than sprintf and (nonstandard) itoa. */
festring &festring::Append (sLong Integer) {
  if (!IntegerMap) InstallIntegerMap();
  char IntegerBuffer[12];
  char *BufferPtr = IntegerBuffer;
  truth Negative = false;
  if (Integer < 0) {
    if (Integer < -2147483647) return Append("-Inf", 4);
    *BufferPtr++ = '0';
    Integer = -Integer;
    Negative = true;
  } else if (Integer > 2147483647) return Append("Inf", 3);
  truth ForceZeros = false;
  if (Integer >= 2000000000) {
    *BufferPtr++ = '2';
    Integer -= 2000000000;
    ForceZeros = true;
  } else if (Integer >= 1000000000) {
    *BufferPtr++ = '1';
    Integer -= 1000000000;
    ForceZeros = true;
  }
  if (ForceZeros || Integer >= 1000000) {
    int Temp = Integer/1000000;
    *BufferPtr++ = IntegerMap[Temp][0];
    *BufferPtr++ = IntegerMap[Temp][1];
    *BufferPtr++ = IntegerMap[Temp][2];
    Integer -= Temp*1000000;
    ForceZeros = true;
  }
  if (ForceZeros || Integer >= 1000) {
    int Temp = Integer/1000;
    *BufferPtr++ = IntegerMap[Temp][0];
    *BufferPtr++ = IntegerMap[Temp][1];
    *BufferPtr++ = IntegerMap[Temp][2];
    Integer -= Temp*1000;
  }
  *BufferPtr++ = IntegerMap[Integer][0];
  *BufferPtr++ = IntegerMap[Integer][1];
  *BufferPtr++ = IntegerMap[Integer][2];
  *BufferPtr = 0;
  char *EndPtr = BufferPtr;
  for (BufferPtr = IntegerBuffer; *BufferPtr == '0'; ++BufferPtr);
  if (Negative) *--BufferPtr = '-';
  else if (!*BufferPtr) --BufferPtr; // check if the original Integer was zero
  return Append(BufferPtr, EndPtr-BufferPtr);
}


//FIXME: buffer overflows?
//FIXME: windoze?
festring &festring::Append (int64_t Integer) {
  char buf[4096];
  //
  snprintf(buf, sizeof(buf)-1, LONG_LONG_PFORMAT, Integer);
  buf[sizeof(buf)-1] = 0;
  return Append(buf, strlen(buf));
}


/* The Result string receives up to Length characters from source,
 * but words are left uncut if possible. */
void festring::SplitString (festring &Source, festring &Result, sizetype Length) {
  if (Source.GetSize() <= Length) {
    Result << Source;
    Source.Empty();
    return;
  }
  sizetype Pos = Source.FindLast(' ', Length);
  if (Pos != NPos) {
    Result.Append(Source, Pos);
    Source.Erase(0, Pos+1);
  } else {
    Result.Append(Source, Length);
    Source.Erase(0, Length);
  }
}


/* The Result string receives up to Length characters from source,
 * but words are left uncut if possible. */
void festring::SplitStringColored (festring &Source, festring &Result, sizetype Length) {
  if (Source.GetSize() <= Length || Length < 1) {
    Result << Source;
    Source.Empty();
    return;
  }

  char clrchar = 0;
  sizetype len = 0; // collected

  // add words
  while (Source.Size > 0) {
    cchar *s = Source.Data;
    sizetype left = Source.Size;
    sizetype wordlen = 0;

    // skip leading spaces
    while (left > 0) {
      if (*s == '\1') { ++s; --left; if (left) { clrchar = *s++; --left; } continue; }
      if (*s == '\2') { clrchar = 0; ++s; --left; continue; }
      if (*s != ' ') break;
      ++s;
      --left;
      ++wordlen;
    }

    // find word end
    while (left > 0 && *s != ' ') {
      if (*s == '\1') { ++s; --left; if (left) { clrchar = *s++; --left; } continue; }
      if (*s == '\2') { clrchar = 0; ++s; --left; continue; }
      ++wordlen;
      ++s;
      --left;
    }

    sizetype cplen = (sizetype)(s-Source.Data);

    if (len == 0 && wordlen > Length) {
      // oops, first word is too long, do it specially
      cchar *s = Source.Data;
      sizetype left = Source.Size;
      // append chars up to `Length`
      wordlen = 0;
      while (left > 0) {
        if (*s == '\1') { ++s; --left; if (left) { clrchar = *s++; --left; } continue; }
        if (*s == '\2') { clrchar = 0; ++s; --left; if (wordlen == Length) break; continue; }
        if (wordlen == Length) break;
        ++wordlen;
        ++s;
        --left;
      }
      cplen = (sizetype)(s-Source.Data);
      Result.Append(Source.Data, cplen);
      Source.Erase(0, cplen);
      // insert color code
      if (clrchar != 0) { char ccbuf[2]; ccbuf[0] = '\1'; ccbuf[1] = clrchar; Source.Insert(0, ccbuf, 2); }
      return;
    }

    // fits?
    if (len == 0 || len+wordlen <= Length) {
      // yes, append it
      Result.Append(Source.Data, cplen);
      len += wordlen;
      // erase appended part
      Source.Erase(0, cplen);
      continue;
    }

    // can't append; remove leading space (if any)
    if (Source.Size > 0 && *Source.Data == ' ') Source.Erase(0, 1);
    // insert color code
    if (clrchar != 0) { char ccbuf[2]; ccbuf[0] = '\1'; ccbuf[1] = clrchar; Source.Insert(0, ccbuf, 2); }
    return;
  }

  // clear `Source`
  if (Source.Size > 0) Source.Erase(0, Source.Size);
}


/* Divides Source into lines of size up to Length without cutting words
 * and stores them one by one to StringVector. You can also specify a
 * Marginal, in which case a number of spaces is inserted in the
 * beginning of each line except the first. It returns the number of
 * created lines. */
int festring::SplitString (cfestring &Source, std::vector<festring> &StringVector, sizetype Length, sizetype Marginal) {
  if (!Length) ABORT("Illegal Length 0 passed to festring::SplitString()!");
  if (Marginal >= Length) ABORT("Illegal festring::SplitString() call: Marginal must be less than Length!");
  festring CopyOfSource(Source);
  if (StringVector.empty()) StringVector.push_back(festring()); else StringVector[0].Empty();
  SplitString(CopyOfSource, StringVector[0], Length);
  sizetype Size = 1;
  while (!CopyOfSource.IsEmpty()) {
    if (StringVector.size() <= Size) StringVector.push_back(festring());
    festring &String = StringVector[Size++];
    String.Assign(Marginal, ' ');
    SplitString(CopyOfSource, String, Length-Marginal);
  }
  return Size;
}


int festring::SplitStringColored (cfestring &Source, std::vector<festring> &StringVector, sizetype Length, sizetype Marginal) {
  if (!Length) ABORT("Illegal Length 0 passed to festring::SplitStringColored()!");
  if (Marginal >= Length) ABORT("Illegal festring::SplitStringColored() call: Marginal must be less than Length!");
  festring CopyOfSource(Source);
  if (StringVector.empty()) StringVector.push_back(festring()); else StringVector[0].Empty();
  SplitStringColored(CopyOfSource, StringVector[0], Length);
  sizetype Size = 1;
  while (!CopyOfSource.IsEmpty()) {
    if (StringVector.size() <= Size) StringVector.push_back(festring());
    festring &String = StringVector[Size++];
    String.Assign(Marginal, ' ');
    SplitStringColored(CopyOfSource, String, Length-Marginal);
  }
  return Size;
}


truth festring::hasCtlCodes () const {
  cchar *str = Data;
  for (sizetype c = Size; c > 0; --c, ++str) {
    if (*str == '\1' || *str == '\2') return true;
  }
  return false;
}


// without color codes
festring::sizetype festring::rawLength () const {
  sizetype len = 0;
  cchar *str = Data;
  for (sizetype c = Size; c > 0; --c, ++str) {
    if (*str == '\1' || *str == '\2') {
      if (*str == '\1') {
        if (c == 1) break;
        ++str;
        --c;
      }
    } else {
      ++len;
    }
  }
  return len;
}


static inline char Capitalize (char Char) { return Char > 0x60 && Char < 0x7B ? Char ^ 0x20 : Char; }


/* Returns the position of the first occurance of What in Where
 * starting at Begin or after it, ignoring the case of letters.
 * If the search fails, festring::NPos is returned instead. */
festring::sizetype festring::IgnoreCaseFind (cfestring &Where, cfestring &What, sizetype Begin) {
  if (What.IsEmpty()) return Begin;
  for (; Where.GetSize() >= What.GetSize() + Begin; ++Begin) {
    if (::Capitalize(Where[Begin]) == ::Capitalize(What[0])) {
      truth Equal = true;
      for (sizetype c = 1; c < What.GetSize(); ++c)
      if (::Capitalize(Where[Begin + c]) != ::Capitalize(What[c])) {
        Equal = false;
        break;
      }
      if (Equal) return Begin;
    }
  }
  return NPos;
}


/* Replaces all occurances of What in Where after Begin with With */
void festring::SearchAndReplace (festring &Where, cfestring &What, cfestring &With, sizetype Begin) {
  for (sizetype Pos = Where.Find(What, Begin); Pos != NPos; Pos = Where.Find(What, Pos)) {
    Where.Erase(Pos, What.GetSize());
    Where.Insert(Pos, With);
  }
}


/* Returns whether First is behind Second in alphabetical order, ignoring case */
bool festring::IgnoreCaseCompare (cfestring &First, cfestring & Second) {
  for (sizetype Pos = 0; Pos < First.GetSize() && Pos < Second.GetSize(); ++Pos) {
    char Char1 = ::Capitalize(First[Pos]);
    char Char2 = ::Capitalize(Second[Pos]);
    if (Char1 != Char2) return Char1 < Char2;
  }
  return First.GetSize() < Second.GetSize();
}


/* Sorry for ugliness */
void festring::PreProcessForFebot () {
  sizetype c, d, Length;
  for (c = 0, Length = 0; c < Size && (Data[c] == ' ' || Data[c] == '\t'); ++c) ++Length;
  Erase(0, Length);
  if (!Size) return;
  for (c = Size-1, Length = 0; Data[c] == ' ' || Data[c] == '\t'; --c) ++Length;
  Erase(c+1, Length);
  for (c = 0; c < Size - 1; ++c) {
    char Char = Data[c+1];
    if (Data[c] == '\t') Data[c] = ' ';
    else if (Data[c] == '\"' || Data[c] == '(' || Data[c] == ')') {
      Erase(c--, 1);
      continue;
    }
    if (Data[c] == ' ') {
      if (Char == ' ' || Char == '\t') {
        for (d = c+2, Length = 1; d < Size && (Data[d] == ' ' || Data[d] == '\t'); ++d) ++Length;
        Erase(c+1, Length);
      }
    } else if ((Char == '.' || Char == '!' || Char == '?') &&
               Data[c] != '.' && Data[c] != '!' && Data[c] != '?' &&
               (c == Size-2 || Data[c+2] == ' ' || Data[c+2] == '\t')) Insert(c+++1, " ", 1);
  }
  if (Data[c] == '\"' || Data[c] == '(' || Data[c] == ')') Erase(c--, 1);
  if (!ispunct(Data[c])) *this << ' ' << '.';
}


/* Sorry for ugliness */
void festring::PostProcessForFebot () {
  Capitalize();
  truth CapitalizeNextChar = false;
  for (sizetype c = 0; c < Size-1; ++c) {
    char Char1 = Data[c];
    if (Char1 == ' ') {
      char Char2 = Data[c+1];
      if ((Char2 == '.' || Char2 == '!' || Char2 == '?') && (c == Size-2 || Data[c+2] == ' ' || Data[c+2] == '\t')) {
        Erase(c++, 1);
        CapitalizeNextChar = true;
      }
    } else if ((Char1 == '.' || Char1 == '!' || Char1 == '?') &&
               (c == Size - 2 || Data[c+2] == ' ' || Data[c+2] == '\t')) CapitalizeNextChar = true;
    /* Erase() guarantees that OwnsData != false && REFS(Data) == 0 */
    else if (CapitalizeNextChar) {
      if (Char1 > 0x60 && Char1 < 0x7B) Data[c] &= ~0x20;
      CapitalizeNextChar = false;
    }
  }
}


/* Erases the first word of the sentence and places it to To.
 * Should currently be used only by strings processed for Febot */
void festring::ExtractWord (festring &To) {
  for (sizetype c = 0; c < Size; ++c) {
    if (Data[c] == ' ') {
      To.Empty();
      To.Append(&Data[c+1], Size-c-1);
      Erase(c, Size-c);
      SwapData(To);
      return;
    }
  }
  To = *this;
  Empty();
}


void festring::SwapData (festring &Str) {
  char *const TData = Data;
  csizetype TSize = Size;
  csizetype TReserved = Reserved;
  ctruth TOwnsData = OwnsData;
  Data = Str.Data;
  Size = Str.Size;
  Reserved = Str.Reserved;
  OwnsData = Str.OwnsData;
  Str.Data = TData;
  Str.Size = TSize;
  Str.Reserved = TReserved;
  Str.OwnsData = TOwnsData;
}


sLong festring::GetCheckSum () const {
  sLong Counter = 0;
  for (uShort c = 0; c < GetSize(); ++c) Counter = sLong(Data[c]);
  return Counter;
}


void festring::EnsureOwnsData () {
  if (!OwnsData) CreateOwnData(Data, Size);
}
