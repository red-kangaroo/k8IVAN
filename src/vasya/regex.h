/* Copyright (c) 2003, David Leonard. All rights reserved. */
/* $Id: regex.h 1008 2006-02-05 13:01:44Z d $ */
#ifndef _SEE_h_regex_
#define _SEE_h_regex_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Regex  Regex;

typedef struct {
  int start, end;
} Capture;


#define TJSRX_IS_CAPTURE_UNDEFINED(c)  ((c).end == -1)

#define TJSRX_FLAG_GLOBAL      0x01  /* 'g'-flag */
#define TJSRX_FLAG_IGNORECASE  0x02  /* 'i'-flag */
#define TJSRX_FLAG_MULTILINE   0x04  /* 'm'-flag */


extern Regex *SEE_regex_parse (const char *source, int patLen, int flags);
extern int SEE_regex_count_captures (const Regex *regex);
extern int SEE_regex_get_flags (const Regex *regex);
extern int SEE_regex_match (Regex *regex, const char *text, int textLen, int start, Capture *captures);

extern void SEE_regex_free (Regex *regex);


#ifdef __cplusplus
}
#endif
#endif
