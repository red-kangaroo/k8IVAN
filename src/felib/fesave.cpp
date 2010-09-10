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
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>

#include "fesave.h"
#include "femath.h"


truth inputfile::fileExists (const festring &fname) {
  struct stat st;
  if (stat(fname.CStr(), &st)) return false;
  if (!S_ISREG(st.st_mode)) return false;
  return access(fname.CStr(), R_OK) == 0;
}


festring inputfile::GetMyDir (void) {
  char buf[128], myDir[8192];
  pid_t mypid = getpid();
  memset(myDir, 0, sizeof(myDir));
  sprintf(buf, "/proc/%u/exe", (unsigned int)mypid);
  if (readlink(buf, myDir, sizeof(myDir)-1) < 0) strcpy(myDir, ".");
  else {
    char *p = (char *)strrchr(myDir, '/');
    if (!p) strcpy(myDir, "."); else *p = '\0';
  }
  if (myDir[strlen(myDir)-1] == '/') myDir[strlen(myDir)-1] = '\0';
  return myDir;
}


outputfile::outputfile (cfestring &FileName, truth AbortOnErr) :
  Buffer(fopen(FileName.CStr(), "wb")),
  FileName(FileName)
{
  if (AbortOnErr && !IsOpen()) ABORT("Can't open %s for output!", FileName.CStr());
}


outputfile::~outputfile () {
  if (Buffer) fclose(Buffer);
}


void outputfile::ReOpen () {
  fclose(Buffer);
  Buffer = fopen(FileName.CStr(), "ab");
}


inputfile::inputfile (cfestring &FileName, const valuemap *ValueMap, truth AbortOnErr) :
  Buffer(fopen(FileName.CStr(), "rb")),
  FileName(FileName),
  ValueMap(ValueMap),
  lastWordWasString(false)
{
  if (AbortOnErr && !IsOpen()) ABORT("File %s not found!", FileName.CStr());
}


inputfile::~inputfile () {
  if (Buffer) fclose(Buffer);
}


festring inputfile::findVar (cfestring &name, truth *found) const {
  if (found) *found = true;
  for (uint f = 0; f < mVars.size(); f++) {
    std::pair<festring, festring> v = mVars[f];
    if (v.first == name) return v.second;
  }
  if (found) *found = false;
  return "";
}


festring inputfile::getVar (cfestring &name) {
  truth found;
  festring res = findVar(name, &found);
  if (!found) {
    if (mGetVar) {
      res = mGetVar(name);
    } else {
      festring s = "unknown variable: "+name;
      die(s);
    }
  }
  return res;
}


void inputfile::setVar (cfestring &name, cfestring &value) {
  for (uint f = 0; f < mVars.size(); f++) {
    std::pair<festring, festring> v = mVars[f];
    if (v.first == name) {
      v.second = value;
      mVars[f] = v;
      break;
    }
  }
}


void inputfile::delVar (cfestring &name) {
  ABORT("no delVar() yet");
}


void inputfile::die (cfestring &msg) {
  ABORT("ERROR in file %s, line %ld: %s", GetFileName().CStr(), TellLine(), msg.CStr());
}


// 0: term
// 1: unary
// 2: comparisons
// 3: &&
// 4: ||
const int maxCPrio = 4;
const char *opers[5][7] = {
  {NULL},
  {NULL},
  {"<", ">", "<=", ">=", "==", "!=", NULL},
  {"&&", NULL},
  {"||", NULL}
};

festring inputfile::readCondition (festring &token, int prio, truth skipIt) {
  festring res, op1, opc;
  //fprintf(stderr, "IN:  prio: %d; skip: %s; [%s]\n", prio, skipIt?"t":"o", token.CStr());
  switch (prio) {
    case 0: // term
      if (token == "(") {
        readWordIntr(token, true);
        res = readCondition(token, maxCPrio, skipIt);
        if (token != ")") die("')' expected");
      } else if (token == "@") {
        readWordIntr(token, true);
        if (!skipIt) res = getVar(token);
      } else res = token;
      readWordIntr(token, true);
      goto done;
      //return res;
    case 1:
      if (token == "!") {
        readWordIntr(token, true);
        res = readCondition(token, 1, skipIt);
        if (!skipIt) {
          if (res == "") res = "tan"; else res = "";
        }
      } else {
        res = readCondition(token, prio-1, skipIt);
      }
      goto done;
      //return res;
  }
  //
  if (prio > 4) return res;
  res = readCondition(token, prio-1, skipIt);
  for (;;) {
    //readWordIntr(token, true);
    bool myOp = false;
    if (token == "=") die("no assignments yet!");
    if (token == ";") {
      //fprintf(stderr, " RET: [%s]\n", res.CStr());
      break;
    }
    if (token == "less") token = "<";
    else if (token == "great") token = ">";
    else if (token == "equ") token = "==";
    else if (token == "neq") token = "!=";
    else if (token == "lessequ") token = "<=";
    else if (token == "greatequ") token = ">=";
    for (int f = 0; opers[prio][f]; f++) {
      if (!strcmp(opers[prio][f], token.CStr())) { myOp = true; break; }
    }
    //fprintf(stderr, "tk: [%s]; %s\n", token.CStr(), myOp?"MY":"skip");
    if (!myOp) break;
    opc = token;
    readWordIntr(token, true);
    op1 = readCondition(token, prio-1, skipIt);
    //fprintf(stderr, " prio: %d; opc=[%s]; res=[%s]; op1=[%s]\n", prio, opc.CStr(), res.CStr(), op1.CStr());
    switch (prio) {
      case 2: // comparisons
        if (opc == "==") {
          if (!skipIt) res = res==op1 ? "tan" : "";
        } else if (opc == "!=") {
          if (!skipIt) res = res!=op1 ? "tan" : "";
        } else if (opc == "<") {
          if (!skipIt) res = res<op1 ? "tan" : "";
        } else if (opc == ">") {
          if (!skipIt) res = res>op1 ? "tan" : "";
        } else if (opc == "<=") {
          if (!skipIt) res = res<=op1 ? "tan" : "";
        } else if (opc == ">=") {
          if (!skipIt) res = res>=op1 ? "tan" : "";
        }
        break;
      case 3: // &&
        if (opc == "&&") {
          if (!skipIt) {
            res = res!=""&&op1!="" ? "tan" : "";
            if (res == "") skipIt = true;
          }
        }
        break;
      case 4: // ||
        if (opc == "||") {
          if (!skipIt) {
            res = res!=""||op1!="" ? "tan" : "";
            if (res != "") skipIt = true;
          }
        }
        break;
      default:
        die("invalid priority");
    }
  }
done:
  //fprintf(stderr, "OUT: prio: %d; skip: %s; [%s]\n", prio, skipIt?"t":"o", token.CStr());
  return res;
}


// stack top:
//  1: processing 'then'
//  2: processing 'else'
// -1: skiping 'then'
// -2: skiping 'else'
// -3: skiping whole 'if', 'then' part
// -4: skiping whole 'if', 'else' part
// -666: skiping '{}'
// 666: in '{}', processing
void inputfile::ReadWord (festring &str, truth AbortOnEOF, truth skipIt) {
  int prc;
  for (;;) {
    if (!mIfStack.empty()) prc = mIfStack.top(); else prc = 0;
    readWordIntr(str, AbortOnEOF);
    if (str == "if") {
      readWordIntr(str, true);
      festring res = readCondition(str, maxCPrio, prc<0);
      if (str != ";") die("';' expected");
      if (prc < 0) {
        // skiping
        mIfStack.push(-3);
      } else {
        mIfStack.push(res!="" ? 1 : -1);
      }
      continue;
    }
    if (str == "else") {
      switch (prc) {
        case 1: // processing 'then'
          mIfStack.pop();
          mIfStack.push(-2);
          break;
        case -1: // skiping 'then'
          mIfStack.pop();
          mIfStack.push(2);
          break;
        case -3: // skiping whole, 'then'
          mIfStack.pop();
          mIfStack.push(-4);
          break;
        default: die("unexpected 'else'");
      }
      continue;
    }
    if (str == "endif") {
      switch (prc) {
        case 1: // processing 'then'
        case 2: // processing 'else'
        case -1: // skiping 'then'
        case -2: // skiping 'else'
        case -3: // skiping whole, 'then'
        case -4: // skiping whole, 'else'
          mIfStack.pop();
          break;
        default: die("unexpected 'endif'");
      }
      continue;
    }
    if (str == "{") {
      mIfStack.push(prc>=0 ? 666 : -666);
      if (prc >= 0) return;
      continue;
    }
    if (str == "}") {
      if (abs(prc) != 666) die("unexpected '}'");
      mIfStack.pop();
      if (prc >= 0) return;
      continue;
    }
    if (prc >= 0) return;
  }
}


festring inputfile::ReadWord (truth AbortOnEOF) {
  festring ToReturn;
  ReadWord(ToReturn, AbortOnEOF);
  return ToReturn;
}


void inputfile::SkipSpaces () {
  while (!feof(Buffer)) {
    int ch = fgetc(Buffer);
    if (ch == EOF) break;
    if (ch&0xff > ' ') {
      ungetc(ch, Buffer);
      break;
    }
  }
}


#define MODE_WORD 1
#define MODE_NUMBER 2

#define PUNCT_RETURN 0
#define PUNCT_CONTINUE 1

int inputfile::HandlePunct (festring &String, int Char, int Mode) {
  if (Char == '/') {
    // comment? (can be nested)
    if (!feof(Buffer)) {
      Char = fgetc(Buffer);
      if (Char == '*') {
        long StartPos = TellPos();
        int OldChar = 0, CommentLevel = 1;
        for (;;) {
          if (feof(Buffer)) ABORT("Unterminated comment in file %s, beginning at line %ld!", FileName.CStr(), TellLineOfPos(StartPos));
          Char = fgetc(Buffer);
          if (OldChar != '*' || Char != '/') {
            if (OldChar != '/' || Char != '*') OldChar = Char;
            else {
              ++CommentLevel;
              OldChar = 0;
            }
          } else {
            if (!--CommentLevel) break;
            OldChar = 0;
          }
        }
        return PUNCT_CONTINUE;
      }
      if (Char == '/') {
        // comment (to eol)
        while (!feof(Buffer)) {
          int ch = fgetc(Buffer);
          if (ch == '\n') break;
        }
        return PUNCT_CONTINUE;
      }
      ungetc(Char, Buffer);
      clearerr(Buffer);
    }
    if (Mode) ungetc('/', Buffer); else String << '/';
    return PUNCT_RETURN;
  }
  //
  if (Mode) {
    ungetc(Char, Buffer);
    return PUNCT_RETURN;
  }
  //
  if (Char == '"') {
    // string
    lastWordWasString = true;
    long StartPos = TellPos();
    int OldChar = 0;
    for (;;) {
      if (feof(Buffer)) ABORT("Unterminated string in file %s, beginning at line %ld!", FileName.CStr(), TellLineOfPos(StartPos));
      Char = fgetc(Buffer);
      if (Char == '\\') {
        Char = fgetc(Buffer);
        if (Char == EOF) ABORT("Unterminated string in file %s, beginning at line %ld!", FileName.CStr(), TellLineOfPos(StartPos));
        switch (Char) {
          case 't': String << '\t'; break;
          case 'n': String << '\n'; break;
          case 'r': String << '\r'; break;
          case '"': String << '"'; break;
          default:
            ABORT("Invalid escape in string in file %s at line %ld!", FileName.CStr(), TellLine());
        }
      } else if (Char == '"') {
        return PUNCT_RETURN;
      } else {
        String << char(Char);
      }
      /*
      if (Char != '"') {
        String << char(Char);
        OldChar = Char;
      } else if (OldChar == '\\') {
        String[String.GetSize()-1] = '"';
        OldChar = 0;
      } else return PUNCT_RETURN;
      */
    }
  }
  String << char(Char);
  if (!feof(Buffer)) {
    if (Char == '=' || Char == '<' || Char == '>' || Char == '!') {
      Char = fgetc(Buffer);
      if (Char == '=') String << char(Char); else ungetc(Char, Buffer);
    } else if (Char == '&' || Char == '|') {
      int ch = fgetc(Buffer);
      if (Char == ch) String << char(ch); else ungetc(ch, Buffer);
    }
  }
  return PUNCT_RETURN;
}


void inputfile::readWordIntr (festring &String, truth AbortOnEOF) {
  int Mode = 0;
  String.Empty();
  lastWordWasString = false;
  for (int Char = fgetc(Buffer); !feof(Buffer); Char = fgetc(Buffer)) {
    if (isalpha(Char) || Char == '_') {
      if (!Mode) Mode = MODE_WORD;
      else if (Mode == MODE_NUMBER) {
        ungetc(Char, Buffer);
        return;
      }
      String << char(Char);
      continue;
    }
    if (isdigit(Char)) {
      if (!Mode) Mode = MODE_NUMBER;
      else if (Mode == MODE_WORD) {
        ungetc(Char, Buffer);
        return;
      }
      String << char(Char);
      continue;
    }
    if ((Char == ' ' || Char == '\n' || Char == '\r' || Char == '\t') && Mode) return;
    if (ispunct(Char) && HandlePunct(String, Char, Mode) == PUNCT_RETURN) return;
  }
  if (AbortOnEOF) ABORT("Unexpected end of file %s!", FileName.CStr());
  if (Mode) clearerr(Buffer);
}


char inputfile::ReadLetter (truth AbortOnEOF) {
  for (int Char = fgetc(Buffer); !feof(Buffer); Char = fgetc(Buffer)) {
    if (isalpha(Char) || isdigit(Char)) return Char;
    if (ispunct(Char)) {
      if (Char == '/') {
        if (!feof(Buffer)) {
          Char = fgetc(Buffer);
          if (Char == '*') {
            long StartPos = TellPos();
            int OldChar = 0, CommentLevel = 1;
            for (;;) {
              if (feof(Buffer)) ABORT("Unterminated comment in file %s, beginning at line %ld!", FileName.CStr(), TellLineOfPos(StartPos));
              Char = fgetc(Buffer);
              if (OldChar != '*' || Char != '/') {
                if (OldChar != '/' || Char != '*') OldChar = Char;
                else {
                  ++CommentLevel;
                  OldChar = 0;
                }
              } else {
                if (!--CommentLevel) break;
                OldChar = 0;
              }
            }
            continue;
          } else {
            ungetc(Char, Buffer);
          }
        }
        return '/';
      }
      return Char;
    }
  }
  if (AbortOnEOF) ABORT("Unexpected end of file %s!", FileName.CStr());
  return 0;
}


/* Reads a number or a formula from inputfile. Valid values could be for
   instance "3", "5 * 4+5", "2+Variable%4" etc. */
//long inputfile::ReadNumber (int CallLevel, truth PreserveTerminator) {
festring inputfile::ReadNumberIntr (int CallLevel, long *num, truth *isString, truth allowStr, truth PreserveTerminator) {
  long Value = 0;
  festring Word;
  truth NumberCorrect = false;
  truth firstWord = true;
  *isString = false;
  *num = 0;
  festring res;
  for (;;) {
    ReadWord(Word);
    if (Word == "@") {
      // variable
      ReadWord(Word, true);
      //fprintf(stderr, "var: [%s]\n", Word.CStr());
      Word = getVar(Word);
      //fprintf(stderr, " value: [%s]\n", Word.CStr());
      const char *s = Word.CStr();
      char *e;
      long l = strtoll(s, &e, 10);
      if (*e == '\0') {
        //fprintf(stderr, " number: [%ld]\n", l);
        Value = l;
        NumberCorrect = true;
        continue;
      }
      if (firstWord && allowStr) {
        *isString = true;
        return Word;
      } else {
        ABORT("Number expected in file %s, line %ld!", FileName.CStr(), TellLine());
      }
    }
    if (firstWord) {
      if (allowStr && lastWordWasString) {
        *isString = true;
        ReadWord(res);
        if (res.GetSize() == 1) {
          if (res[0] != ';' && res[0] != ',' && res[0] != ':') {
            ABORT("Invalid terminator in file %s, line %ld!", FileName.CStr(), TellLine());
          }
          if (PreserveTerminator) ungetc(res[0], Buffer);
        } else {
          ABORT("Terminator expected in file %s, line %ld!", FileName.CStr(), TellLine());
        }
        return Word;
      }
      firstWord = false;
    }
    char First = Word[0];
    if (isdigit(First)) {
      Value = atoi(Word.CStr());
      NumberCorrect = true;
      continue;
    }
    if (Word.GetSize() == 1) {
      if (First == ';' || First == ',' || First == ':') {
        if (CallLevel != HIGHEST || PreserveTerminator) ungetc(First, Buffer);
        *num = Value;
        return res;
      }
      if (First == ')') {
        if ((CallLevel != HIGHEST && CallLevel != 4) || PreserveTerminator) ungetc(')', Buffer);
        *num = Value;
        return res;
      }
      if (First == '~') {
        Value = ~ReadNumber(4);
        NumberCorrect = true;
        continue;
      }
/* Convert this into an inline function! */
#define CHECK_OP(op, cl) \
  if (First == #op[0]) { \
    if (cl < CallLevel) {\
      Value op##= ReadNumber(cl);\
      NumberCorrect = true;\
      continue;\
    } else {\
      ungetc(#op[0], Buffer);\
      *num = Value;\
      return res;\
    } \
  }
      CHECK_OP(&, 1); CHECK_OP(|, 1); CHECK_OP(^, 1);
      CHECK_OP(*, 2); CHECK_OP(/, 2); CHECK_OP(%, 2);
      CHECK_OP(+, 3); CHECK_OP(-, 3);
      if (First == '<') {
        char Next = Get();
        if (Next == '<')
        if (1 < CallLevel) {
          Value <<= ReadNumber(1);
          NumberCorrect = true;
          continue;
        } else {
          ungetc('<', Buffer);
          ungetc('<', Buffer);
          *num = Value;
          return res;
        } else {
          ungetc(Next, Buffer);
        }
      }
      if (First == '>') {
        char Next = Get();
        if (Next == '>')
        if (1 < CallLevel) {
          Value >>= ReadNumber(1);
          NumberCorrect = true;
          continue;
        } else {
          ungetc('>', Buffer);
          ungetc('>', Buffer);
          *num = Value;
          return res;
        } else {
          ungetc(Next, Buffer);
        }
      }
      if (First == '(') {
        if (NumberCorrect) {
          ungetc('(', Buffer);
          *num = Value;
          return res;
        } else {
          Value = ReadNumber(4);
          NumberCorrect = false;
          continue;
        }
      }
      if (First == '=' && CallLevel == HIGHEST) continue;
      if (First == '#') {
        // for #defines
        ungetc('#', Buffer);
        *num = Value;
        return res;
      }
    }
    /*
    if (Word == "enum" || Word == "bitenum") {
      if (CallLevel != HIGHEST || PreserveTerminator) ungetc(';', Buffer);
      *num = Value;
      return res;
    }
    */
    if (Word == "rgb") {
      int Bits = ReadNumber();
      if (Bits == 16) {
        int Red = ReadNumber();
        int Green = ReadNumber();
        int Blue = ReadNumber();
        Value = MakeRGB16(Red, Green, Blue);
      } else if (Bits == 24) {
        int Red = ReadNumber();
        int Green = ReadNumber();
        int Blue = ReadNumber();
        Value = MakeRGB24(Red, Green, Blue);
      } else {
        ABORT("Illegal RGB bit size %d in file %s, line %ld!", Bits, FileName.CStr(), TellLine());
      }
      NumberCorrect = true;
      continue;
    }
    if (Word == "true" || Word == "tan") {
      Value = 1;
      NumberCorrect = true;
      continue;
    }
    if (Word == "false" || Word == "ona") {
      Value = 0;
      NumberCorrect = true;
      continue;
    }
    if (ValueMap) {
      valuemap::const_iterator Iterator = ValueMap->find(Word);
      if (Iterator != ValueMap->end()) {
        Value = Iterator->second;
        NumberCorrect = true;
        continue;
      }
    }
    ABORT("Odd numeric value \"%s\" encountered in file %s, line %ld!",
    Word.CStr(), FileName.CStr(), TellLine());
  }
}


long inputfile::ReadNumber (int CallLevel, truth PreserveTerminator) {
  long num = 0;
  truth isString = false;
  ReadNumberIntr(CallLevel, &num, &isString, false, PreserveTerminator);
  return num;
}


festring inputfile::ReadStringOrNumber (long *num, truth *isString, truth PreserveTerminator) {
  return ReadNumberIntr(0xFF, num, isString, true, PreserveTerminator);
}


v2 inputfile::ReadVector2d () {
  v2 Vector;
  Vector.X = ReadNumber();
  Vector.Y = ReadNumber();
  return Vector;
}


rect inputfile::ReadRect () {
  rect Rect;
  Rect.X1 = ReadNumber();
  Rect.Y1 = ReadNumber();
  Rect.X2 = ReadNumber();
  Rect.Y2 = ReadNumber();
  return Rect;
}


outputfile &operator << (outputfile &SaveFile, cfestring &String) {
  ushort Length = String.GetSize();
  SaveFile << Length;
  if (Length) SaveFile.Write(String.CStr(), Length);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, festring &String) {
  char *RealBuffer, StackBuffer[1024];
  ushort Length;
  SaveFile >> Length;
  RealBuffer = Length < 1024 ? StackBuffer : new char[Length+1];
  String.Empty();
  if (Length) {
    SaveFile.Read(RealBuffer, Length);
    RealBuffer[Length] = 0;
    String << RealBuffer;
  }
  if (Length >= 1024) delete [] RealBuffer;
  return SaveFile;
}


outputfile &operator << (outputfile &SaveFile, cchar *String) {
  ushort Length = String ? strlen(String) : 0;
  SaveFile << Length;
  if (Length) SaveFile.Write(String, Length);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, char *&String) {
  ushort Length;
  SaveFile >> Length;
  if (Length) {
    String = new char[Length+1];
    SaveFile.Read(String, Length);
    String[Length] = 0;
  } else {
    String = 0;
  }
  return SaveFile;
}


void ReadData (festring &String, inputfile &SaveFile) {
  SaveFile.ReadWord(String);
  if (String == "=") SaveFile.ReadWord(String);
  SaveFile.ReadWord();
}


void ReadData (fearray<long> &Array, inputfile &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  //if (Word == "=") SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    Array.Data[0] = SaveFile.ReadNumber();
    return;
  }
  if (Word != "=") ABORT("Array syntax error: '=' or '==' expected in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  SaveFile.ReadWord(Word);
  if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  fearray<long>::sizetype Size = SaveFile.ReadNumber();
  Array.Allocate(Size);
  for (fearray<long>::sizetype c = 0; c < Size; ++c) Array.Data[c] = SaveFile.ReadNumber();
  if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
}


void ReadData (fearray<festring> &Array, inputfile &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  //if (Word == "=") SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    SaveFile.ReadWord(Array.Data[0]);
    if (SaveFile.ReadWord() != ";") ABORT("Array syntax error \"%s\" found in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
    return;
  }
  if (Word != "=") ABORT("Array syntax error: '=' or '==' expected in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  SaveFile.ReadWord(Word);
  if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  fearray<festring>::sizetype Size = SaveFile.ReadNumber();
  Array.Allocate(Size);
  for (fearray<festring>::sizetype c = 0; c < Size; ++c) {
    SaveFile.ReadWord(Array.Data[c]);
    SaveFile.ReadWord(Word);
    if (Word != "," && Word != ";") ABORT("Array syntax error \"%s\" found in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  }
  if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %ld!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
}


ulong inputfile::TellLineOfPos (long Pos) {
  ulong Line = 1;
  long BackupPos = TellPos();
  SeekPosBegin(0);
  while (TellPos() != Pos) { if (fgetc(Buffer) == '\n') ++Line; }
  if (TellPos() != BackupPos) SeekPosBegin(BackupPos);
  return Line;
}


meminputfile::meminputfile (cfestring &str, const valuemap *ValueMap) :
  inputfile("", ValueMap, false)
{
  if (Buffer) fclose(Buffer);
  bufSize = str.GetSize();
  buf = malloc(bufSize+1);
  memcpy(buf, str.CStr(), bufSize);
  Buffer = fmemopen(buf, bufSize, "rb");
  FileName = "<memory>";
}
