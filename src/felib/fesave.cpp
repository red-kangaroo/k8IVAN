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

#ifdef WIN32
# include <windows.h>
#endif

#include "fesave.h"
#include "femath.h"


#ifdef USE_ZLIB
# define Xgetc  gzgetc
# define Xfeof  gzeof
# define Xungc  gzungetc
# define Xseek  gzseek
# define Xtell  gztell
# define Xclre  gzclearerr
#else
# define Xgetc  fgetc
# define Xfeof  feof
# define Xungc  ungetc
# define Xseek  fseek
# define Xtell  ftell
# define Xclre  clearerr
#endif


////////////////////////////////////////////////////////////////////////////////
outputfile::outputfile (cfestring &FileName, truth AbortOnErr) :
#ifdef USE_ZLIB
  Buffer(gzopen(FileName.CStr(), "wb9")),
#else
  Buffer(fopen(FileName.CStr(), "wb")),
#endif
  FileName(FileName)
{
  if (AbortOnErr && !IsOpen()) ABORT("Can't open %s for output!", FileName.CStr());
}


outputfile::~outputfile () {
  Close();
}


void outputfile::Close () {
  if (Buffer) {
#ifdef USE_ZLIB
    gzclose(Buffer);
#else
    fclose(Buffer);
#endif
    Buffer = 0;
  }
}


void outputfile::Flush () {
#ifdef USE_ZLIB
  gzflush(Buffer, Z_FINISH);
#else
  fflush(Buffer);
#endif
}


void outputfile::Put (char What) {
#ifdef USE_ZLIB
  gzputc(Buffer, What);
#else
  fputc(What, Buffer);
#endif
}


void outputfile::Write (cchar *Offset, sLong Size) {
#ifdef USE_ZLIB
  gzwrite(Buffer, Offset, Size);
#else
  fwrite(Offset, 1, Size, Buffer);
#endif
}


////////////////////////////////////////////////////////////////////////////////
truth inputfile::fileExists (const festring &fname) {
#ifndef WIN32
  struct stat st;
  if (stat(fname.CStr(), &st)) return false;
  if (!S_ISREG(st.st_mode)) return false;
  return access(fname.CStr(), R_OK) == 0;
#else
  FILE *fl = fopen(fname.CStr(), "rb");
  if (fl) fclose(fl);
  return fl != 0;
#endif
}


festring inputfile::GetMyDir (void) {
  char myDir[8192];
#ifndef WIN32
  char buf[128];
  pid_t mypid = getpid();
  memset(myDir, 0, sizeof(myDir));
  sprintf(buf, "/proc/%u/exe", (unsigned int)mypid);
  if (readlink(buf, myDir, sizeof(myDir)-1) < 0) strcpy(myDir, ".");
  else {
    char *p = (char *)strrchr(myDir, '/');
    if (!p) strcpy(myDir, "."); else *p = '\0';
  }
  if (myDir[strlen(myDir)-1] == '/') myDir[strlen(myDir)-1] = '\0';
#else
  char *p;
  memset(myDir, 0, sizeof(myDir));
  GetModuleFileName(GetModuleHandle(NULL), myDir, sizeof(myDir)-1);
  p = strrchr(myDir, '\\');
  if (!p) strcpy(myDir, "."); else *p = '\0';
#endif
  return myDir;
}


inputfile::inputfile (cfestring &FileName, const valuemap *ValueMap, truth AbortOnErr) :
#ifdef USE_ZLIB
  Buffer(gzopen(FileName.CStr(), "rb")),
#else
  Buffer(fopen(FileName.CStr(), "rb")),
#endif
  FileName(FileName),
  ValueMap(ValueMap),
  lastWordWasString(false)
#ifdef USE_ZLIB
  , mFileSize(-1)
#endif
{
  if (AbortOnErr && !IsOpen()) ABORT("File %s not found!", FileName.CStr());
}


inputfile::~inputfile () {
  Close();
}


void inputfile::Close () {
  if (Buffer) {
#ifdef USE_ZLIB
    gzclose(Buffer);
#else
    fclose(Buffer);
#endif
    Buffer = 0;
  }
}


int inputfile::Get () {
  return Xgetc(Buffer);
}


int inputfile::Unget (int ch) {
  return Xungc(ch, Buffer);
}


void inputfile::Read (char *Offset, sLong Size) {
#ifdef USE_ZLIB
  if (gzread(Buffer, Offset, Size) != Size) ABORT("File '%s' read error!", FileName.CStr());
#else
  if (fread(Offset, Size, 1, Buffer) != 1) ABORT("File '%s' read error!", FileName.CStr());
#endif
}


truth inputfile::Eof () {
  return Xfeof(Buffer);
}


void inputfile::SeekPosBegin (sLong Offset) {
  if (Xseek(Buffer, Offset, SEEK_SET) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}


void inputfile::SeekPosCurrent (sLong Offset) {
  if (Xseek(Buffer, Offset, SEEK_CUR) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}


#ifdef USE_ZLIB
void inputfile::SeekPosEnd (sLong Offset) {
  //HACKHACK: emulate this
  if (mFileSize < 0) {
    //SLOOOW, but we have to do that
    int opos;
    char *buffer, buf[512];
    int bufsize;
    //
    for (bufsize = 256*1024; bufsize > (int)sizeof(buf); bufsize /= 2) {
      if ((buffer = (char *)malloc(bufsize)) != NULL) break;
    }
    if (buffer == NULL) { buffer = buf; bufsize = sizeof(buf); }
    //
    //fprintf(stderr, "determining file size...\n");
    mFileSize = opos = gztell(Buffer);
    for (;;) {
      int len = gzread(Buffer, buffer, bufsize);
      //
      if (len < 0) { mFileSize = -1; break; } // error
      mFileSize += len;
      if (len < bufsize) break; // eof reached
    }
    if (buffer != buf) free(buffer);
    //fprintf(stderr, "file size: %d\n", ctx->filesize);
  }
  //
  if (mFileSize < 0) ABORT("File '%s': seek error!", FileName.CStr());
  //
  if (gzseek(Buffer, mFileSize+Offset, SEEK_SET) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}

#else

void inputfile::SeekPosEnd (sLong Offset) {
  if (fseek(Buffer, Offset, SEEK_END) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}
#endif


sLong inputfile::TellPos () {
  return Xtell(Buffer);
}


void inputfile::ClearFlags () {
  Xclre(Buffer);
}


festring inputfile::findVar (cfestring &name, truth *found) const {
  VarMap::const_iterator i = mVars.find(name);
  if (i != mVars.end()) {
    if (found) *found = true;
    return i->second;
  }
  if (found) *found = false;
  return "";
}


festring inputfile::getVar (cfestring &name) {
  truth found;
  festring res = findVar(name, &found);
  //
  if (!found) {
    if (mGetVar) {
      res = mGetVar(this, name);
    } else {
      festring s = "unknown variable: "+name;
      die(s);
    }
  }
  return res;
}


void inputfile::setVar (cfestring &name, cfestring &value) {
  mVars[name] = value;
}


//TODO: invoke callback
truth inputfile::delVar (cfestring &name) {
  VarMap::iterator i = mVars.find(name);
  if (i != mVars.end()) {
    mVars.erase(i);
    return true;
  }
  return false;
}


void inputfile::die (cfestring &msg) {
  ABORT("ERROR in file %s, line %d: %s", GetFileName().CStr(), TellLine(), msg.CStr());
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
      } else {
        res = token;
      }
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
  while (!Xfeof(Buffer)) {
    int ch = Xgetc(Buffer);
    if (ch == EOF) break;
    if ((unsigned char)ch > ' ') {
      Xungc(ch, Buffer);
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
    if (!Xfeof(Buffer)) {
      Char = Xgetc(Buffer);
      if (Char == '*') {
        sLong StartPos = TellPos();
        int OldChar = 0, CommentLevel = 1;
        for (;;) {
          if (Xfeof(Buffer)) ABORT("Unterminated comment in file %s, beginning at line %d!", FileName.CStr(), TellLineOfPos(StartPos));
          Char = Xgetc(Buffer);
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
        while (!Xfeof(Buffer)) {
          int ch = Xgetc(Buffer);
          if (ch == '\n') break;
        }
        return PUNCT_CONTINUE;
      }
      Xungc(Char, Buffer);
      Xclre(Buffer);
    }
    if (Mode) Xungc('/', Buffer); else String << '/';
    return PUNCT_RETURN;
  }
  //
  if (Mode) {
    Xungc(Char, Buffer);
    return PUNCT_RETURN;
  }
  //
  if (Char == '"') {
    // string
    lastWordWasString = true;
    sLong StartPos = TellPos();
    for (;;) {
      if (Xfeof(Buffer)) ABORT("Unterminated string in file %s, beginning at line %d!", FileName.CStr(), TellLineOfPos(StartPos));
      Char = Xgetc(Buffer);
      if (Char == '\\') {
        Char = Xgetc(Buffer);
        if (Char == EOF) ABORT("Unterminated string in file %s, beginning at line %d!", FileName.CStr(), TellLineOfPos(StartPos));
        switch (Char) {
          case 't': String << '\t'; break;
          case 'n': String << '\n'; break;
          case 'r': String << '\r'; break;
          case '"': String << '"'; break;
          default:
            ABORT("Invalid escape in string in file %s at line %d!", FileName.CStr(), TellLine());
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
  if (!Xfeof(Buffer)) {
    if (Char == '=' || Char == '<' || Char == '>' || Char == '!') {
      Char = Xgetc(Buffer);
      if (Char == '=') String << char(Char); else Xungc(Char, Buffer);
    } else if (Char == '&' || Char == '|') {
      int ch = Xgetc(Buffer);
      if (Char == ch) String << char(ch); else Xungc(ch, Buffer);
    }
  }
  return PUNCT_RETURN;
}


void inputfile::readWordIntr (festring &String, truth AbortOnEOF) {
  int Mode = 0;
  String.Empty();
  lastWordWasString = false;
  for (int Char = Xgetc(Buffer); !Xfeof(Buffer); Char = Xgetc(Buffer)) {
    if (isalpha(Char) || Char == '_') {
      if (!Mode) Mode = MODE_WORD;
      else if (Mode == MODE_NUMBER) {
        Xungc(Char, Buffer);
        return;
      }
      String << char(Char);
      continue;
    }
    if (isdigit(Char)) {
      if (!Mode) Mode = MODE_NUMBER;
      else if (Mode == MODE_WORD) {
        Xungc(Char, Buffer);
        return;
      }
      String << char(Char);
      continue;
    }
    if ((Char == ' ' || Char == '\n' || Char == '\r' || Char == '\t') && Mode) return;
    if (ispunct(Char) && HandlePunct(String, Char, Mode) == PUNCT_RETURN) return;
  }
  if (AbortOnEOF) ABORT("Unexpected end of file %s!", FileName.CStr());
  if (Mode) Xclre(Buffer);
}


char inputfile::ReadLetter (truth AbortOnEOF) {
  for (int Char = Xgetc(Buffer); !Xfeof(Buffer); Char = Xgetc(Buffer)) {
    if (isalpha(Char) || isdigit(Char)) return Char;
    if (ispunct(Char)) {
      if (Char == '/') {
        if (!Xfeof(Buffer)) {
          Char = Xgetc(Buffer);
          if (Char == '*') {
            sLong StartPos = TellPos();
            int OldChar = 0, CommentLevel = 1;
            for (;;) {
              if (Xfeof(Buffer)) ABORT("Unterminated comment in file %s, beginning at line %d!", FileName.CStr(), TellLineOfPos(StartPos));
              Char = Xgetc(Buffer);
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
            Xungc(Char, Buffer);
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
//sLong inputfile::ReadNumber (int CallLevel, truth PreserveTerminator) {
festring inputfile::ReadNumberIntr (int CallLevel, sLong *num, truth *isString, truth allowStr, truth PreserveTerminator) {
  sLong Value = 0;
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
      sLong l = strtoll(s, &e, 10);
      if (*e == '\0') {
        //fprintf(stderr, " number: [%d]\n", l);
        Value = l;
        NumberCorrect = true;
        continue;
      }
      if (firstWord && allowStr) {
        *isString = true;
        return Word;
      } else {
        ABORT("Number expected in file %s, line %d!", FileName.CStr(), TellLine());
      }
    }
    if (firstWord) {
      if (allowStr && lastWordWasString) {
        *isString = true;
        ReadWord(res);
        if (res.GetSize() == 1) {
          if (res[0] != ';' && res[0] != ',' && res[0] != ':') {
            ABORT("Invalid terminator in file %s, line %d!", FileName.CStr(), TellLine());
          }
          if (PreserveTerminator) Xungc(res[0], Buffer);
        } else {
          ABORT("Terminator expected in file %s, line %d!", FileName.CStr(), TellLine());
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
        if (CallLevel != HIGHEST || PreserveTerminator) Xungc(First, Buffer);
        *num = Value;
        return res;
      }
      if (First == ')') {
        if ((CallLevel != HIGHEST && CallLevel != 4) || PreserveTerminator) Xungc(')', Buffer);
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
      Xungc(#op[0], Buffer);\
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
          Xungc('<', Buffer);
          Xungc('<', Buffer);
          *num = Value;
          return res;
        } else {
          Xungc(Next, Buffer);
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
          Xungc('>', Buffer);
          Xungc('>', Buffer);
          *num = Value;
          return res;
        } else {
          Xungc(Next, Buffer);
        }
      }
      if (First == '(') {
        if (NumberCorrect) {
          Xungc('(', Buffer);
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
        Xungc('#', Buffer);
        *num = Value;
        return res;
      }
    }
    /*
    if (Word == "enum" || Word == "bitenum") {
      if (CallLevel != HIGHEST || PreserveTerminator) Xungc(';', Buffer);
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
        ABORT("Illegal RGB bit size %d in file %s, line %d!", Bits, FileName.CStr(), TellLine());
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
    ABORT("Odd numeric value \"%s\" encountered in file %s, line %d!",
    Word.CStr(), FileName.CStr(), TellLine());
  }
}


festring inputfile::ReadCode (truth AbortOnEOF) {
  int sqLevel = 1;
  char inString = 0;
  festring res;
  //
  for (char Char = Xgetc(Buffer); !Xfeof(Buffer); Char = Xgetc(Buffer)) {
    //fprintf(stderr, "char: [%c]; inString: %d; sqLevel: %d\n", (Char < 32 || Char > 126 ? '?' : Char), inString, sqLevel);
    if (inString) {
      res << Char;
      if (Char == inString) {
        inString = 0;
      } else if (Char == '\\') {
        if (Xfeof(Buffer)) break;
        Char = Xgetc(Buffer);
        res << Char;
      }
    } else {
      if (Char == '[') {
        ++sqLevel;
        res << Char;
      } else if (Char == ']') {
        if (--sqLevel == 0) break;
        res << Char;
      } else if (Char == '/') {
        if (Xfeof(Buffer)) { res << Char; break; }
        switch ((Char = Xgetc(Buffer))) {
          case '/': // eol comment
            while (!Xfeof(Buffer)) if (Xgetc(Buffer) == '\n') break;
            break;
          case '*': // c-like comment
            while (!Xfeof(Buffer)) {
              if (Xgetc(Buffer) == '*') {
                if (Xfeof(Buffer)) break;
                if (Xgetc(Buffer) == '/') break;
              }
            }
            break;
          default:
            res << '/';
            res << Char;
            break;
        }
      } else if (Char == '"' || Char == '\'') {
        res << Char;
        inString = Char;
      } else {
        res << Char;
      }
    }
  }
  if (AbortOnEOF && Xfeof(Buffer)) ABORT("Unexpected end of file %s!", FileName.CStr());
  return res;
}


sLong inputfile::ReadNumber (int CallLevel, truth PreserveTerminator) {
  sLong num = 0;
  truth isString = false;
  ReadNumberIntr(CallLevel, &num, &isString, false, PreserveTerminator);
  return num;
}


festring inputfile::ReadStringOrNumber (sLong *num, truth *isString, truth PreserveTerminator) {
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
  uShort Length = String.GetSize();
  SaveFile << Length;
  if (Length) SaveFile.Write(String.CStr(), Length);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, festring &String) {
  char *RealBuffer, StackBuffer[1024];
  uShort Length;
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
  uShort Length = String ? strlen(String) : 0;
  SaveFile << Length;
  if (Length) SaveFile.Write(String, Length);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, char *&String) {
  uShort Length;
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


void ReadData (fearray<sLong> &Array, inputfile &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  //if (Word == "=") SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    Array.Data[0] = SaveFile.ReadNumber();
    return;
  }
  if (Word != "=") ABORT("Array syntax error: '=' or '==' expected in file %s, line %d!", SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  SaveFile.ReadWord(Word);
  if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  fearray<sLong>::sizetype Size = SaveFile.ReadNumber();
  Array.Allocate(Size);
  for (fearray<sLong>::sizetype c = 0; c < Size; ++c) Array.Data[c] = SaveFile.ReadNumber();
  if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
}


void ReadData (fearray<festring> &Array, inputfile &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  //if (Word == "=") SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    SaveFile.ReadWord(Array.Data[0]);
    if (SaveFile.ReadWord() != ";") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
    return;
  }
  if (Word != "=") ABORT("Array syntax error: '=' or '==' expected in file %s, line %d!", SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  SaveFile.ReadWord(Word);
  if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  fearray<festring>::sizetype Size = SaveFile.ReadNumber();
  Array.Allocate(Size);
  for (fearray<festring>::sizetype c = 0; c < Size; ++c) {
    SaveFile.ReadWord(Array.Data[c]);
    SaveFile.ReadWord(Word);
    if (Word != "," && Word != ";") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
  }
  if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TellLine());
}


feuLong inputfile::TellLineOfPos (sLong Pos) {
  feuLong Line = 1;
  sLong BackupPos = TellPos();
  SeekPosBegin(0);
  while (TellPos() != Pos) { if (Xgetc(Buffer) == '\n') ++Line; }
  if (TellPos() != BackupPos) SeekPosBegin(BackupPos);
  return Line;
}


////////////////////////////////////////////////////////////////////////////////
#ifdef USE_ZLIB
meminputfile::meminputfile (cfestring &str, const valuemap *ValueMap) :
  inputfile("", ValueMap, false)
{
  Close();
#ifdef WIN32
  char nbuf[MAX_PATH+1], tfn[MAX_PATH+1];
  GetTempPath(MAX_PATH, nbuf);
  GetTempFileName(nbuf, "ivan", 0, tfn);
  tfname = tfn;
  FILE *fl = fopen(tfn, "wb");
  fwrite(str.CStr(), str.GetSize(), 1, fl);
  fclose(fl);
  Buffer = gzopen(tfn, "rb");
#else
  char fname[1024];
  int fd;
  //
  strcpy(fname, "/tmp/i.v.a.n.XXXXXX");
  tfname = fname;
  fd = mkstemp(fname);
  if (fd < 0) ABORT("Can't create temporary file!");
  write(fd, str.CStr(), str.GetSize());
  close(fd);
  Buffer = gzopen(fname, "rb");
#endif
  FileName = "<memory>";
}


meminputfile::~meminputfile () {
  if (buf) free(buf);
  Close();
  unlink(tfname.CStr());
}


#else


meminputfile::meminputfile (cfestring &str, const valuemap *ValueMap) :
  inputfile("", ValueMap, false)
{
  Close();
#ifdef WIN32
  char nbuf[MAX_PATH+1], tfn[MAX_PATH+1];
  GetTempPath(MAX_PATH, nbuf);
  GetTempFileName(nbuf, "ivan", 0, tfn);
  tfname = tfn;
  FILE *fl = fopen(tfn, "wb");
  fwrite(str.CStr(), str.GetSize(), 1, fl);
  fclose(fl);
  Buffer = fopen(tfn, "rb");
#else
  bufSize = str.GetSize();
  buf = malloc(bufSize+1);
  memmove(buf, str.CStr(), bufSize);
  Buffer = fmemopen(buf, bufSize, "rb");
#endif
  FileName = "<memory>";
}


meminputfile::~meminputfile () {
  if (buf) free(buf);
  Close();
#ifdef WIN32
  DeleteFile(tfname.CStr());
#endif
}
#endif
