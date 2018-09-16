//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//% ndutils.c - Utility functions for NameDate                              %
//% Written by James Greene, Schliengen, Germany                            %
//%-------------------------------------------------------------------------%
//% Copyright (c) 2016 James Greene                                         %
//% NAMEDATE comes with NO WARRANTY to the extent permitted by law.         %
//% You may redistribute copies of NAMEDATE under the terms of the          %
//% GNU General Public License. For more information about these            %
//% matters, see the file named COPYING, or contact the author at           %
//% <james.greene@outlook.com>                                              %
//%                                                                         %
//% Please report any bugs to <james.greene@outlook.com>                    %
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef _DEBUG
#include <stdio.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "ndutils.h"

#ifndef TRUE
#define	TRUE	!0
#endif

#ifdef _WIN32
extern int strcpy_s(char *so, size_t size, const char *si);
extern int strcat_s(char *szOut, size_t nCnt, const char *szIn);
extern int sprintf_s(char *so, size_t size, const char *fmt, ...);
#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define	ndstrlen(S)		strlen(S)

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int		ndstrcpy_s(char *szOut, size_t nCnt, const char *szIn) {
#ifdef _WIN32
	return strcpy_s(szOut, nCnt, szIn);
#else
	if (szOut == 0)
		return EINVAL;

	if (szIn == 0) {
		*szOut = '\0';
		return EINVAL;
	}
	
	if ((nCnt == 0) || (nCnt < ndstrlen(szIn))) {
		*szOut = '\0';
		return ERANGE;
	}
	
	while ((*szOut++ = *szIn++) != 0)
		;
	*szOut = '\0';
	return 0;
#endif
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int		ndstrcat_s(char *szOut, size_t nCnt, const char *szIn) {
#ifdef _WIN32
	return strcat_s(szOut, nCnt, szIn);
#else
	char *szCat;
	
	if (szOut == 0)
		return EINVAL;

	if (szIn == 0) {
		*szOut = '\0';
		return EINVAL;
	}
	
	if ((nCnt == 0) || (nCnt < (ndstrlen(szOut) + ndstrlen(szIn)))) {
		*szOut = '\0';
		return ERANGE;
	}
	
	szCat = szOut + ndstrlen(szOut);
	while ((*szCat++ = *szIn++) != 0)
		;
	*szCat = '\0';
	return 0;
#endif
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

char *ndstrtok_s(char *s, const char *delim, char **last) {
	char *spanp;
	char c, sc;
	char *tok;

	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
	do {
		// emulating a strstr(delim, c) here, where c is the next character of the string
		// goal is to consume all of the leading delimiters
		c = *s++;
		if (c == 0) {		// if c is null, we've hit the end of the string
			*last = NULL;
			return NULL;
		}
		
		for (spanp = (char *)delim; (sc = *spanp++) != 0; ) 
			if (c == sc)
				break;
		
	} while (c == sc);		// found a match in the delimiter list, so loop
		
	tok = s - 1;			// otherwise, tok is the first non-delimiter character

	do {
		//	now we repeat the above process, looping until we hit a delimiter or null again
		
		c = *s++;
		if (c == 0) {
			*last = --s;
			return tok;
		}
		
		spanp = (char *)delim;
		for (spanp = (char *)delim; (sc = *spanp++) != 0; ) {
			if (c == sc) {
				*last = s; 
				*--s = 0;
				return tok;
			}
		}
	} while (c != sc);
	
	// we should never reach here
	*last = NULL;
	return NULL;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// tokenize the string, like above, with the following additional functionality
// 	1) if we see a string enclosed in double-quotes, take it as a literal
//	2) when we finish chewing a token, look at the last character or ahead to the next token. If either is : or =, then 
//		a) append it to the first token (removing whitespaces) if applicable
//		b) grab the following token and append it as well
//	Goal is:
//		token = token 			--> one token: token=token
//		token= "token token"	--> one token: token="token token"
//		"token token"			--> one token: "token token"
//		token token = token		--> two tokens: token & token=token
//	Lines beginning with # are ignored (comments)

char *nexttoken(char *s, int reentry, char **last) {
	char *cur;
	char *white = " \t\r\n";
	char *spw, *stl;
	char c, sc;
	static char strRet[MAX_STRLEN];
	
	// we're at the end of the string, so just return null
	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	// first, scan the string for the first non-delimter character.
	// if it's a double-quote, handle that by searching forward for the matching 
	// closing quote, then return the whole string. Since commands are not quote-enclosed,
	// we don't need to look ahead, so we can just return from here.

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
	do {
		// emulating a strstr(delim, c) here, where c is the next character of the string
		// goal is to consume all of the leading delimiters
		c = *s++;
		if (c == 0) {		// if c is null, we've hit the end of the string
			*last = NULL;
			return NULL;
		}
		
		for (spw = white; (sc = *spw++) != 0; ) 
			if (c == sc)
				break;
		
	} while (c == sc);		// found a match in the delimiter list, so loop
	
	--s;					// now s is pointing to the first non-whitespace
	
	if (*s == '#')	{		// comment line - ignore
		*last = NULL;
		return NULL;
	}
	
	if (*s == '"') {		// bite off a whole quoted string
		cur = s;
		do {
			c = *++cur;
		} while (c && (c != *s));
	
		if (c)
			*++cur = 0;
		
		*last = ++cur;
		return s;
	}

	// otherwise, grab the first token
	cur = ndstrtok_s(s, white, last);
	
	if ((cur == NULL) || reentry) 		// nothing more to do - either we're in re-entry mode or
		return cur;						// there's nothing more to parse in the string
	
	if ((spw = strchr(cur, '=')) != NULL || (spw = strchr(cur, ':')) != NULL) {	// our token contains a : or =
		if (spw[1]) 		// the = or : is in the middle of the token somewhere
			return cur;		// so we have a complete command line parameter to return
		
		// if we get here, then our token ENDS with : or =, so we need to grab the next one as well
		spw = nexttoken(NULL, TRUE, &stl);
		// spw should never be NULL, but you never know...
		
		if (spw) {
			ndstrcpy_s(strRet, MAX_STRLEN, cur);
			ndstrcat_s(strRet, MAX_STRLEN, spw);
			*last = stl;
			return strRet;
		}
		
		// if spw returned null, just act like it didn't happen 
		return cur;
	}
	
	// ok, so the current token didn't contain an = or :, look ahead if the next one starts with : or =
	
	stl = *last;
	do {
		// emulating a strstr(delim, c) here, where c is the next character of the string
		// goal is to consume all of the leading delimiters
		c = *stl++;
		if (c) {		// if c is null, we've hit the end of the string
			for (spw = white; (sc = *spw++) != 0; ) 
				if (c == sc)
					break;
		}
		
	} while (c && (c == sc));		// found a match in the delimiter list, so loop
	
	--stl;					// now s is pointing to the first non-whitespace
	
	if ((*stl != '=') && (*stl != ':'))	{		// no : or = sign, so we're done.
		*last = stl;
		return cur;
	}
	
	// if we get here, we've found an : or = sign, so we need to pull up the parameter
	c = *stl++;
	spw = nexttoken(NULL, TRUE, &stl);
	if (spw) {
		sprintf_s(strRet, MAX_STRLEN, "%s%c%s", cur, c, spw);
		*last = stl;
		return strRet;
	}
	
	// getting here means that there was no : or =, or nothing more after it, so we'll just act normal
	return cur;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
