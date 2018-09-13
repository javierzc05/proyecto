//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//% ndutils.h - Utility functions for NameDate                              %
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

#ifndef NDUTILS_H

#define NDUTILS_H

#define MAX_STRLEN	256		// size of the static buffer used by nexttoken()

int		ndstrcpy_s(char *szOut, size_t nCnt, const char *szIn);
int		ndstrcat_s(char *szOut, size_t nCnt, const char *szIn);
char 	*ndstrtok_s(char *s, const char *delim, char **last);
char 	*nexttoken(char *s, int reentry, char **last);

#endif // NDUTILS_H
