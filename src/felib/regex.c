/*
 * Copyright (c) 2003
 *      David Leonard.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of David Leonard nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* $Id: regex.c 1282 2007-09-13 22:26:35Z d $ */
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "regex.h"

/*
 * Regular expression engine.
 *
 * This module contains a parser to compile ECMA-262 regular expressions
 * into 'p-code', and a matcher engine that runs the p-code against
 * strings.
 *
 * NOTE: ECMA-262 "regular" expressions are not actually regular in
 * a technical sense, since the presence of non-greedy and zero-width
 * lookahead patterns make the matching process an NP-complete algorithm
 * rather than a linear process implied by context-free finite automata.
 * (See the NP-complete regex discussion at <http://perl.plover.com/NPC/>)
 *
 * The regex parser generates a 'SEE_RegExpr' which contains the bytecode
 * ('p-code'), that when executed will try to match the pattern against the
 * given string, commencing at the given index into the string.
 */
#ifndef NDEBUG
# define NDEBUG
#endif

#ifdef TJS_RE_DUMP
# undef NDEBUG
#endif


////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
# define dprintf(...)  fprintf(stderr, __VA_ARGS__)
static void dprints (const char *str, int len) {
  fputc('"', stderr);
  for (; len > 0; len--, str++) {
    uint8_t cc = (uint8_t)(*str);
    if (cc < ' ') fputc('.', stderr); else fputc(cc, stderr);
  }
  fputc('"', stderr);
}
#else
# define dprintf(...)
# define dprints(...)
#endif


////////////////////////////////////////////////////////////////////////////////
typedef unsigned char  SEE_unicode_t;
typedef int  SEE_boolean_t;

#define UNICODE_TOUPPER(ch)  toupper(ch)


////////////////////////////////////////////////////////////////////////////////
typedef struct {
  const char *str;
  int strLen;
  int pos; // next char to be taken
  int eof;
} XXInput;


static XXInput *xxiNew (const char *source, int srcLen) {
  XXInput *res = calloc(1, sizeof(XXInput));
  if (srcLen < 0) srcLen = strlen(source);
  res->str = source;
  res->strLen = srcLen;
  res->pos = 0;
  res->eof = srcLen<=0;
  return res;
}


static void SEE_INPUT_CLOSE (XXInput *input) {
  free(input);
}


static SEE_unicode_t xxiAhead (XXInput *input) {
  if (input->pos >= input->strLen) {
    input->eof = 1;
    return 0;
  }
  return input->str[input->pos];
}


static void SEE_INPUT_NEXT (XXInput *input) {
  input->pos++;
  if (input->pos >= input->strLen) {
    input->pos = input->strLen;
    input->eof = 1;
  }
}


static int SEE_input_lookahead_copy (XXInput *input, SEE_unicode_t *buf, int len) {
  uint32_t opos = input->pos;
  int f;
  if (len <= 0) return 0;
  memset(buf, 0, len);
  for (f = 0; f < len; f++) {
    if (input->pos >= input->strLen) break;
    buf[f] = input->str[input->pos++];
  }
  input->pos = opos;
  return f;
}


////////////////////////////////////////////////////////////////////////////////
typedef struct rxGrowable {
  uint8_t *data;
  int used;
  int size;
  int is_string;
  void **xptr;
  int elSize;
  unsigned int *usedPtr;
} rxGrowable;


static void SEE_grow_init (rxGrowable *gr, void **xptr, int elSize, unsigned int *usedPtr) {
  if (elSize < 1) elSize = 1;
  //fprintf(stderr, "GROW %p: elSize=%d\n", gr, elSize);
  gr->elSize = elSize;
  gr->data = NULL;
  gr->used = gr->size = 0;
  gr->is_string = 0;
  gr->xptr = xptr;
  if (gr->xptr) *(gr->xptr) = gr->data;
  gr->usedPtr = usedPtr;
  if (gr->usedPtr) *(gr->usedPtr) = gr->used;
}

#define SEE_GROW_INIT(gr, xptr, uptr)  SEE_grow_init(gr, (void **)(&(xptr)), sizeof(*xptr), &uptr)


static void SEE_grow_to (rxGrowable *gr, int minUsed) {
  if (minUsed >= gr->size) {
    int nsz = minUsed+128;
    void *p = realloc(gr->data, nsz*gr->elSize);
    if (!p) abort();
    gr->data = p;
    gr->size = nsz;
    if (gr->xptr) *(gr->xptr) = gr->data;
  }
  gr->used = minUsed;
  if (gr->usedPtr) *(gr->usedPtr) = gr->used;
}


static void SEE_grow_deinit (rxGrowable *gr) {
  if (gr) {
    if (gr->data) free(gr->data);
  }
}


////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int SEE_regex_debug = 1;
#endif


enum {
  OP_FAIL,    /* match failed */
  OP_SUCCEED, /* match succeeded */
  OP_CHAR,    /* match a char class instance */
  OP_ZERO,    /* reset counter */
  OP_REACH,   /* test counter over */
  OP_NREACH,  /* test counter under */
  OP_START,   /* enter a group */
  OP_END,     /* exit a group */
  OP_UNDEF,   /* reset a group */
  OP_MARK,    /* record a position */
  OP_FDIST,   /* position test */
  OP_RDIST,   /* position and counter test */
  OP_MNEXT,   /* max-loop */
  OP_RNEXT,   /* reach-loop */
  OP_GOTO,    /* branch */
  OP_GS,      /* greedy success */
  OP_NS,      /* non-greedy success */
  OP_GF,      /* greedy fail */
  OP_NF,      /* non-greedy fail */
  OP_AS,      /* assert success */
  OP_AF,      /* assert fail */
  OP_BOL,     /* test beginning of line */
  OP_EOL,     /* test end of line */
  OP_BRK,     /* test word-break */
  OP_NBRK,    /* test non-word-break */
  OP_BACKREF, /* backreference match */

  OP_LAST
};


typedef struct CharClassRange CharClassRange;
typedef struct CharClass CharClass;


struct CharClassRange {
  CharClassRange *next;
  uint32_t lo; /* simple range of chars, eg [a-z] */
  uint32_t hi;
  CharClassRange *prev; // all allocated
};


struct CharClass {
  CharClassRange *ranges; /* linked list of character ranges */
  CharClass *prev; // all allocated
};


struct SEE_RegExpr {
  int ncaptures, ncounters, nmarks, maxref;
  int statesz;
  unsigned char *code;
  unsigned int codelen;
  rxGrowable codegrow;
  CharClass **cc;
  unsigned int cclen;
  rxGrowable ccgrow;
  int flags;
  CharClassRange *crList; // all allocated
  CharClass *ccList;
  jmp_buf errJP;
};


typedef struct REContext {
  XXInput *input;
  SEE_RegExpr *regex;
  jmp_buf errJP;
} REContext;


////////////////////////////////////////////////////////////////////////////////
#define SEE_NEW(eng, type)     calloc(1, sizeof(type))
#define SEE_ASSERT(eng, cond)  assert(cond)


#define NEW1(t)     SEE_NEW(REContext->eng, t)
#define NEXT        xxiAhead(REContext->input)
#define SKIP        SEE_INPUT_NEXT(REContext->input)
#define ATEOF       (REContext->input->eof)

#define LOOKAHEAD(buf, len)  SEE_input_lookahead_copy(REContext->input, buf, len)

//  SEE_error_throw_string(REContext->eng, REContext->eng->SyntaxError, STR(regex_syntax_error))
#define SYNTAX_ERROR \
  { \
    /*fprintf(stderr, "REGEX: syntax error\n");*/ \
    longjmp(REContext->errJP, 666); \
  }

#define EXPECT(c) \
  do { \
    if (ATEOF || NEXT != (c)) SYNTAX_ERROR; \
    SKIP; \
  } while (0)

#define RELADDR(base, addr)  ((addr)-(base))
#define ASSERT(x)            SEE_ASSERT(REContext->eng, x)
#define CODE_ADD(c)          code_add(REContext, c)
#define CODE_INSERT(pos, n)  code_insert(REContext, pos, n)
#define CODE_POS             REContext->regex->codelen
#define CODE_PATCH(pos, c)   REContext->regex->code[pos] = c

#define CODE_ADDI(i) \
  do { \
    unsigned int _i = (i); \
    CODE_ADD((_i >> 8) & 0xff); \
    CODE_ADD(_i & 0xff); \
 } while (0)

#define CODE_ADDA(i)  CODE_ADDI(RELADDR(CODE_POS, i))

#define CODE_PATCHI(pos, i)           \
    do { CODE_PATCH(pos, ((i) >> 8) & 0xff);      \
   CODE_PATCH((pos)+1, (i) & 0xff); } while (0)
#define CODE_PATCHA(addr, i)  CODE_PATCHI(addr, RELADDR(addr, i))
#define CODE_SZA  2
#define CODE_SZI  2

#define CODE_MAKEI(code, addr)  ((code[addr] << 8) | code[(addr)+1])
#define CODE_MAKEA(code, addr)  ((CODE_MAKEI(code, addr) + (addr)) & 0xffff)

#define CC_NEW()                cc_new(REContext)
#define CC_ADDRANGE(cc, l, h)   cc_add_range(REContext, cc, l, (h)+1)
#define CC_ADDCHAR(cc, ch)      CC_ADDRANGE(cc, ch, ch)
#define CC_INVERT(cc)         cc_invert(REContext, cc)
#define CC_ADDCC(dst, src)      cc_add_cc(REContext, dst, src)
#define CC_INTERN(cc)         cc_intern(REContext, cc)

#define UNDEFINED   (-1)
#undef INFINITY /* XXX fixme - rename INFINITY in this file maybe? */
#define INFINITY  (-1)


////////////////////////////////////////////////////////////////////////////////
// Prototypes
//
static CharClass *cc_new (REContext *);
static void cc_add_range (REContext *, CharClass *, SEE_unicode_t, SEE_unicode_t);
static void cc_invert (REContext *, CharClass *);
static void cc_add_cc (REContext *, CharClass *, CharClass *);
static int cc_issingle (CharClass *);
static uint32_t cc_count (CharClass *);
static int cc_cmp (CharClass *, CharClass *);
static int cc_intern (REContext *, CharClass *);
static int cc_contains (CharClass *, SEE_unicode_t);
static SEE_RegExpr *regex_new (REContext *);
static void code_add (REContext *, int);
static void code_insert (REContext *, int, int);

static void Disjunction_parse (REContext *);
static void Alternative_parse (REContext *);
static void Term_parse (REContext *);
static int Quantifier_is_next (REContext *);
static int Integer_parse (REContext *);
static void Atom_parse (REContext *);
static unsigned char HexDigit_parse (REContext *);
static CharClass *ClassEscape_parse (REContext *);
static CharClass *ClassAtom_parse (REContext *);
static CharClass *CanonicalizeClass (REContext *, CharClass *);
static CharClass *CharacterClass_parse (REContext *);

#ifndef NDEBUG
static void dprint_ch (SEE_unicode_t);
static void dprint_cc (CharClass *);
static int  dprint_code (SEE_RegExpr *, int);
static void dprint_regex (SEE_RegExpr *);
#endif

static SEE_unicode_t Canonicalize (SEE_RegExpr *, SEE_unicode_t);
static SEE_boolean_t pcode_run (SEE_RegExpr *regex, unsigned int addr, const char *text, int textLen, char *state);
static void optimize_regex (SEE_RegExpr *);


////////////////////////////////////////////////////////////////////////////////
// CharClass
//
// Create a new, empty CharClass
static CharClass *cc_new (REContext *REContext) {
  CharClass *c;
  c = NEW1(struct CharClass);
  c->ranges = NULL;
  c->prev = REContext->regex->ccList;
  REContext->regex->ccList = c;
  return c;
}


// Add a range to a CharClass
static void cc_add_range (REContext *REContext, CharClass *c, SEE_unicode_t lo, SEE_unicode_t hi) {
  CharClassRange *s, *newr = NULL;
  CharClassRange **rp;
  for (rp = &c->ranges; *rp; rp = &(*rp)->next) if (lo <= (*rp)->hi) break;
  if (!*rp || hi < (*rp)->lo) {
    newr = NEW1(struct CharClassRange);
    newr->lo = lo;
    newr->hi = hi;
    newr->next = *rp;
    *rp = newr;
    newr->prev = REContext->regex->crList;
    REContext->regex->crList = newr;
  } else {
    if (lo < (*rp)->lo) (*rp)->lo = lo;
    if (hi > (*rp)->hi) {
      (*rp)->hi = hi;
      s = (*rp)->next;
      while (s && s->hi < hi) (*rp)->next = s = s->next;
      if (s && s->lo <= hi) {
        (*rp)->hi = s->hi;
        (*rp)->next = s->next;
      }
    }
  }
}


// Invert a CharClass
static void cc_invert (REContext *REContext, CharClass *c) {
  CharClassRange *l, *newlist, *r;
  r = c->ranges;
  if (r && r->lo == 0 && r->hi == ~0) {
    c->ranges = NULL;
    return;
  }
  l = newlist = NEW1(struct CharClassRange);
  l->prev = REContext->regex->crList;
  REContext->regex->crList = l;
  if (r && r->lo == 0) {
    l->lo = r->hi;
    r = r->next;
  } else l->lo = 0;
  for (; r; r = r->next) {
    l->hi = r->lo;
    if (r->hi == ~0) {
      l->next = NULL;
      l = NULL;
      break;
    }
    l = (l->next = NEW1(struct CharClassRange));
    l->lo = r->hi;
    l->prev = REContext->regex->crList;
    REContext->regex->crList = l;
  }
  if (l) {
    l->hi = ~0;
    l->next = NULL;
  }
  c->ranges = newlist;
}


static void cc_add_cc (REContext *REContext, CharClass *dst, CharClass *src) {
  CharClassRange *r;
  //FIXME: very inefficient
  for (r = src->ranges; r; r = r->next) cc_add_range(REContext, dst, r->lo, r->hi);
}


static int cc_issingle (CharClass *c) {
  CharClassRange *r = c->ranges;
  return r != NULL &&
         r->next == NULL &&
         r->lo+1 == r->hi;
}


// Return the number of characters in the class
static uint32_t cc_count (CharClass *c) {
  uint32_t count = 0;
  CharClassRange *r;
  for (r = c->ranges; r; r = r->next) count += r->hi-r->lo;
  return count;
}


// Return 0 if two CharClasses are identical
static int cc_cmp (CharClass *c1, CharClass *c2) {
  CharClassRange *r1, *r2;
  r1 = c1->ranges;
  r2 = c2->ranges;
  while (r1 && r2) {
    if (r1->lo != r2->lo) return r1->lo-r2->lo;
    if (r1->hi != r2->hi) return r1->hi-r2->hi;
    r1 = r1->next;
    r2 = r2->next;
  }
  if (r1) return 1;
  if (r2) return -1;
  return 0;
}


// Insert CharClass into REContext, returning unique ID
static int cc_intern (REContext *REContext, CharClass *c) {
  SEE_RegExpr *regex = REContext->regex;
  int i;
  for (i = 0; i < regex->cclen; i++) if (cc_cmp(c, regex->cc[i]) == 0) return i;
  i = regex->cclen;
  SEE_grow_to(&regex->ccgrow, regex->cclen+1);
  regex->cc[i] = c;
  return i;
}


// Return true if CharClass c contains character ch
static int cc_contains (CharClass *c, SEE_unicode_t ch) {
  CharClassRange *r;
  for (r = c->ranges; r; r = r->next) {
    if (ch >= r->lo && ch < r->hi) return 1;
    if (ch < r->lo) return 0;
  }
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
// Print a character in a readable form
static void dprint_ch (SEE_unicode_t ch) {
  switch (ch) {
    case '{': case '}': case '[': case ']':
    case '(': case ')': case '-':
    case '.': case '^': case '$': case '|':
    case '?': case '*': case '+': case '\\':
      dprintf("\\%c", ch & 0x7f); break;
    case 0x0000: dprintf("\\0"); break;
    case 0x0009: dprintf("\\t"); break;
    case 0x000a: dprintf("\\n"); break;
    case 0x000b: dprintf("\\v"); break;
    case 0x000c: dprintf("\\f"); break;
    case 0x000d: dprintf("\\r"); break;
    default:
      if (ch >= ' ' && ch <= '~') dprintf("%c", ch & 0x7f);
      else if (ch < 0x100) dprintf("\\x%02x", ch & 0xff);
      else dprintf("\\u%04x", ch);
  }
}


/// Print a character class in a readable form
static void dprint_cc (CharClass *c) {
  CharClassRange *r;
  dprintf("[");
  if (c->ranges && c->ranges->lo == 0) {
    dprintf("^");
    for (r = c->ranges; r; r = r->next) {
      if (r->next) {
        dprint_ch(r->hi);
        if (r->next->lo != r->hi + 1) {
          dprintf("-");
          dprint_ch(r->next->lo - 1);
        }
      } else if (r->hi != ~0) {
        dprint_ch(r->hi);
        dprintf("-");
        dprint_ch(~0);
      }
    }
  } else {
    for (r = c->ranges; r; r = r->next) {
      dprint_ch(r->lo);
      if (r->hi != r->lo + 1) {
        dprintf("-");
        dprint_ch(r->hi - 1);
      }
    }
  }
  dprintf("]");
}
#endif


////////////////////////////////////////////////////////////////////////////////
// regex and pcode construction
//

/*
 * Allocate a new regex structure. This will contain everything needed
 * to run and match a string, independent of the original pattern text.
 */
static SEE_RegExpr *regex_new (REContext *REContext) {
  SEE_RegExpr *regex;
  regex = NEW1(struct SEE_RegExpr);
  regex->ncaptures = 0;
  regex->maxref = 0;
  regex->ncounters = 0;
  regex->nmarks = 0;
  regex->statesz = 0;
  SEE_GROW_INIT(&regex->codegrow, regex->code, regex->codelen);
  regex->codegrow.is_string = 1;
  SEE_GROW_INIT(&regex->ccgrow, regex->cc, regex->cclen);
  regex->flags = 0;
  return regex;
}


// add to the end of the p-code array, resizing as needed
static void code_add (REContext *REContext, int c) {
  SEE_RegExpr *regex = REContext->regex;
  unsigned int i;
  i = regex->codelen;
  SEE_grow_to(&regex->codegrow, i+1);
  regex->code[i] = c;
}


// insert some bytes into the middle of the p-code, resizing as needed
static void code_insert (REContext *REContext, int pos, int n) {
  SEE_RegExpr *regex = REContext->regex;
  int i;
  for (i = 0; i < n; i++) code_add(REContext, 0);
  for (i = regex->codelen - n; i > pos; i--) regex->code[i-1+n] = regex->code[i-1];
}


////////////////////////////////////////////////////////////////////////////////
// Parser
/*
 * This recursive descent parser builds a p-code array as it runs.
 * During recursion, the p-code array is sometimes 'back-patched'
 * because branch distances weren't known in advance. In some
 * cases, p-code segments are also shifted. This all means that we
 * have to be very careful that our p-code is quite relocatable,
 * and not dependendent on absolute addresses.
 */

// parse a source pattern, and return a filled-in regex structure
SEE_RegExpr *SEE_regex_parse (const char *source, int patLen, int flags) {
  REContext *REContext;
  SEE_RegExpr *regex;
  REContext = SEE_NEW(eng, struct REContext);
  //REContext->input = SEE_input_lookahead(SEE_input_string(eng, source), 24);
  REContext->input = xxiNew(source, patLen);
  REContext->regex = regex = regex_new(REContext);
  regex->flags = flags;
  regex->ncaptures = 1;
  if (setjmp(REContext->errJP)) {
    free(REContext);
    SEE_regex_free(regex);
    return NULL;
  }
  Disjunction_parse(REContext);
  if (!ATEOF) SYNTAX_ERROR;
  CODE_ADD(OP_SUCCEED);
  // Check that no backreferences were too big
  if (regex->maxref >= regex->ncaptures) SYNTAX_ERROR;
  //FIXME: XXX - should this close be enclosed in a 'finally'?
  SEE_INPUT_CLOSE(REContext->input);
  // compute the size of a captures context
  regex->statesz =
    regex->ncaptures*sizeof(Capture)+
    regex->ncounters*sizeof(int)+
    regex->nmarks*sizeof(int);
  optimize_regex(regex);
#ifndef NDEBUG
  if (SEE_regex_debug) {
    dprintf("regex:");
    dprint_regex(regex);
    dprintf(".\n");
  }
#endif
  free(REContext);
  return regex;
}


// Returns the number of capture parentheses in the compiled regex
int SEE_regex_count_captures (const SEE_RegExpr *regex) {
  return regex->ncaptures;
}


// Returns the flags of the regex object
int SEE_regex_get_flags (const SEE_RegExpr *regex) {
  return regex->flags;
}


/*
 * Disjunction :: Alternative
 *      Alternative | Disjunction
 */
static void Disjunction_parse (REContext *REContext) {
  int pos = CODE_POS;
  Alternative_parse(REContext);
  if (!ATEOF && NEXT == '|') {
    int p = pos, p1, p2, x1, x2;
    int insert = 1+CODE_SZA;
    SKIP;
    CODE_INSERT(pos, insert);
    CODE_PATCH(p, OP_GF); p++;    /* GF x1 */
    p1 = p; p += CODE_SZA;
    ASSERT(p == pos + insert);    /* (a) */
    CODE_ADD(OP_GOTO);      /* GOTO x2 */
    p2 = CODE_POS; CODE_ADDA(0);
    x1 = CODE_POS;        /* x1: (b) */
    Disjunction_parse(REContext);
    x2 = CODE_POS;        /* x2: */
    CODE_PATCHA(p1, x1);
    CODE_PATCHA(p2, x2);
  }
}


/*
 * Alternative :: [empty]   -- lookahead in ) |
 *      Term
 *      Term Alternative
 */
static void Alternative_parse (REContext *REContext) {
  while (!(ATEOF || NEXT == /*(*/')' || NEXT == '|')) Term_parse(REContext);
}


/*------------------------------------------------------------
 * Term ::
 *  Assertion   la = ^ $ \b \B
 *  Atom
 *  Atom Quantifier
 */

/* We have 24 bytes of lookahead, which is sufficient to
 * scan for {2147483647,2147483647}. Anything larger will
 * overflow the signed int type on 32 bit systems.
 */
static int Quantifier_is_next (REContext *REContext) {
  int pos, len;
  SEE_unicode_t lookahead[24];
  if (NEXT != '{') return 0;
  /*
   * Strict ECMA-262 says that '{' is NOT a Pattern character,
   * but Mozilla allows it
   */
/*EXT:24*/
  //if (!SEE_COMPAT_JS(REContext->eng, >=, JS11)) return 1;
  len = LOOKAHEAD(lookahead, 24);
  pos = 1;
  while (pos < len && lookahead[pos] >= '0' && lookahead[pos] <= '9') pos++;
  if (pos < len && lookahead[pos] == ',') pos++;
  else if (pos < len && lookahead[pos] == '}') return pos > 1;
  else return 0;
  while (pos < len && lookahead[pos] >= '0' && lookahead[pos] <= '9') pos++;
  if (pos < len && lookahead[pos] == '}') pos++; else return 0;
  return 1;
}


static void Term_parse (REContext *REContext) {
  int min, max, greedy, pos;
  int oparen, cparen;
  int lookahead_len;
  SEE_unicode_t lookahead[2];
  /*
   * parse Assertion inline since it is a bit special
   * in terms of its lookahead
   */
  switch (NEXT) {
    case '\\':
      lookahead_len = LOOKAHEAD(lookahead, 2);
      if (lookahead_len > 1 && lookahead[1] == 'b') {
        SKIP; SKIP;
        CODE_ADD(OP_BRK);
        return;
      }
      if (lookahead_len > 1 && lookahead[1] == 'B') {
        SKIP; SKIP;
        CODE_ADD(OP_NBRK);
        return;
      }
      // some other kind of escape
      break;
    case '^':
      SKIP;
      CODE_ADD(OP_BOL);
      return;
    case '$':
      SKIP;
      CODE_ADD(OP_EOL);
      return;
    // Lookaheads forbidden by the Atom production
    case '*': case '+': case '?': case ')':
    case ']': case '{': case '}': case '|':
      SYNTAX_ERROR;
  }
  pos = CODE_POS;
  oparen = REContext->regex->ncaptures;
  Atom_parse(REContext);
  cparen = REContext->regex->ncaptures;
  /*
   * parse Quantifier inline to save my sanity
   */
  if (ATEOF) {
    min = max = 1;
  } else if (NEXT == '*') {
    SKIP; min = 0; max = INFINITY;
  } else if (NEXT == '+') {
    SKIP; min = 1; max = INFINITY;
  } else if (NEXT == '?') {
    SKIP; min = 0; max = 1;
  } else if (Quantifier_is_next(REContext)) {
    SKIP;
    min = Integer_parse(REContext);
    if (!ATEOF && NEXT == ',') {
      EXPECT(',');
      if (!ATEOF && NEXT == '}') max = INFINITY;
      else max = Integer_parse(REContext);
    } else max = min;
    EXPECT('}');
  } else {
    min = max = 1;
  }
  if (!ATEOF && NEXT == '?') {
    SKIP; greedy = 0;
  } else greedy = 1;
  if (min == max && !greedy) {
    /*
     * XXX should we warn that the greedy modifiers to
     * 'a{n,n}?' and 'a{n}?' are technically meaningless?
     * We speed up our code by using greedy mode anyway.
     */
    greedy = 1;
  }
  /* Don't allow stupid ranges, such as 'a{7,3}' */
  if (max != INFINITY && min > max) SYNTAX_ERROR;
  if (min == 1 && max == 1) return; /* a */
  if (max == 0) {
    /* a{0} */
    /* Undo! */
    CODE_POS = pos;
    return;
  }
  if (oparen != cparen) {
    /*
     * If the atom introduces capture parentheses,
     * then insert code to reset them before each
     * iteration.
     */
    CODE_INSERT(pos, 1 + 2*CODE_SZI);
    CODE_PATCH(pos, OP_UNDEF);
    CODE_PATCHI(pos + 1, oparen);
    CODE_PATCHI(pos + 1 + CODE_SZI, cparen);
  }
  /*
   * The following code generators all generate looping
   * matchers.  While every corresponding pattern could
   * be written as the generalised 'a{n,m}' (where m could
   * be INFINITY), some efficiency is gained by selecting cases
   * where general code that would be a no-op is not emitted.
   */
  if (min == max) {       /* a{m} */
    int p = pos, px;
    int insert = 1 + CODE_SZI;
    int c = REContext->regex->ncounters++;
    CODE_INSERT(pos, insert);
    CODE_PATCH(p, OP_ZERO); p++;    /* ZERO c; */
    CODE_PATCHI(p, c); p += CODE_SZI;
    px = p;         /* x: */
    ASSERT(p == pos + insert);    /*  (a) */
    CODE_ADD(OP_RNEXT);     /* RNEXT c,m,x */
    CODE_ADDI(c);
    CODE_ADDI(max);
    CODE_ADDA(px);
    return;
  }
  if (min == 0 && max == 1) {
    /* a? */
    int p = pos, p1, px;
    int insert = 1 + CODE_SZA;
    CODE_INSERT(pos, insert);
    CODE_PATCH(p, greedy ? OP_GF : OP_NF);  /* GF x */
    p++; p1 = p; p += CODE_SZA;
    ASSERT(p == pos + insert);    /* (a) */
    px = CODE_POS;        /* x: */

    CODE_PATCHA(p1, px);
    return;
  }
  if (min == 0 && max == INFINITY) {
    /* a* */
    int p = pos, px, py, p1;
    int insert = 2 + CODE_SZA + CODE_SZI;
    int m = REContext->regex->nmarks++;
    CODE_INSERT(pos, insert);
    px = p;         /* x: GF y */
    CODE_PATCH(p, greedy ? OP_GF : OP_NF); p++;
    p1 = p; p += CODE_SZA;
    CODE_PATCH(p, OP_MARK); p++;    /* MARK m */
    CODE_PATCHI(p, m); p += CODE_SZI;
    ASSERT(p == pos + insert);    /* (a) */
    CODE_ADD(OP_FDIST);     /* FDIST m */
    CODE_ADDI(m);
    CODE_ADD(OP_GOTO);      /* GOTO x */
    CODE_ADDA(px);
    py = CODE_POS;        /* y: */
    CODE_PATCHA(p1, py);
    return;
  }
  {
    /* a{n,m} */
    int p = pos, px, py, p1;
    int insert = 3 + CODE_SZI * 2 + CODE_SZA;
    int c = REContext->regex->ncounters++;
    int k = REContext->regex->nmarks++;
    CODE_INSERT(pos, insert);
    CODE_PATCH(p, OP_ZERO); p++;    /* ZERO c */
    CODE_PATCHI(p, c); p += CODE_SZI;
    px = p;         /* x: GF y */
    CODE_PATCH(p, greedy ? OP_GF : OP_NF); p++;
    p1 = p; p += CODE_SZA;
    CODE_PATCH(p, OP_MARK); p++;    /* MARK k */
    CODE_PATCHI(p, k); p += CODE_SZI;
    ASSERT(p == pos + insert);    /* (a) */
    if (min) {
      CODE_ADD(OP_RDIST);   /* RDIST k,c,n */
      CODE_ADDI(k);
      CODE_ADDI(c);
      CODE_ADDI(min);
    } else {
      CODE_ADD(OP_FDIST);   /* FDIST k */
      CODE_ADDI(k);
    }
    if (max != INFINITY) {
      CODE_ADD(OP_RNEXT);   /* RNEXT c,m,x */
      CODE_ADDI(c);
      CODE_ADDI(max);
      CODE_ADDA(px);
    } else {
      CODE_ADD(OP_MNEXT);   /* MNEXT c,n,x */
      CODE_ADDI(c);
      CODE_ADDI(min);
      CODE_ADDA(px);
    }
    py = CODE_POS;        /* y: */
    if (min) {
      CODE_ADD(OP_REACH);   /* REACH c,n */
      CODE_ADDI(c);
      CODE_ADDI(min);
    }
    CODE_PATCHA(p1, py);
    return;
  }
}


// Parse a simple integer; used for repetition ranges
static int Integer_parse (REContext *REContext) {
  int val;
  int hasdig = 0;
  val = 0;
  while (!ATEOF && NEXT >= '0' && NEXT <= '9') {
    val = 10 * val + (NEXT-'0');
    hasdig = 1;
    SKIP;
  }
  if (!hasdig) SYNTAX_ERROR;
  return val;
}


/*
 * Atom:: la != ^ $     * + ?   )   ] { } |
 *  pattern character la != ^ $ \ . * + ? ( ) [ ] { } |
 *  .
 *  \ AtomEscape
 *  [ CharacterClass ]
 *  ( Disjunction )
 *  ( ?: Disjunction )
 *  ( ?= Disjunction )
 *  ( ?! Disjunction )
 */
static void Atom_parse (REContext *REContext) {
  CharClass *c;
  int i;
  if (NEXT == '(') {
    SKIP;
    if (!ATEOF && NEXT == '?') {    /* (?... */
      SKIP;
      if (!ATEOF && NEXT == ':') {  /* (?:... */
        SKIP;
        Disjunction_parse(REContext);
      } else if (!ATEOF && (NEXT == '=' || NEXT == '!')) {
        int px, p1;     /* (?=... */
        int neg = (NEXT == '!');  /* (?!... */
        SKIP;
        CODE_ADD(neg ? OP_AF : OP_AS);  /* AS x */
        p1 = CODE_POS; CODE_ADDI(0);
        Disjunction_parse(REContext); /* (a) */
        CODE_ADD(OP_SUCCEED);   /* SUCCEED */
        px = CODE_POS;      /* x: */
        CODE_PATCHA(p1, px);
      } else SYNTAX_ERROR;
    } else {        /* (...) */
      i = REContext->regex->ncaptures++;
      CODE_ADD(OP_START);     /* START i */
      CODE_ADDI(i);
      Disjunction_parse(REContext); /* (a) */
      CODE_ADD(OP_END);     /* END i */
      CODE_ADDI(i);
    }
    EXPECT(')');
    return;
  }
  /*
   * All other atoms compile to simple character class matches
   * (or backreferences)
   */
  switch (NEXT) {
    case '\\':
      SKIP;
      if (ATEOF) SYNTAX_ERROR;
      if (NEXT >= '1' && NEXT <= '9') {
        i = 0;
        while (!ATEOF && (NEXT >= '0' && NEXT <= '9')) {
          i = 10 * i + NEXT - '0';
          SKIP;
        }
        CODE_ADD(OP_BACKREF);
        CODE_ADDI(i);
        if (i > REContext->regex->maxref) REContext->regex->maxref = i;
        return;
      }
      c = ClassEscape_parse(REContext);
      break;
    case '[':
      c = CharacterClass_parse(REContext);
      break;
    case '.':
      SKIP;
      c = CC_NEW();
      CC_ADDCHAR(c, 0x000a);
      CC_ADDCHAR(c, 0x000d);
      //CC_ADDCHAR(c, 0x2028);
      //CC_ADDCHAR(c, 0x2029);
      CC_INVERT(c);
      break;
    default:
      c = CC_NEW();
      CC_ADDCHAR(c, Canonicalize(REContext->regex, NEXT));
      SKIP;
      break;
  }
  i = CC_INTERN(c);
  CODE_ADD(OP_CHAR);
  CODE_ADDI(i);
}


static unsigned char HexDigit_parse (REContext *REContext) {
  SEE_unicode_t c;
  if (ATEOF) SYNTAX_ERROR;
  c = NEXT; SKIP;
  if (c >= '0' && c <= '9') return c-'0';
  if (c >= 'a' && c <= 'f') return c-'a'+10;
  if (c >= 'A' && c <= 'F') return c-'A'+10;
  SYNTAX_ERROR;
}


static CharClass *ClassEscape_parse (REContext *REContext) {
  CharClass *c;
  SEE_unicode_t ch, lookahead[3];
  int i;
  // EXPECT('\\'); // backslash already skipped
  c = CC_NEW();
  if (NEXT >= '0' && NEXT <= '9') {
    // \0oo - 3 digit octal escapes
/*EXT:25*/
    if (/*SEE_COMPAT_JS(REContext->eng, >=, JS11) &&*/ NEXT == '0' && LOOKAHEAD(lookahead, 3) >= 2 &&
        lookahead[1] >= '0' && lookahead[1] < '8' && lookahead[2] >= '0' && lookahead[2] < '8') {
      ch = (lookahead[1] - '0') * 8 + (lookahead[2] - '0');
      CC_ADDCHAR(c, ch);
      SKIP;
      SKIP;
      SKIP;
      return c;
    }
    i = 0;
    while (!ATEOF && NEXT >= '0' && NEXT <= '9') {
      i = 10 * i + NEXT - '0';
      SKIP;
    }
    /*
     * 15.10.2.1.9: "Using a backreference inside a ClassAtom
     * causes an error"
     */
    if (i != 0) SYNTAX_ERROR;
    CC_ADDCHAR(c, i);
    return c;
  }
  ch = NEXT;
  SKIP;
  switch (ch) {
    case 'b': CC_ADDCHAR(c, 0x0008); break;
    case 't': CC_ADDCHAR(c, 0x0009); break;
    case 'n': CC_ADDCHAR(c, 0x000a); break;
    case 'v': CC_ADDCHAR(c, 0x000b); break;
    case 'f': CC_ADDCHAR(c, 0x000c); break;
    case 'r': CC_ADDCHAR(c, 0x000d); break;
    case 'D':
    case 'd':
      CC_ADDRANGE(c, '0', '9');
      if (ch == 'D') CC_INVERT(c);
      break;
    case 'W':
    case 'w':
      CC_ADDRANGE(c, 'a', 'z');
      CC_ADDRANGE(c, 'A', 'Z');
      CC_ADDRANGE(c, '0', '9');
      CC_ADDCHAR(c, '_');
      if (ch == 'W') CC_INVERT(c);
      break;
    case 'S':
    case 's':
#if WITH_UNICODE_TABLES
      for (i = 0; i < SEE_unicode_Zscodeslen; i++) CC_ADDCHAR(c, SEE_unicode_Zscodes[i]);
#else
      CC_ADDCHAR(c, 0x0020);
#endif
      if (ch == 'S') CC_INVERT(c);
      break;
    case 'c':
      if (ATEOF) SYNTAX_ERROR;
      ch = NEXT; SKIP;
      if (('a' <= ch && ch <= 'z') ||
          ('A' <= ch && ch <= 'Z')) CC_ADDCHAR(c, ch % 32);
      else SYNTAX_ERROR;
      break;
    case 'u':
    case 'x':
      i = (ch == 'x' ? 2 : 4);
      ch = 0;
      while (i--) ch = (ch << 4) | HexDigit_parse(REContext);
      CC_ADDCHAR(c, ch);
      break;
    default:
      CC_ADDCHAR(c, ch);
      break;
  }
  return c;
}


/*
 * ClassAtom :
 *  '\' ClassEscape
 *  anychar
 */
static CharClass *ClassAtom_parse (REContext *REContext) {
  CharClass *c;
  if (ATEOF) SYNTAX_ERROR;
  if (NEXT == '\\') {
    SKIP;
    return ClassEscape_parse(REContext);
  }
  c = CC_NEW();
  CC_ADDCHAR(c, NEXT);
  SKIP;
  return c;
}


/*
 * Convert the CharClass into a canonicalised version. (SLOW!)
 * !!!Every character in the class has to be individually canonicalized!!!
 * !!!OUCH!!!OUCH!!!I!HATE!YOU!ALL!!!
 */
static CharClass *CanonicalizeClass (REContext *REContext, CharClass *c) {
  CharClass *ccanon;
  CharClassRange *r;
  SEE_unicode_t ch, uch;
  if (cc_count(c) > (uint32_t)~0 / 2) {
    CC_INVERT(c);
    ccanon = CanonicalizeClass(REContext, c);
    CC_INVERT(ccanon);
    return ccanon;
  }
  /*
   * Evil hack: if the CharClass range includes ['A'-0xEFFFF], then
   * there is no need to canonicalize because every uppercase character
   * is already there.
   */
  for (r = c->ranges; r; r = r->next) if (r->lo <= 'A' && r->hi > 0xf0000) return c;
  ccanon = CC_NEW();
  for (r = c->ranges; r; r = r->next) {
    for (ch = r->lo; ch < r->hi; ch++) {
      uch = UNICODE_TOUPPER(ch);
      CC_ADDCHAR(ccanon, uch);
    }
  }
  return ccanon;
}


/*
 * CharacterClass ::
 *  '[' [^] ( ClassAtom | ClassAtom '-' ClassAtom ) * ']'
 */
static CharClass *CharacterClass_parse (REContext *REContext) {
  CharClass *c = CC_NEW(), *a, *b;
  int invertflag = 0;
  EXPECT('[');
  if (!ATEOF && NEXT == '^') {
    invertflag = 1;
    SKIP;
  }
  while (!ATEOF && NEXT != ']') {
    a = ClassAtom_parse(REContext);
    if (!ATEOF && NEXT == '-') {
      SKIP;
      // Treat '-' literally if at end of ClassRanges 15.10.2.16
      if (!ATEOF && NEXT == ']') {
        CC_ADDCHAR(a, '-');
        goto out;
      }
      if (!cc_issingle(a)) SYNTAX_ERROR;
      b = ClassAtom_parse(REContext);
      if (!cc_issingle(b)) SYNTAX_ERROR;
      if (b->ranges->lo < a->ranges->lo) SYNTAX_ERROR;
      a->ranges->hi = b->ranges->hi;
      // free(b)
    }
out:
    CC_ADDCC(c, a);
    // free(a)
  }
  //if (c->ranges == NULL) SYNTAX_ERROR;
  EXPECT(']');
  if (REContext->regex->flags & SEERX_FLAG_IGNORECASE) {
    c = CanonicalizeClass(REContext, c);
    // free(c); // i.e the old c
  }
  if (invertflag) CC_INVERT(c);
  return c;
}


////////////////////////////////////////////////////////////////////////////////
// P-code
//
#ifndef NDEBUG
static int dprint_code (SEE_RegExpr *regex, int addr) {
  int i;
  const char *op="", *opc;
  unsigned char *code = regex->code;
  dprintf("0x%04x: ", addr);
  switch (code[addr]) {
    case OP_FAIL: dprintf("FAIL"); op = ""; break;
    case OP_SUCCEED: dprintf("SUCCEED"); op = ""; break;
    case OP_CHAR: dprintf("CHAR"); op = "i"; break;
    case OP_ZERO: dprintf("ZERO"); op = "i"; break;
    case OP_REACH: dprintf("REACH"); op = "ii"; break;
    case OP_NREACH: dprintf("NREACH"); op = "ii"; break;
    case OP_START: dprintf("START"); op = "i"; break;
    case OP_END: dprintf("END"); op = "i"; break;
    case OP_UNDEF: dprintf("UNDEF"); op = "ii"; break;
    case OP_MARK: dprintf("MARK"); op = "i"; break;
    case OP_FDIST: dprintf("FDIST"); op = "i"; break;
    case OP_RDIST: dprintf("RDIST"); op = "iii"; break;
    case OP_MNEXT: dprintf("MNEXT"); op = "iia"; break;
    case OP_RNEXT: dprintf("RNEXT"); op = "iia"; break;
    case OP_GOTO: dprintf("GOTO"); op = "a"; break;
    case OP_GS: dprintf("GS"); op = "a"; break;
    case OP_NS: dprintf("NS"); op = "a"; break;
    case OP_GF: dprintf("GF"); op = "a"; break;
    case OP_NF: dprintf("NF"); op = "a"; break;
    case OP_AS: dprintf("AS"); op = "a"; break;
    case OP_AF: dprintf("AF"); op = "a"; break;
    case OP_BOL: dprintf("BOL"); op = ""; break;
    case OP_EOL: dprintf("EOL"); op = ""; break;
    case OP_BRK: dprintf("BRK"); op = ""; break;
    case OP_NBRK: dprintf("NBRK"); op = ""; break;
    case OP_BACKREF: dprintf("BACKREF"); op = "i"; break;
    default: dprintf("*** %d", code[addr]); break;
  }
  addr++;
  for (opc = op; *opc; opc++) {
    if (opc != op) dprintf(",");
    dprintf(" ");
    switch (*opc) {
      case 'a':
        i = CODE_MAKEA(code, addr);
        dprintf("0x%04x", i);
        i = CODE_MAKEI(code, addr);
        dprintf(" [0x%04x]", i);
        addr += CODE_SZA;
        break;
      case 'i':
        i = CODE_MAKEI(code, addr); addr += CODE_SZI;
        dprintf("%d", i);
        break;
      case 'c':
        i = CODE_MAKEI(code, addr); addr += CODE_SZI;
        dprintf("%d=", i);
        if (i > regex->cclen) dprintf("**BAD**");
        else dprint_cc(regex->cc[i]);
        break;
    }
  }
  dprintf("\n");
  return addr;
}


static void dprint_regex (SEE_RegExpr *regex) {
  int i, addr;
  CharClassRange *r;
  dprintf("regex %p\n", regex);
  dprintf("\tncaptures = %d\n", regex->ncaptures);
  dprintf("\tcodelen = %d\n", regex->codelen);
  dprintf("\tcclen = %d\n", regex->cclen);
  dprintf("\tflags = 0x%x\n", regex->flags);
  dprintf("\tcc:\n");
  for (i = 0; i < regex->cclen; i++) {
    dprintf("\t\t%d = ", i);
    dprint_cc(regex->cc[i]);
    dprintf("\n\t\t  = { ");
    for (r = regex->cc[i]->ranges; r; r = r->next) dprintf("%x:%x ", r->lo, r->hi);
    dprintf("}\n");
  }
  dprintf("\tcode:\n");
  addr = 0;
  while (addr < regex->codelen) addr = dprint_code(regex, addr);
}
#endif


////////////////////////////////////////////////////////////////////////////////
// pcode-execution
//
/* 15.10.2.8 */
static SEE_unicode_t Canonicalize (SEE_RegExpr *regex, SEE_unicode_t ch) {
  if (regex->flags & SEERX_FLAG_IGNORECASE) return UNICODE_TOUPPER(ch);
  return ch;
}


static SEE_boolean_t pcode_run (SEE_RegExpr *regex, unsigned int addr, const char *text, int textLen, char *state) {
  SEE_boolean_t result;
  int i = 0, i2 = 0, i3 = 0;
  unsigned int a = 0;
  unsigned char op;
  SEE_unicode_t ch;
  Capture *capture;
  int *counter, *mark, statesz;
  unsigned int newaddr;
  char *newstate;
  //if (SEE_system.periodic) (*SEE_system.periodic)(eng);
  // Compute the offsets into the state structure
  statesz = 0;
  capture = (Capture *)state;
  statesz += regex->ncaptures * sizeof (Capture);
  counter = (int *)(state + statesz);
  statesz += regex->ncounters * sizeof (int);
  mark = (int *)(state + statesz);
  statesz += regex->nmarks * sizeof (int);
  SEE_ASSERT(eng, statesz == regex->statesz);
  newstate = calloc(statesz+1, sizeof(char));//SEE_STRING_ALLOCA(eng, char, statesz);
#define index (capture[0].end)
  for (;;) {
    /* Catch bad branches */
    if (addr >= regex->codelen) {
      free(newstate);
      longjmp(regex->errJP, 666);
      //abort();
      //SEE_error_throw_string(eng, eng->Error, STR(internal_error));
    }
    /* Read the opcode and its arguments */
    op = regex->code[addr];
#ifndef NDEBUG
    if (SEE_regex_debug) {
      int x;
      //TJString mys;
      //mys.stringclass = 0;
      //mys.eng = 0;
      //mys.flags = 0;
      dprintf("index=%d captures=[", index);
      for (x = 0; x < regex->ncaptures; x++) {
        if (x) dprintf(",");
        if (capture[x].start == -1)
        dprintf("undef");
        else if (capture[x].start+capture[x].end > textLen) {
          dprintf("bad<%x:%x>",
          capture[x].start, capture[x].end);
        } else {
          int end = capture[x].end;
          if (end == -1) end = index;
          /*
          mys.length = end-capture[x].start;
          mys.data = text+capture[x].start;
          dprintf("\"");
          dprints(&mys);
          dprintf("\"");
          */
          dprints(text+capture[x].start, end-capture[x].start);
          if (capture[x].end == -1) dprintf("+");
        }
      }
      dprintf("]");
      if (op == OP_ZERO || op == OP_REACH || op == OP_NREACH || op == OP_MNEXT || op == OP_RNEXT) {
        dprintf(" counter=[");
        for (x = 0; x < regex->ncounters; x++) {
          if (x) dprintf(",");
          dprintf("%d", counter[x]);
        }
        dprintf("]");
      }
      if (op == OP_MARK || op == OP_FDIST || op == OP_RDIST) {
        dprintf(" mark=[");
        for (x = 0; x < regex->nmarks; x++) {
          if (x) dprintf(",");
          dprintf("%d", mark[x]);
        }
        dprintf("]");
      }
      if (regex->code[addr] == OP_CHAR && index < textLen) dprintf(" ch='%c'", Canonicalize(regex, text[index]));
      dprintf("\n");
      (void)dprint_code(regex, addr);
    }
#endif
    addr++;
    switch (op) {
      case OP_FAIL: case OP_SUCCEED: case OP_BOL:
      case OP_EOL: case OP_BRK: case OP_NBRK:
        break;
      case OP_CHAR: case OP_ZERO: case OP_START:
      case OP_END: case OP_MARK: case OP_FDIST:
      case OP_BACKREF:
        i = CODE_MAKEI(regex->code, addr);  /* integer arg */
        addr += CODE_SZI;
        break;
      case OP_REACH: case OP_NREACH: case OP_UNDEF:
        i = CODE_MAKEI(regex->code, addr);  /* integer arg */
        addr += CODE_SZI;
        i2 = CODE_MAKEI(regex->code, addr); /* integer arg */
        addr += CODE_SZI;
        break;
      case OP_RDIST:
        i = CODE_MAKEI(regex->code, addr);  /* integer arg */
        addr += CODE_SZI;
        i2 = CODE_MAKEI(regex->code, addr); /* integer arg */
        addr += CODE_SZI;
        i3 = CODE_MAKEI(regex->code, addr); /* integer arg */
        addr += CODE_SZI;
        break;
      case OP_MNEXT: case OP_RNEXT:
        i = CODE_MAKEI(regex->code, addr);  /* integer arg */
        addr += CODE_SZI;
        i2 = CODE_MAKEI(regex->code, addr); /* integer arg */
        addr += CODE_SZI;
        a = CODE_MAKEA(regex->code, addr);  /* address arg */
        addr += CODE_SZA;
        break;
      case OP_GOTO: case OP_GS: case OP_NS:
      case OP_GF: case OP_NF: case OP_AS:
      case OP_AF:
        a = CODE_MAKEA(regex->code, addr);  /* address arg */
        addr += CODE_SZA;
        break;
      default:
        /* error */
        //SEE_error_throw_string(eng, eng->Error, STR(internal_error));
        //abort();
        free(newstate);
        longjmp(regex->errJP, 666);
    }

    switch (op) {
      case OP_FAIL:
        free(newstate);
        return 0;
      case OP_SUCCEED:
        free(newstate);
        return 1;
      /* succeed if current character matches CharClass. index++ */
      case OP_CHAR:
        if (index < textLen) {
          ch = text[index++];
          /* N.B. strings are UTF-16 encoded! */
          /*
          if ((ch & 0xfc00) == 0xd800 && index < textLen && (text[index] & 0xfc00) == 0xdc00)
            ch = (((ch & 0x3ff) << 10) | (text[index++] & 0x3ff))+0x10000;
          */
          ch = Canonicalize(regex, ch);
          if (!cc_contains(regex->cc[i], ch)) {
            free(newstate);
            return 0;
          }
        } else {
          free(newstate);
          return 0;
        }
        break;
      /* reset an iteration counter */
      case OP_ZERO: counter[i] = 0;
        break;
      /* fail if we havent reached a particular count */
      case OP_REACH:
        if (counter[i] < i2) {
          free(newstate);
          return 0;
        }
        break;
      /* fail if we reached a particular count */
      case OP_NREACH:
        if (counter[i] >= i2) {
          free(newstate);
          return 0;
        }
        break;
      /* start a capture group at current index */
      case OP_START:
        capture[i].start = index;
        capture[i].end = -1;
        break;
      /* finish a capture group at current index */
      case OP_END:
        capture[i].end = index;
        break;
      /* reset the given captures - usually done at a loop start */
      case OP_UNDEF:
        while (i < i2) {
          capture[i].start = -1;
          capture[i].end = -1;
          i++;
        }
        break;
      /* Set a mark to the current index */
      case OP_MARK:
        mark[i] = index;
        break;
      /* fail if we haven't advanced past the mark */
      case OP_FDIST:
        if (mark[i] == index) {
          free(newstate);
          return 0;
        }
        break;
      /* fail if haven't advanced past mark AND counter has reached a limit */
      case OP_RDIST:
        if (mark[i] == index && counter[i2] >= i3) {
          free(newstate);
          return 0;
        }
        break;
      /* increment counter if it is less than n. always branch */
      case OP_MNEXT:
        if (counter[i] < i2) counter[i]++;
        addr = a;
        break;
      /* increment counter. if it is less than n, then branch */
      case OP_RNEXT:
        counter[i]++;
        if (counter[i] < i2) addr = a;
        break;
      case OP_GOTO:
        addr = a;
        break;
      /* operations that push state and branch for backtracking */
      case OP_GS: /* greedy success */
      case OP_NS: /* non-greedy success */
      case OP_GF: /* greedy fail */
      case OP_NF: /* greedy fail */
      case OP_AS: /* assert success */
      case OP_AF: /* assert fail */
        newaddr = (op == OP_NS || op == OP_NF) ? a : addr;
        memcpy(newstate, state, statesz);
        result = pcode_run(regex, newaddr, text, textLen, newstate);
        if (result) {
          if (op == OP_GF || op == OP_NF) {
            memcpy(state, newstate, statesz);
            free(newstate);
            return 1;
          } else if (op == OP_AF) {
            free(newstate);
            return 0;
          } else if (op == OP_AS) {
            int index_save = index;
            memcpy(state, newstate, statesz);
            index = index_save;
            addr = a;
          } else if (op == OP_GS) {
            addr = a;
          }
        } else {
          if (op == OP_GS || op == OP_NS || op == OP_AS) {
            free(newstate);
            return 0;
          } else if (op == OP_GF || op == OP_AF) addr = a;
        }
        break;
      /* succeed if we are at the beginning of a line */
      /* See 15.10.2.6 */
      case OP_BOL: /* ^ */
        if (index == 0) ; /* succeed */
        else if ((regex->flags & SEERX_FLAG_MULTILINE) == 0) { free(newstate); return 0; }
        else if (text[index-1] == 0x000a || /*LF*/
                 text[index-1] == 0x000d) // || /*CR*/
                 //text[index-1] == 0x2028 || /*LS*/
                 //text[index-1] == 0x2029) /*PS*/
            ; /* succeed */
        else { free(newstate); return 0; }
        break;
      /* succeed if we are at the end of a line */
      case OP_EOL: /* $ */
        if (index == textLen) ; /* succeed */
        else if ((regex->flags & SEERX_FLAG_MULTILINE) == 0) { free(newstate); return 0; }
        else if (text[index] == 0x000a || /*LF*/
                 text[index] == 0x000d) // || /*CR*/
                 //text[index] == 0x2028 || /*LS*/
                 //text[index] == 0x2029) /*PS*/
            ; /* succeed */
        else { free(newstate); return 0; }
        break;
#define IsWordChar(e) ((e) >= 0 && (e) < textLen && ( \
         (text[e] >= 'a' && text[e] <= 'z') \
      || (text[e] >= 'A' && text[e] <= 'Z') \
      || (text[e] >= '0' && text[e] <= '9') \
      || text[e] == '_'))
      /* succeed if we are at a word break */
      case OP_BRK:
      case OP_NBRK: {
        SEE_boolean_t a, b;
        a = IsWordChar(index - 1);
        b = IsWordChar(index);
        if (op == OP_BRK) {
          if (a == b) { free(newstate); return 0; }
        } else {
          if (a != b) { free(newstate); return 0; }
        }
        } break;
      /* succeed if we match a backreference */
      case OP_BACKREF:
        if (!SEERX_IS_CAPTURE_UNDEFINED(capture[i])) {
          int x, len, br;
          br = capture[i].start;
          len = capture[i].end - br;
          if (len+index > textLen) { free(newstate); return 0; }
          for (x = 0; x < len; x++)
            if (Canonicalize(regex, text[br+x]) != Canonicalize(regex, text[index+x])) {
              free(newstate);
              return 0;
            }
          index += len;
        }
        break;
      /* catch unexpected instructions */
      default:
        //SEE_error_throw_string(eng, eng->Error, STR(internal_error));
        //abort();
        free(newstate);
        longjmp(regex->errJP, 666);
      }
  }
}
#undef index


/*
 * Executes the regex on the text beginning at index.
 * Returns true of a match was successful.
 */
int SEE_regex_match (SEE_RegExpr *regex, const char *text, int textLen, int index, Capture *capture_ret) {
  int i, success;
  if (textLen < 0) textLen = text?strlen(text):0;
  char *state = calloc(regex->statesz+1, sizeof(char));//SEE_STRING_ALLOCA(eng, char, regex->statesz);
  Capture *capture = (Capture *)state;
#ifndef NDEBUG
  memset(state, 0xd0, regex->statesz); /* catch bugs */
#endif
  capture[0].start = index;
  capture[0].end = index;
  for (i = 1; i < regex->ncaptures; i++) {
    capture[i].start = -1;
    capture[i].end = -1;
  }
  if (setjmp(regex->errJP)) {
    free(state);
    return 0;
  }
  success = pcode_run(regex, 0, text, textLen, state);
#ifndef NDEBUG
  if (SEE_regex_debug) dprintf(". %s\n", success ? "success" : "failure");
#endif
  if (success && capture_ret) memcpy(capture_ret, capture, regex->ncaptures*sizeof(Capture));
  free(state);
  return success;
}


/*------------------------------------------------------------
 * optimizer
 */
static void optimize_regex (SEE_RegExpr *regex) {
  /*
   * (nothing here yet)
   *
   * possible optimisations include branch short-cuts,
   * and compiling the p-code to native machine instructions.
   */
}


void SEE_regex_free (SEE_RegExpr *regex) {
  if (regex) {
    CharClassRange *r, *n;
    for (n = NULL, r = regex->crList; r; r = n) {
      n = r->prev;
      free(r);
    }
    CharClass *cr, *cn;
    for (cn = NULL, cr = regex->ccList; cr; cr = cn) {
      cn = cr->prev;
      free(cr);
    }
    SEE_grow_deinit(&regex->codegrow);
    SEE_grow_deinit(&regex->ccgrow);
    free(regex);
  }
}


#ifdef __cplusplus
}
#endif
