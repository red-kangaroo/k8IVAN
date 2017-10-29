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

#include "feparse.h"
#include "femath.h"


////////////////////////////////////////////////////////////////////////////////
struct InputFileSaved {
  friend TextInput;

private:
  TextInput *ifile;
  void *svbuf;
  int mCharBuf[4];
  int mCharBufPos;
  int mCurrentLine;
  int mTokenLine;
  sLong mRealPos;

private:
  InputFileSaved (TextInput *aifile) : ifile(aifile), svbuf(nullptr) {
    if (aifile) {
      memcpy(mCharBuf, aifile->mCharBuf, sizeof(mCharBuf));
      mCharBufPos = aifile->mCharBufPos;
      mCurrentLine = aifile->mCurrentLine;
      mTokenLine = aifile->mTokenLine;
      mRealPos = aifile->realGetPos();
    }
  }

public:
  ~InputFileSaved () {
    if (ifile) {
      memcpy(ifile->mCharBuf, mCharBuf, sizeof(ifile->mCharBuf));
      ifile->mCharBufPos = mCharBufPos;
      ifile->mCurrentLine = mCurrentLine;
      ifile->mTokenLine = mTokenLine;
      ifile->realSetPos(mRealPos);
    }
  }
};


////////////////////////////////////////////////////////////////////////////////
TextInput::TextInput (const valuemap *aValueMap) {
  setup(aValueMap);
}


TextInput::~TextInput () {
  //fprintf(stderr, "TI:~this();\n");
  Close();
  //fprintf(stderr, "TI:~this(); -- exit\n");
}


void TextInput::setup (const valuemap *aValueMap) {
  ValueMap = aValueMap;
  lastWasNL = false;
  lastWordWasString = false;
  mCharBufPos = 0;
  mCurrentLine = 1;
  mTokenLine = 1;
  mNumStr = "";
  mCollectingNumStr = false;
}


void TextInput::Close () {
  //fprintf(stderr, "TI:Close();\n");
  lastWasNL = false;
  while (!mIfStack.empty()) mIfStack.pop();
  mCharBufPos = 0;
  mCurrentLine = 0;
  mTokenLine = 0;
  mNumStr = "";
  mCollectingNumStr = false;
  //fprintf(stderr, "TI:Close(); -- exit\n");
}


int TextInput::GetChar () {
  if (mCharBufPos > 0) {
    return mCharBuf[--mCharBufPos];
  } else {
    if (lastWasNL) { ++mCurrentLine; lastWasNL = false; }
    int ch = realGetChar();
    if (ch == 0) ch = ' ';
    lastWasNL = (ch == '\n');
    return (ch < 0 ? EOF : ch);
  }
}


void TextInput::UngetChar (int ch) {
  if (ch >= 0) {
    if (mCharBufPos > MaxUngetChars) die("too many unread chars");
    mCharBuf[mCharBufPos++] = ch;
  }
}


truth TextInput::Eof () {
  if (mCharBufPos > 0) return false;
  return isRealEof();
}


// just read `ch`, skip possible comment; returns `ch` or -1
int TextInput::gotCharSkipComment (int ch, truth allowSingleLineComments) {
  if (ch < 0) ABORT("The thing that should not be");
  if (ch != '/') return ch;
  ch = GetChar();
  if (ch == EOF) return '/';
  // single-line comment?
  if (allowSingleLineComments && ch == '/') {
    while (ch != EOF && ch != '\n') ch = GetChar();
    return -1;
  }
  // multiline comment? (possibly nested)
  if (ch != '*') { UngetChar(ch); return '/'; }
  int prevch = 0, level = 1;
  for (;;) {
    ch = GetChar();
    if (ch == EOF) ABORT("Unterminated comment in file %s, beginning at line %d!", GetFileName().CStr(), mTokenLine);
    // close comment
    if (prevch == '*' && ch == '/') {
      if (--level == 0) return -1;
      prevch = 0;
      continue;
    }
    // open comment
    if (prevch == '/' && ch == '*') {
      ++level;
      prevch = 0;
      continue;
    }
    // other chars
    prevch = ch;
  }
}


void TextInput::skipBlanks () {
  for (;;) {
    int ch = GetChar();
    if (ch == EOF) return;
    if (ch <= ' ') continue;
    ch = gotCharSkipComment(ch);
    if (ch < 0) continue;
    UngetChar(ch);
    return;
  }
}


#define StackDepth  (256)
int TextInput::countArrayItems (char echar) {
  auto savedPos = InputFileSaved(this);
  char stack[StackDepth]; // end chars
  int sp = 0;
  stack[0] = echar;
  skipBlanks();
  int ch = GetChar();
  if (ch == EOF) return -1; // oops
  if (ch == ',' || ch == ';') return -1;
  //fprintf(stderr, "COUNT: ch='%c'\n", ch);
  if (ch == echar) return 0;
  UngetChar(ch);
  int count = 1;
  while (sp >= 0) {
    skipBlanks();
    ch = GetChar();
    if (ch == EOF) return -1; // oops
    // string?
    if (ch == '"' || ch == '\'') {
      echar = ch;
      while (ch != EOF) {
        ch = GetChar();
        if (ch == '\\') {
          ch = GetChar();
          if (ch == EOF) return -1; // oops
        } else if (ch == echar) {
          break;
        }
      }
      continue;
    }
    if (sp == 0 && (ch == ',' || ch == ';')) {
      skipBlanks();
      ch = GetChar();
      if (ch == EOF) return -1;
      if (ch == ',' || ch == ';') return -1;
      //fprintf(stderr, " oldcount=%d; ch='%c'; sp=%d\n", count, ch, sp);
      if (sp == 0 && ch == stack[0]) {} else ++count;
      //fprintf(stderr, "  newcount=%d; ch='%c'; sp=%d\n", count, ch, sp);
      //if (ch != ')' && ch != ']' && ch != '}') ++count;
    }
    // endchar?
    if (ch == stack[sp]) {
      //fprintf(stderr, " *close; ch='%c'; sp=%d\n", ch, sp);
      --sp;
      continue;
    }
    // check for openings
    switch (ch) {
      case '(': echar = ')'; break;
      case '[': echar = ']'; break;
      case '{': echar = '}'; break;
      case ')': case ']': case '}': return -1; // oops
      default: echar = 0; break;
    }
    if (echar) {
      if (sp >= StackDepth-1) return -1; // oops
      //fprintf(stderr, " *open; ch='%c'; echar='%c'; sp=%d\n", ch, echar, sp);
      stack[++sp] = echar;
    }
  }
  return count;
}
#undef StackDepth


// ////////////////////////////////////////////////////////////////////////// //
festring TextInput::findVar (cfestring &name, truth *found) const {
  VarMap::const_iterator i = mVars.find(name);
  if (i != mVars.end()) {
    if (found) *found = true;
    return i->second;
  }
  if (found) *found = false;
  return "";
}


festring TextInput::getVar (cfestring &name) {
  truth found;
  festring res = findVar(name, &found);
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


void TextInput::setVar (cfestring &name, cfestring &value) {
  mVars[name] = value;
}


//TODO: invoke callback
truth TextInput::delVar (cfestring &name) {
  VarMap::iterator i = mVars.find(name);
  if (i != mVars.end()) {
    mVars.erase(i);
    return true;
  }
  return false;
}


// ////////////////////////////////////////////////////////////////////////// //
void TextInput::die (cfestring &msg) {
  ABORT("ERROR in file %s, line %d: %s", GetFileName().CStr(), mTokenLine, msg.CStr());
}


// ////////////////////////////////////////////////////////////////////////// //
// 0: term
// 1: unary
// 2: comparisons
// 3: &&
// 4: ||
static const int maxCPrio = 4;
static const char *opers[5][7] = {
  {NULL},
  {NULL},
  {"<", ">", "<=", ">=", "==", "!=", NULL},
  {"&&", NULL},
  {"||", NULL}
};

festring TextInput::readCondition (festring &token, int prio, truth skipIt) {
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
          if (!skipIt) res = (res == op1 ? "tan" : "");
        } else if (opc == "!=") {
          if (!skipIt) res = (res != op1 ? "tan" : "");
        } else if (opc == "<") {
          if (!skipIt) res = (res < op1 ? "tan" : "");
        } else if (opc == ">") {
          if (!skipIt) res = (res > op1 ? "tan" : "");
        } else if (opc == "<=") {
          if (!skipIt) res = (res <= op1 ? "tan" : "");
        } else if (opc == ">=") {
          if (!skipIt) res = (res >= op1 ? "tan" : "");
        }
        break;
      case 3: // &&
        if (opc == "&&") {
          if (!skipIt) {
            res = (res != "" && op1 != "" ? "tan" : "");
            if (res == "") skipIt = true;
          }
        }
        break;
      case 4: // ||
        if (opc == "||") {
          if (!skipIt) {
            res = (res != "" || op1 != "" ? "tan" : "");
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
truth TextInput::ReadWord (festring &str, truth abortOnEOF) {
  for (;;) {
    int prc = (mIfStack.empty() ? 0 : mIfStack.top());
    if (!readWordIntr(str, abortOnEOF)) return false; // EOF
    if (str == "if") {
      readWordIntr(str, true);
      festring res = readCondition(str, maxCPrio, prc<0);
      if (str != ";") die("';' expected");
      if (prc < 0) {
        // skiping
        mIfStack.push(-3);
      } else {
        mIfStack.push(res.IsEmpty() ? -1 : 1);
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
      mIfStack.push(prc >= 0 ? 666 : -666);
      if (prc >= 0) return true;
      continue;
    }
    if (str == "}") {
      if (abs(prc) != 666) die("unexpected '}'");
      mIfStack.pop();
      if (prc >= 0) return true;
      continue;
    }
    if (prc >= 0) return true;
  }
}


festring TextInput::ReadWord (truth abortOnEOF) {
  festring ToReturn;
  ReadWord(ToReturn, abortOnEOF);
  return ToReturn;
}


truth TextInput::readWordIntr (festring &String, truth abortOnEOF) {
  String.Empty();
  lastWordWasString = false;
  skipBlanks();
  mTokenLine = mCurrentLine;
  for (;;) {
    int ch = GetChar();
    if (ch == EOF) {
      if (abortOnEOF) ABORT("Unexpected end of file %s!", GetFileName().CStr());
      return false;
    }
    // identifier?
    if (isalpha(ch) || ch == '_') {
      String << (char)(ch);
      for (;;) {
        ch = GetChar();
        if (ch == EOF) break;
        /*
        if (isdigit(ch)) {
          if (String == "rgb") { UngetChar(ch); break; } // temporary hack
          die("identifiers cannot contain digits (yet)");
        }
        if (ch != '_' && !isalpha(ch)) { UngetChar(ch); break; }
        */
        if (ch != '_' && !isalpha(ch) && !isdigit(ch)) { UngetChar(ch); break; }
        String << (char)(ch);
      }
      return true;
    }
    // number?
    if (isdigit(ch)) {
      String << (char)(ch);
      for (;;) {
        ch = GetChar();
        if (ch == EOF) break;
        if (ch == '_') continue;
        if (isalpha(ch)) die("invalid number");
        if (!isdigit(ch)) { UngetChar(ch); break; }
        String << (char)(ch);
      }
      return true;
    }
    // string?
    if (ch == '"') {
      lastWordWasString = true;
      for (;;) {
        ch = GetChar();
        if (ch == EOF) ABORT("Unterminated string in file %s, beginning at line %d!", GetFileName().CStr(), mTokenLine);
        if (ch == '"') return true;
        if (ch == '\\') {
          ch = GetChar();
          if (ch == EOF) ABORT("Unterminated string in file %s, beginning at line %d!", GetFileName().CStr(), mTokenLine);
          switch (ch) {
            case 't': String << '\t'; break;
            case 'n': String << '\n'; break;
            case 'r': String << '\r'; break;
            case '1': String << '\x01'; break;
            case '2': String << '\x02'; break;
            case '"': String << '"'; break;
            default: ABORT("Invalid escape in string in file %s at line %d!", GetFileName().CStr(), mTokenLine);
          }
          continue;
        }
        String << (char)ch;
      }
    }
    // punctuation
    ch = gotCharSkipComment(ch);
    if (ch < 0) continue;
    // delimiter
    String << (char)ch;
    // two-char delimiters?
    if (ch == '=' || ch == '<' || ch == '>' || ch == '!') {
      ch = GetChar();
      if (ch == '=') String << (char)ch; else UngetChar(ch);
    } else if (ch == '&' || ch == '|') {
      int c1 = GetChar();
      if (c1 == ch) String << (char)c1; else UngetChar(c1);
    } else if (ch == ':') {
      ch = GetChar();
      if (ch == '=') String << (char)ch; else UngetChar(ch);
    }
    return true;
  }
}


char TextInput::ReadLetter (truth abortOnEOF) {
  mTokenLine = mCurrentLine;
  for (;;) {
    int ch = GetChar();
    if (ch == EOF) {
      if (abortOnEOF) ABORT("Unexpected end of file %s!", GetFileName().CStr());
      return 0;
    }
    if (ch <= ' ') continue;
    //ch = gotCharSkipComment(ch);
    //if (ch >= 0) return ch;
    return ch;
  }
}


/* Reads a number or a formula from inputfile. Valid values could be for
   instance "3", "5 * 4+5", "2+Variable%4" etc. */
//sLong inputfile::ReadNumber (int CallLevel, truth PreserveTerminator) {
festring TextInput::ReadNumberIntr (int CallLevel, sLong *num, truth *isString, truth allowStr, truth PreserveTerminator, truth *wasCloseBrc) {
  sLong Value = 0;
  festring Word, res;
  truth NumberCorrect = false;
  truth firstWord = true;
  if (isString) *isString = false;
  if (num) *num = 0;
  if (wasCloseBrc) *wasCloseBrc = false;
  mTokenLine = mCurrentLine;
  //fprintf(stderr, ">>> ReadNumberIntr()\n");
  for (;;) {
    ReadWord(Word);
    //fprintf(stderr, "  ReadNumberIntr: word='%s'\n", Word.CStr());
    // specials?
    if (Word == "@") {
      // variable
      if (mCollectingNumStr) mNumStr << Word;
      ReadWord(Word, true);
      if (mCollectingNumStr) mNumStr << Word;
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
        if (isString) *isString = true;
        return Word;
      } else {
        ABORT("Number expected in file %s, line %d!", GetFileName().CStr(), mTokenLine);
      }
    }
    // first word?
    if (firstWord) {
      if (allowStr && lastWordWasString) {
        if (isString) *isString = true;
        ReadWord(res);
        if (res.GetSize() == 1) {
          if (res[0] != ';' && res[0] != ',' && res[0] != ':') {
            ABORT("Invalid terminator in file %s, line %d!", GetFileName().CStr(), mTokenLine);
          }
          if (PreserveTerminator) UngetChar(res[0]);
        } else {
          ABORT("Terminator expected in file %s, line %d!", GetFileName().CStr(), mTokenLine);
        }
        return Word;
      }
      firstWord = false;
    }
    // other things
    char First = Word[0];
    // number?
    if (isdigit(First)) {
      if (mCollectingNumStr) mNumStr << Word;
      Value = atoi(Word.CStr());
      NumberCorrect = true;
      // HACK: autoinsert terminator
      skipBlanks();
      int ch = GetChar();
      if (ch != EOF) {
        UngetChar(ch);
        if (ch == '}') UngetChar(';');
      }
      continue;
    }
    // delimiter/math?
    if (Word.GetSize() == 1) {
      //fprintf(stderr, "  ReadNumberIntr: First='%c'\n", First);
      if (First == ';' || First == ',' || First == ':' || (wasCloseBrc && First == '}')) {
        if (First == '}' && wasCloseBrc) *wasCloseBrc = true;
        if (CallLevel != HIGHEST || PreserveTerminator) UngetChar(First);
        if (num) *num = Value;
        return res;
      }
      if (First == ')') {
        if ((CallLevel != HIGHEST && CallLevel != 4) || PreserveTerminator) UngetChar(')');
        if (num) *num = Value;
        return res;
      }
      if (First == '~') {
        if (mCollectingNumStr) mNumStr << Word;
        Value = ~ReadNumber(4);
        NumberCorrect = true;
        continue;
      }
/* Convert this into an inline function! */
#define CHECK_OP(op, cl) \
  if (First == #op[0]) { \
    if (cl < CallLevel) {\
      if (mCollectingNumStr) mNumStr << Word; \
      Value op##= ReadNumber(cl);\
      NumberCorrect = true;\
      continue;\
    } else {\
      UngetChar(#op[0]);\
      if (num) *num = Value;\
      return res;\
    } \
  }
      CHECK_OP(&, 1); CHECK_OP(|, 1); CHECK_OP(^, 1);
      CHECK_OP(*, 2); CHECK_OP(/, 2); CHECK_OP(%, 2);
      CHECK_OP(+, 3); CHECK_OP(-, 3);
#undef CHECK_OP
      if (First == '<') {
        char Next = GetChar();
        if (Next == '<') {
          if (1 < CallLevel) {
            if (mCollectingNumStr) mNumStr << "<<";
            Value <<= ReadNumber(1);
            NumberCorrect = true;
            continue;
          } else {
            UngetChar('<');
            UngetChar('<');
            if (num) *num = Value;
            return res;
          }
        } else {
          UngetChar(Next);
        }
      }
      if (First == '>') {
        char Next = GetChar();
        if (Next == '>') {
          if (1 < CallLevel) {
            if (mCollectingNumStr) mNumStr << ">>";
            Value >>= ReadNumber(1);
            NumberCorrect = true;
            continue;
          } else {
            UngetChar('>');
            UngetChar('>');
            if (num) *num = Value;
            return res;
          }
        } else {
          UngetChar(Next);
        }
      }
      if (First == '(') {
        if (NumberCorrect) {
          UngetChar('(');
          if (num) *num = Value;
          return res;
        } else {
          if (mCollectingNumStr) mNumStr << Word;
          Value = ReadNumber(4);
          if (mCollectingNumStr) mNumStr << ")";
          NumberCorrect = false;
          continue;
        }
      }
      if (First == '=' && CallLevel == HIGHEST) continue;
      if (First == '#') {
        // for #defines
        UngetChar('#');
        if (num) *num = Value;
        return res;
      }
    }
    /*
    if (Word == "enum" || Word == "bitenum") {
      if (CallLevel != HIGHEST || PreserveTerminator) UngetChar(';');
      if (num) *num = Value;
      return res;
    }
    */
    // rgbX?
    if (Word == "rgb16" || Word == "rgb24") {
      truth is16 = (Word == "rgb16");
      if (mCollectingNumStr) mNumStr << Word;
      int Red = ReadNumber();
      if (Red < 0 || Red > 255) ABORT("Illegal Red value (%d) file %s, line %d!", Red, GetFileName().CStr(), mTokenLine);
      int Green = ReadNumber();
      if (Green < 0 || Green > 255) ABORT("Illegal Green value (%d) file %s, line %d!", Green, GetFileName().CStr(), mTokenLine);
      int Blue = ReadNumber();
      if (Blue < 0 || Blue > 255) ABORT("Illegal Blue value (%d) file %s, line %d!", Blue, GetFileName().CStr(), mTokenLine);
      Value = (is16 ? MakeRGB16(Red, Green, Blue) : MakeRGB24(Red, Green, Blue));
      NumberCorrect = true;
      continue;
    }
    // `true` literal?
    if (Word == "true" || Word == "tan") {
      if (mCollectingNumStr) mNumStr << Word;
      Value = 1;
      NumberCorrect = true;
      continue;
    }
    // `false` literal?
    if (Word == "false" || Word == "ona") {
      if (mCollectingNumStr) mNumStr << Word;
      Value = 0;
      NumberCorrect = true;
      continue;
    }
    // known value?
    if (ValueMap) {
      valuemap::const_iterator Iterator = ValueMap->find(Word);
      if (Iterator != ValueMap->end()) {
        if (mCollectingNumStr) mNumStr << Word;
        Value = Iterator->second;
        NumberCorrect = true;
        continue;
      }
    }
    // something bad
    ABORT("Odd numeric value \"%s\" encountered in file %s, line %d!", Word.CStr(), GetFileName().CStr(), mTokenLine);
  }
}


festring TextInput::ReadCode (truth abortOnEOF) {
  int sqLevel = 1;
  char inString = 0;
  festring res;
  mTokenLine = mCurrentLine;
  for (int ch = GetChar(); !Eof(); ch = GetChar()) {
    //fprintf(stderr, "char: [%c]; inString: %d; sqLevel: %d\n", (Char < 32 || Char > 126 ? '?' : Char), inString, sqLevel);
    if (inString) {
      res << ((char)ch);
      if (ch == inString) {
        inString = 0;
      } else if (ch == '\\') {
        if (Eof()) break;
        ch = GetChar();
        res << ((char)ch);
      }
    } else {
      if (ch == '[') {
        ++sqLevel;
        res << ((char)ch);
      } else if (ch == ']') {
        if (--sqLevel == 0) break;
        res << ((char)ch);
      } else if (ch == '/') {
        if (Eof()) { res << ((char)ch); break; }
        switch ((ch = GetChar())) {
          case '/': // eol comment
            while (!Eof()) if (GetChar() == '\n') break;
            break;
          case '*': // c-like comment
            while (!Eof()) {
              if (GetChar() == '*') {
                if (Eof()) break;
                if (GetChar() == '/') break;
              }
            }
            break;
          default:
            res << '/';
            res << ((char)ch);
            break;
        }
      } else if (ch == '"' || ch == '\'') {
        res << ((char)ch);
        inString = ((char)ch);
      } else {
        res << ((char)ch);
      }
    }
  }
  if (abortOnEOF && Eof()) ABORT("Unexpected end of file %s!", GetFileName().CStr());
  return res;
}


sLong TextInput::ReadNumber (int CallLevel, truth PreserveTerminator, truth *wasCloseBrc) {
  sLong num = 0;
  truth isString = false;
  ReadNumberIntr(CallLevel, &num, &isString, false, PreserveTerminator, wasCloseBrc);
  return num;
}


festring TextInput::ReadStringOrNumber (sLong *num, truth *isString, truth PreserveTerminator, truth *wasCloseBrc) {
  return ReadNumberIntr(HIGHEST, num, isString, true, PreserveTerminator, wasCloseBrc);
}


// ////////////////////////////////////////////////////////////////////////// //
// fuck you, shitplusplus!
struct FuckedShitForFuckedFinally {
  truth *var;
  truth oval;
  FuckedShitForFuckedFinally (truth *avar, truth nval=true) {
    var = avar;
    oval = *var;
    *var = nval;
  }
  ~FuckedShitForFuckedFinally () {
    *var = oval;
  }
};


sLong TextInput::ReadNumberKeepStr (int CallLevel, truth PreserveTerminator, truth *wasCloseBrc) {
  auto fuck = FuckedShitForFuckedFinally(&mCollectingNumStr);
  mNumStr = "";
  return ReadNumber(CallLevel, PreserveTerminator, wasCloseBrc);
}


festring TextInput::ReadStringOrNumberKeepStr (sLong *num, truth *isString, truth PreserveTerminator, truth *wasCloseBrc) {
  auto fuck = FuckedShitForFuckedFinally(&mCollectingNumStr);
  mNumStr = "";
  return ReadStringOrNumber(num, isString, PreserveTerminator, wasCloseBrc);
}


// ////////////////////////////////////////////////////////////////////////// //
v2 TextInput::ReadVector2d () {
  skipBlanks();
  int ch = GetChar();
  if (ch == '{') ch = '}'; else { UngetChar(ch); ch = 0; }

  v2 Vector;
  Vector.X = ReadNumber();
  Vector.Y = ReadNumber();

  if (ch) {
    skipBlanks();
    if (GetChar() != ch) ABORT("Vector syntax error: \"%c\" expected in file %s, line %d!", ch, GetFileName().CStr(), TokenLine());
    skipBlanks();
    ch = GetChar();
         if (ch == '}') UngetChar(ch);
    else if (ch != ';' && ch != ',') ABORT("Vector syntax error: terminator expected in file %s, line %d!", GetFileName().CStr(), TokenLine());
  }

  return Vector;
}


rect TextInput::ReadRect () {
  skipBlanks();
  int ch = GetChar();
  if (ch == '{') ch = '}'; else { UngetChar(ch); ch = 0; }

  rect Rect;
  Rect.X1 = ReadNumber();
  Rect.Y1 = ReadNumber();
  Rect.X2 = ReadNumber();
  Rect.Y2 = ReadNumber();

  if (ch) {
    skipBlanks();
    if (GetChar() != ch) ABORT("Vector syntax error: \"%c\" expected in file %s, line %d!", ch, GetFileName().CStr(), TokenLine());
    skipBlanks();
    ch = GetChar();
         if (ch == '}') UngetChar(ch);
    else if (ch != ';' && ch != ',') ABORT("Vector syntax error: terminator expected in file %s, line %d!", GetFileName().CStr(), TokenLine());
  }

  return Rect;
}


// ////////////////////////////////////////////////////////////////////////// //
void ReadData (festring &String, TextInput &SaveFile) {
  SaveFile.ReadWord(String);
  if (String == "=") SaveFile.ReadWord(String);
  SaveFile.ReadWord();
}


void ReadData (fearray<sLong> &Array, TextInput &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    Array.Data[0] = SaveFile.ReadNumber();
  } else if (Word == ":=") {
    SaveFile.ReadWord(Word);
    if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    std::vector<sLong> v;
    for (;;) {
      truth wasCloseBrc = false;
      sLong n = SaveFile.ReadNumber(HIGHEST, false, &wasCloseBrc);
      if (wasCloseBrc) break;
      v.push_back(n);
    }
    Array.Allocate(v.size());
    for (unsigned int f = 0; f < v.size(); ++f) Array.Data[f] = v[f];
  } else if (Word == "=") {
    SaveFile.ReadWord(Word);
    if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    fearray<sLong>::sizetype Size = SaveFile.ReadNumber();
    Array.Allocate(Size);
    for (fearray<sLong>::sizetype c = 0; c < Size; ++c) Array.Data[c] = SaveFile.ReadNumber();
    if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  } else {
    ABORT("Array syntax error: '=', '==' or ':=' expected in file %s, line %d!", SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  }
}


void ReadData (fearray<festring> &Array, TextInput &SaveFile) {
  Array.Clear();
  festring Word;
  SaveFile.ReadWord(Word);
  if (Word == "==") {
    Array.Allocate(1);
    SaveFile.ReadWord(Array.Data[0]);
    if (SaveFile.ReadWord() != ";") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  } else if (Word == ":=") {
    SaveFile.ReadWord(Word);
    if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    //
    std::vector<festring> v;
    //
    for (;;) {
      SaveFile.ReadWord(Word);
      if (Word == "}") break;
      v.push_back(Word);
      SaveFile.ReadWord(Word);
      if (Word == "}") break;
      if (Word != "," && Word != ";") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    }
    Array.Allocate(v.size());
    for (unsigned int f = 0; f < v.size(); ++f) Array.Data[f] = v[f];
  } else if (Word == "=") {
    SaveFile.ReadWord(Word);
    if (Word != "{") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    fearray<festring>::sizetype Size = SaveFile.ReadNumber();
    Array.Allocate(Size);
    for (fearray<festring>::sizetype c = 0; c < Size; ++c) {
      SaveFile.ReadWord(Array.Data[c]);
      SaveFile.ReadWord(Word);
      if (Word != "," && Word != ";") ABORT("Array syntax error \"%s\" found in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
    }
    if (SaveFile.ReadWord() != "}") ABORT("Illegal array terminator \"%s\" encountered in file %s, line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  } else {
    ABORT("Array syntax error: '=', '==' or ':=' expected in file %s, line %d!", SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
  }
}


// ////////////////////////////////////////////////////////////////////////// //
TextInputFile::TextInputFile (cfestring &FileName, const valuemap *aValueMap, truth AbortOnErr) {
  ifile.Open(FileName, AbortOnErr);
  setup(aValueMap);
}


TextInputFile::~TextInputFile () {
  //fprintf(stderr, "TIF:~this();\n");
  Close();
  //fprintf(stderr, "TIF:~this(); -- exit\n");
}


cfestring &TextInputFile::GetFileName () const { return ifile.GetFileName(); }
int TextInputFile::realGetChar () { return ifile.Get(); }
truth TextInputFile::isRealEof () { return ifile.Eof(); }
truth TextInputFile::IsOpen () { return ifile.IsOpen(); }
void TextInputFile::Close () { /*fprintf(stderr, "TIF:Close();\n");*/ ifile.Close(); TextInput::Close(); }

sLong TextInputFile::realGetPos () { return (ifile.IsOpen() ? ifile.TellPos() : 0); }
void TextInputFile::realSetPos (sLong apos) { if (ifile.IsOpen()) ifile.SeekPosBegin(apos); }



// ////////////////////////////////////////////////////////////////////////// //
MemTextFile::MemTextFile (cfestring &afname, cfestring &str, const valuemap *aValueMap) :
  buf(nullptr),
  bufSize(0),
  bufPos(0),
  tfname(afname)
{
  bufSize = str.GetSize();
  buf = (unsigned char *)calloc(1, bufSize+1);
  memmove(buf, str.CStr(), bufSize);
  setup(aValueMap);
}


MemTextFile::~MemTextFile () {
  //fprintf(stderr, "MTF:~this();\n");
  Close();
  //fprintf(stderr, "MTF:~this(); -- exit\n");
}


int MemTextFile::realGetChar () {
  if (bufPos >= bufSize) return EOF;
  return buf[bufPos++];
}


cfestring &MemTextFile::GetFileName () const { return tfname; }
truth MemTextFile::isRealEof () { return (bufPos >= bufSize); }
truth MemTextFile::IsOpen () { return (buf != nullptr); }


void MemTextFile::Close () {
  //fprintf(stderr, "MTF:Close();\n");
  if (buf) {
    free(buf);
    buf = nullptr;
    tfname = "";
    bufSize = 0;
    bufPos = 0;
  }
  TextInput::Close();
}

sLong MemTextFile::realGetPos () { return bufPos; }
void MemTextFile::realSetPos (sLong apos) { if (buf != nullptr) bufPos = apos; }
