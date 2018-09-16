//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//% NAMEDATE - Rename a file with today's date                              %
//% Written by James Greene, Schliengen, Germany                            %
//%-------------------------------------------------------------------------%
//% NAMEDATE                                                                %
//% Copyright (c) 1999-2009, 2014, 2015, 2016, 2017 James Greene            %
//% NAMEDATE comes with NO WARRANTY to the extent permitted by law.         %
//% You may redistribute copies of NAMEDATE under the terms of the          %
//% GNU General Public License. For more information about these            %
//% matters, see the file named COPYING, or contact the author at           %
//% <james.greene@outlook.com>                                              %
//%                                                                         %
//% Please report any bugs to <james.greene@outlook.com>                    %
//%                                                                         %
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <errno.h>
#include <time.h>
#include <io.h>
#include <direct.h>			// 24.04.2003 - Path support
#include <dirent.h>			// 19.06.2009 - opendir() support
#include <fcntl.h>
#include <locale.h>			// 7.09.2007 - locale support
#include <sys/types.h>
#include <sys/stat.h>
// #define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ndutils.h"
#include "cmdlntool.h"
#include "getexif.h"

#define VER_NUMBER  "7.10"
#define VER_STRING  "Version " VER_NUMBER
#define VER_DATE    __DATE__
#define COPR_TEXT	\
	"Copyright \xb8 1999, 2002-2009, 2014-2017 by James Greene. All rights reserved.\n" \
	"james.greene@outlook.com - www.informatics-consulting.de - ASP Member #06128\n\n"

#define	FMT_TEXT	 0
#define	FMT_DMY		 1
#define	FMT_YMD		 2
#define	FMT_USER	 4		// added 26.06.2003 (version 4.0)
#define	FMT_TEXTHM	 8		// added 27.09.2002
#define	FMT_DMYHM	 9		// added 27.09.2002
#define	FMT_YMDHM	10		// added 27.09.2002
#define	FMT_TIMEBIT	 8		// added 27.09.2002
#define FMT_USEREX	20		// added 02.02.2004 (version 5.0)

#define	FLG_CVTSPC	 1<<0		// added 13.10.2002
#define FLG_YES		 1<<1		// added 02.02.2004 (version 5.0)
#define FLG_FRONT	 1<<2		// added 25.03.2003 (version 2.1)
#define FLG_SYSTEM   1<<3		// added 24.04.2003 (version 3.0)
#define FLG_RENDIR   1<<4		// added 24.04.2003 (version 3.0)
#define FLG_FILETIME 1<<5		// added 11.05.2003 (version 3.1)
#define	FLG_OFFSET   1<<6		// added 11.05.2003 (version 3.1)
#define FLG_COPYFILE 1<<7	// added 12.06.2003 (version 3.2)
#define FLG_ASEXT	 1<<8	// added 30.06.2003 (version 4.0)
#define FLG_QUIET	 1<<9	// added 30.06.2003 (version 4.0)
// #define	FLG_OFFTIME  1<<10	// added  1.07.2003 (version 4.0) no longer needed as of v6.10 (combined with FLG_OFFSET)
#define	FLG_FORCE	 1<<11	// added  1.07.2003 (version 4.0)
#define	FLG_8DOT3	 1<<12	// added  23.10.2003 (version 4.1)
#define	FLG_CPY2DIR	 1<<13	// added  23.10.2003 (version 4.1)
#define	FLG_DYNAMIC  1<<14  // added 02.02.2004 (version 5.0)
#define FLG_LASTEXT	 1<<15	// added 24.05.2007 (version 5.2)
#define FLG_LOCALE	 1<<16	// added 7.09.2007 (version 6.0)
#define FLG_OEMCONV	 1<<17	// added 7.09.2007 (version 6.0)
#define FLG_OVERKILL 1<<18	// added 19.06.2009 (version 6.02)
#define FLG_SIMULATE 1<<19	// added 04.01.2015 (version 6.12)
#define FLG_MOVEFILE 1<<20  // added 05.01.2015 (version 6.13)
#define FLG_UTC      1<<21	// added 07.12.2016 (version 6.15)
#define FLG_FORMDIR	 1<<22	// added 20.12.2016 (version 7.0)
#define FLG_EXIFDATE 1<<23	// added 13.05.2017	(version 7.0)

typedef ULONG FLAGTYPE;		// added 7.09.2007 (version 6.0)

// static	int		nOffset=0;	// added 11.05.2003 (version 3.1)
// static  int		nOfftime=0;	// added 01.07.2003 (version 4.0)

#define	OFFSET_YEAR		0
#define	OFFSET_MONTH 	1
#define OFFSET_DAY		2
#define OFFSET_WEEK		3
#define	OFFSET_COUNT	4

#define	OFFTIME_HOUR	0
#define	OFFTIME_MIN		1
#define OFFTIME_SEC		2
#define	OFFTIME_COUNT	3

static	int		anOffset[OFFSET_COUNT] = {0, 0, 0, 0};			// added 24.06.09 (v6.10) - updated 08.12.16
static	int		anOfftime[OFFTIME_COUNT] = {0, 0, 0};	// added 24.06.09 (v6.10) - updated 08.12.16

static	UINT	MAX_ENTRIES = 256;

#define	MAXFORM	256					// it'll never be this long, but just to be safe...
static	char	strFormat[MAXFORM];	// date format
static	char	szTargetFolder[MAXFORM];	// target directory when /S command is used
static	char	strLocale[MAXFORM];			// country ID for locale
static	char	*pszExe;

#define _countof(a) (sizeof(a)/sizeof(*(a)))	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Help text
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	char *txtGNUDisclaimer[] = {
	"This program is free software: you can redistribute it and/or modify it under",
	"the terms of the GNU General Public License as published by the Free Software",
	"Foundation, either version 3 of the License, or (at your option) any later",
	"version.\n",
	"This program is distributed in the hope that it will be useful, but WITHOUT",
	"ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS",
	"FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n",
	"You should have received a copy of the GNU General Public License",
	"along with this program (COPYING.TXT), or see <http://www.gnu.org/licenses/>."
};

#define	numDisclaim (sizeof(txtGNUDisclaimer) / sizeof (char *))

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static void Version(void) {
	register int ri;
	
	fprintf(stderr, "\nNAMEDATE Version %s (%s at %.5s)\n", VER_NUMBER, VER_DATE, __TIME__);
	fputs(COPR_TEXT, stderr);
	for (ri = 0; ri < numDisclaim; ri++)
		fprintf(stderr, "%s\n", txtGNUDisclaimer[ri]);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static  void    Usage(char *exe) {
	cmdUsage(exe);
	Version();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// FormDate - Convert the user's specification into a strftime() format string
// 30-Oct-2017 - Version 7.10 - Added the "inliteral" variable and the recognition of [ and ] 
//	to enclose literal strings which should be skipped by the date formatter.

static	void	FormDate(const char *pForm, char *szFmt, UINT cbFmt, BOOL fEx) {
	char *szF = szFmt;
	int inliteral = 0;

	while (*pForm && (szF - szFmt < cbFmt-1)) {

		if ((*pForm == '[') && !inliteral) {
			++inliteral;
			++pForm;
			continue;
		} else if ((*pForm == ']') && inliteral) {
			inliteral = 0;
			++pForm;
			continue;
		}

		if (!inliteral) {
			if (strchr("aAbBdHIjmMpSUwWyYzZ%", *pForm))	// all strftime flags EXCEPT %c, %x, and %X
				*szF++ = '%';
			else if (fEx && strchr("CFXc", *pForm))		// special handling for (F)ilename, e(X)t and (C)ounter
				*szF++ = '*';
			else if (*pForm == '\\')					// backslash escapes the next character
				++pForm;
		}

		*szF++ = *pForm++;
	}
	*szF++ = '\0';
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	void	NeoDateTime(time_t now, const char *strFormat, char *szDate, const UINT cbDate, const FLAGTYPE nFlags) {
    struct tm *tmNow;
	
	tmNow = (nFlags & FLG_UTC) ? gmtime(&now) : localtime(&now); 

	strftime(szDate, cbDate, strFormat, tmNow);

	if (nFlags & FLG_OEMCONV) {
		char *sz = strdup(szDate);
		CharToOem(sz, szDate);
		free(sz);
	}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	void	LegacyDate(const UINT nFormat) {
	char szForm[64];

	switch (nFormat & ~FMT_TIMEBIT) {
		case FMT_DMY :
			ndstrcpy_s(szForm, _countof(szForm), "d-m-Y");
			break;
		case FMT_YMD :
			ndstrcpy_s(szForm, _countof(szForm), "Y-m-d");
			break;
		case FMT_TEXT :
			ndstrcpy_s(szForm, _countof(szForm), "d-b-Y");
	}

	if (nFormat & FMT_TIMEBIT)
		ndstrcat_s(szForm, _countof(szForm), "-H-M");

	FormDate(szForm, strFormat, _countof(strFormat), FALSE);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	void	EightDotThreeDate(void) {
	FormDate("ymd", strFormat, _countof(strFormat), FALSE);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	int	exists(const char *strName) {
	int fin;

	fin = _open(strName, _O_RDONLY);
	if (fin == -1)
		return FALSE;
	_close(fin);
	return TRUE;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	int	direxists(const char *strName) {
	DIR *pdin;

	pdin = opendir(strName);
	if (pdin == 0)
		return FALSE;
	closedir(pdin);
	return TRUE;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	int	copyfile(const char *szOldName, const char *szNewName, BOOL fForce) {
#ifdef _WIN32
	if (exists(szNewName) && !fForce) {
		errno = EEXIST;
		return -1;
	}
	
	if(CopyFile(szOldName, szNewName, !fForce))
		return 0;
	else {
		errno = GetLastError();
		return -1;
	}
#else
	unsigned char bTemp[4096];
	int fin, fout, cnt, flg;

	fin = _open( szOldName, _O_RDONLY | _O_SEQUENTIAL | _O_BINARY);
	if( fin == -1 )
		return -1;

	flg = _O_WRONLY | _O_CREAT | _O_SEQUENTIAL | _O_BINARY;
	if (fForce)
		flg |= _O_TRUNC;

	fout = _open( szNewName, flg, _S_IREAD | _S_IWRITE );
	if( fout == -1 ) {
		_close(fin);
		return -1;
	}

	while ((cnt = _read(fin, bTemp, sizeof(bTemp))) > 0) {
		_write(fout, bTemp, cnt);
	}

	_close(fout);
	_close(fin);
	return 0;
#endif
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// removedirectory - uses the Windows shell to remove a directory (including all contents!)
// returns 0 if successful, -1 upon failure with errno set.
// *** this function is, bluntly stated, fucking dangerous. Caveat Emptor. ***

static	int	removedirectory(const char *strRoot) {

	SHFILEOPSTRUCT shfo;
	TCHAR fqpn[MAX_PATH];
//	int ri;

	ndstrcpy_s(fqpn, _countof(fqpn)-6, strRoot);
	ndstrcat_s(fqpn, _countof(fqpn), "\\");
	ndstrcat_s(fqpn, _countof(fqpn), "*.*\0\0");

	shfo.hwnd = GetDesktopWindow();
	shfo.wFunc = FO_DELETE;
	shfo.pFrom = fqpn;
	shfo.pTo = 0;
	shfo.fFlags = FOF_NOCONFIRMATION;
	shfo.lpszProgressTitle = "NameDate";

	(void) SHFileOperation(&shfo);

	return rmdir(strRoot);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NeoReplace - Handle /Z:"" replacements where the F, X and/or C are imbedded

static	void	NeoReplace(PSTR szNewName, UINT cbCnt, PCSTR szDate, PCSTR szGiven, PCSTR pExt, UINT uCount) {
	PSTR pT = szNewName;
	PCSTR pF = szDate;
	int nRest = (int)cbCnt;
	char chCvt[8];

	for (pF = szDate ; *pF ; pF++) {
		if (nRest <= 1)
			break;
		
		if (*pF == '*') {	// signal
			switch (*++pF) {
				case 'F' :	ndstrcpy_s(pT, nRest, szGiven);		break;
				case 'X' :	ndstrcpy_s(pT, nRest, pExt);		break;
				case 'C' :	sprintf(chCvt, "%04u", 1+uCount);	
							ndstrcpy_s(pT, nRest, chCvt); 
							break;
				case 'c' :	sprintf(chCvt, "%02u", 1+uCount);
							ndstrcpy_s(pT, nRest, chCvt); 
							break;
				default  :  *pT = '\0';
			}
			while (*pT)
				++pT;
		} else
			*pT++ = *pF;
		nRest = (int)cbCnt - (int)(pT - szNewName);
	}

	*pT = '\0';
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	char	UserVerify(FLAGTYPE nFlags) {
	char ch;

	if (nFlags & (FLG_YES | FLG_SIMULATE)) {
		if (!(nFlags & FLG_QUIET)) {
			fputs("\n", stderr);
			fflush(stderr);
		}
		return 'Y';
	}

	fputs("? (Yes, No, Quit): ", stderr);
	fflush(stderr);

	do {
		ch = _getch();
		ch = toupper(ch);
	} while (!strchr("YNQ", ch));

	switch (ch) {
		case 'Y' :
			fputs("YES\n", stderr);
			break;

		case 'N' :
			fputs("NO\n", stderr);
			break;

		case 'Q' :
			fputs("QUIT\n", stderr);
			break;
	}

	fflush(stderr);
	return ch;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	int	dorename(const char *szName, const time_t tmDate, const UINT nFormat, const FLAGTYPE nFlags, const UINT uCount) {
	char szGiven[MAX_PATH];
	char *pExt, *nExt, cSpc;
	char szNewName[MAX_PATH];
	char szDate[64];
	char chV;

	ndstrcpy_s(szGiven, _countof(szGiven), szName);

	if ((pExt = strrchr(szGiven, '.')) != 0) {
		*pExt++ = '\0';
		if ((nFlags & FLG_LASTEXT) == 0) {		// added 24.05.2007 ...
			while ((nExt = strrchr(szGiven, '.')) != 0){
				*--pExt = '.';
				*nExt++ = '\0';
				pExt = nExt;
			}
		}
	} else {
		pExt = "";
	}

	cSpc = (nFlags & FLG_CVTSPC) ? '_' : ' ';

	// added 13.10.2002 - support for names without spaces...
	if (nFlags & FLG_CVTSPC) {
		char *cp;

		for (cp = szGiven; *cp; cp++) {
			if (*cp == ' ')
				*cp = '_';
		}
	}

	// +++ 11.05.2003 - now generate the date here
//	MakeDate(tmDate, nFormat, szDate);
	NeoDateTime(tmDate, strFormat, szDate, sizeof(szDate), nFlags);
	// --- 11.05.2003

	// +++ 02.02.2004 - new name generation (when using USEREX format)
	if (nFormat & FMT_USER) {
		if (nFormat == FMT_USEREX)
			NeoReplace(szNewName, _countof(szNewName), szDate, szGiven, pExt, uCount);
		else {
			char szTemp[MAX_PATH];	// added 23. Feb 2004 - /Z:"" wasn't working anymore ...

			if (nFlags & FLG_FRONT)
				sprintf(szTemp, "%s%c*F.*X", szDate, cSpc);		// 8.02.2005 - added the %c and cSpc, 23.11.2006 - corrected the order!
			else if (nFlags & FLG_ASEXT)
				sprintf(szTemp, "*F.%s.*X", szDate);
			else
				sprintf(szTemp, "*F%c%s.*X", cSpc, szDate);		// 8.02.2005 - added the %c and cSpc

			NeoReplace(szNewName, _countof(szNewName), szTemp, szGiven, pExt, uCount);
			// so, that should fix /Z again...
		}
	} else {
		if (nFlags & FLG_8DOT3)
			sprintf(szNewName, "%c%c%s.%s", 'A'+(uCount % 26), 'A'+(uCount / 26), szDate, pExt);
		else if (nFlags & FLG_FRONT)
			sprintf(szNewName, "%s%c%s.%s", szDate, cSpc, szGiven, pExt);
		else if (nFlags & FLG_ASEXT)
			sprintf(szNewName, "%s.%s.%s", szGiven, szDate, pExt);
		else
			sprintf(szNewName, "%s%c%s.%s", szGiven, cSpc, szDate, pExt);
	}

	if (nFlags & FLG_CPY2DIR) {
		char szTemp[MAX_PATH];
		ndstrcpy_s(szTemp, _countof(szTemp)-strlen(szNewName), szTargetFolder);
		ndstrcat_s(szTemp, _countof(szTemp), szNewName);
		ndstrcpy_s(szNewName, _countof(szNewName), szTemp);
	}

	if (((nFlags & (FLG_OVERKILL|FLG_RENDIR)) == (FLG_OVERKILL|FLG_RENDIR)) && direxists(szNewName)) {
		if (!(nFlags & FLG_QUIET)) {
			fprintf(stderr, "REMOVE DIRECTORY '%s'", szNewName);
			fflush(stderr);
		}

		if ((chV = UserVerify(nFlags)) == 'Y')
			if (removedirectory(szNewName))
				fprintf(stderr, "REMOVE DIRECTORY FAILED: %s\n", strerror(errno));

		if (chV == 'Q')
			return -2;
	} else {
		if ((nFlags & (FLG_FORCE|FLG_OVERKILL)) && exists(szNewName)) {
			if (!(nFlags & FLG_QUIET)) {
				fprintf(stderr, "DELETE '%s'", szNewName);
				fflush(stderr);
			}

			if ((chV = UserVerify(nFlags)) == 'Y')
				if (remove(szNewName))
					fprintf(stderr, "REMOVE FAILED: %s\n", strerror(errno));

			if (chV == 'Q')
				return -2;
		}
	}

	if (nFlags & (FLG_COPYFILE | FLG_MOVEFILE)) {
		if (!(nFlags & FLG_QUIET)) {
			fprintf(stderr, "%s '%s' TO '%s'", ((nFlags & FLG_MOVEFILE)?"MOVE":"COPY"), szName, szNewName);
			fflush(stderr);
		}

        if ((chV = UserVerify(nFlags)) == 'Y') {
			if (!(nFlags & FLG_SIMULATE) && copyfile(szName, szNewName, (nFlags & (FLG_FORCE|FLG_OVERKILL)))) {
				fprintf(stderr, "COPY FAILED: %s\n", strerror(errno));
				return errno;
			}
        
            if (nFlags & FLG_MOVEFILE) {    // added 2015-01-05
                if (!(nFlags & FLG_SIMULATE) && remove(szName)) {
                    fprintf(stderr, "REMOVE FAILED: %s\n", strerror(errno));
                    return errno;
                }
            }
        }
	} else {
		if (!(nFlags & FLG_QUIET)) {
			fprintf(stderr, "RENAME '%s' TO '%s'", szName, szNewName);
			fflush(stderr);
		}

		if ((chV = UserVerify(nFlags)) == 'Y')
			if (!(nFlags & FLG_SIMULATE) && rename(szName, szNewName)) {
				fprintf(stderr, "RENAME FAILED: %s\n", strerror(errno));
				return errno;
			}
	}

	return (chV == 'Q') ? -2 : 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ChangeDirectory
//	szName (INPUT) directory name
//	pnCWDrv (OUTPUT) current drive ID
//	pszCWDir (OUTPUT) current working directory
//	nCWDir (INPUT) size of pszCWDir buffer
//	pszFile (OUTPUT) filename at the end of szName (may be NULL)
//	nFile (INPUT) size of pszFile buffer (should be 0 if pszFile = NULL)
//	fSilent (INPUT) flag if info message should be suppressed

static int	ChangeDirectory(const char *szName, int *pnCWDrv, char *pszCWDir, const UINT nCWDir, char *pszFile, const UINT nFile, UINT fSilent) {
	char *psl;		// path separator location
//	UINT ri;		// index

#ifdef _DEBUG
	printf("... entering ChangeDirectory(%s)\n", szName);
#endif
	
	// get current working drive and directory
	*pnCWDrv = _getdrive();
	if (_getcwd(pszCWDir, nCWDir) == NULL)
		*pszCWDir = 0;

	// see if some separator (backslash or colon) is present
	if ((psl = strrchr(szName, '\\')) == NULL)
		psl = strchr(szName, ':');

	// if either a backslash or colon are present, then we need to change to that drive/path
	if (psl) {
		char szPath[MAX_PATH];
		int nDrv = *pnCWDrv;

//		ri = (psl - szName);		// we should thrown an exception if ri > MAX_PATH here!
		ndstrcpy_s(szPath, _countof(szPath), szName);
		if (pszFile)				// file can be null, so we need to check for this.
			ndstrcpy_s(pszFile, nFile, psl+1);

		if (szPath[1] == ':') {
			nDrv = 1 + (toupper(szPath[0]) - 'A');
#ifdef _DEBUG
			printf("... setting current drive to %c:\n", 64+nDrv);
#endif
			if (_chdrive(nDrv))
				perror("_chdrive failed");
		}

		if (strcmp(szPath+1, ":") == 0) 	// handle root directory situation
			ndstrcat_s(szPath, _countof(szPath), "\\");
		else {
			if ((psl = strrchr(szPath, '\\')) != NULL)		// truncate after final backslash
				*++psl = '\0';
		}

#ifdef _DEBUG
		printf("... setting current directory to %s\n", szPath);
#endif
		if (_chdir(szPath)) {
			if (errno == ENOENT) {		// directory not found
				if (pszFile == NULL) {	// we've been called by CreateTargetFolder, so let's try!
					if (_mkdir(szPath) == 0) {
						fprintf(stderr, "Directory %s created!\n", szPath);
						if (_chdir(szPath)) {
							fprintf(stderr, "Can't find directory %s: %s\n", szPath, strerror(errno));
							return 0;
						}
					} else {
						fprintf(stderr, "Error creating directory %s: %s\n", szPath, strerror(errno));
						return 0;
					}
				} else {
					fprintf(stderr, "Can't find directory %s: %s\n", szPath, strerror(errno));
					return 0;
				}
			} else {
				fprintf(stderr, "Can't find directory %s: %s\n", szPath, strerror(errno));
				return 0;
			}
		} else if (!fSilent)
			fprintf(stderr, "Directory set to %s\n", szPath);
	} else if (pszFile != NULL) {	// no path separator found
		ndstrcpy_s(pszFile, nFile,szName);
	}

	return 1;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Date / Time Offset handling
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 * isleapyr - decide if the given year is a leap year.
 */

static	int		isleapyr(const int yr) {
	return (((yr % 4) == 0) && ((yr % 100) || ((yr % 400) == 0))) ? 1 : 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// return the number of days in a given month of a given year

static	int		daysinmo(const int month, const int year) {
	int mdays[13] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int adyr = year + 1900;

	if (month > 11)
		return 0;
	
	return mdays[month] + ((month == 1) ? isleapyr(adyr) : 0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// calculate the number of days between two month/year pairs

static	int		deltadays(const int month, const int year, const int dtmon, const int dtyr) {
	int nDays = 0;
	int yrmo = year*12 + month;
	int delta = dtyr*12 + dtmon;

#ifdef _DEBUG
	fprintf(stderr, "** deltadays: month=%d, year=%d, dtmon=%d, dtyr=%d\n", month, year, dtmon, dtyr);
#endif
	
	if (delta < 0) { // we're returning a negative value
		while (delta++) {
			nDays -= daysinmo(yrmo%12, yrmo/12);
			--yrmo;
#ifdef _DEBUG
	fprintf(stderr, "** deltadays: yrmo=%d, delta=%d, nDays=%d\n", yrmo, delta, nDays);
#endif
		}
	} else {
		while (delta--) {
			nDays += daysinmo(yrmo%12, yrmo/12);
			++yrmo;
#ifdef _DEBUG
	fprintf(stderr, "** deltadays: yrmo=%d, delta=%d, nDays=%d\n", yrmo, delta, nDays);
#endif
		}
	}
	
	return nDays;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// removed a bunch of code here - the C runtime library will ensure that the month/year transitions
// are handled correctly. No need to bust my but doing the logic here.

static	time_t	timestamp(const time_t tOrg, const FLAGTYPE nFlags) {
	time_t tRet = tOrg;
	int dtDays = 0;

	if (nFlags & FLG_OFFSET) {		// offset specified?
			
		if (anOffset[OFFSET_MONTH] || anOffset[OFFSET_YEAR]) {	// we have a month or year offset
			struct tm *gmt;
			gmt = gmtime(&tOrg);
			dtDays = deltadays(gmt->tm_mon, gmt->tm_year, anOffset[OFFSET_MONTH], anOffset[OFFSET_YEAR]);
		}
	
		tRet += anOfftime[OFFTIME_SEC];				// seconds
		tRet += anOfftime[OFFTIME_MIN] * 60;		// minutes
		tRet += anOfftime[OFFTIME_HOUR] * 3600;		// hours
		
		dtDays += anOffset[OFFSET_DAY] + 7*anOffset[OFFSET_WEEK];

		tRet += dtDays * 86400;						// days
	}

	return tRet;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static  int namedate(const UINT nFormat, const FLAGTYPE nFlags, const char *szName, UINT *pnCount) {
	time_t now, ftime;
	long lFind;
	struct _finddata_t fi;
	UINT ri, uCnt=0;
	int nRV;
	int  nDrv;
	char szCWD[MAX_PATH];
//	char szPath[MAX_PATH];
	char szFile[MAX_PATH];
//	char *psl;
	char *bNames;
	time_t *bDate;
	int rv;
//	struct lconv *lc;

	// set the locale, if necessary
//	lc = localeconv();

	if (nFlags & FLG_LOCALE) {
		char *rsz = setlocale(LC_ALL, strLocale);
		if (!(nFlags & FLG_QUIET))
			fprintf(stderr, "Locale set to %s\n", rsz);
	}

	// get the current date and time
	now = time(NULL);

	// ++ 24.11.2006 ++ Path name support moved to ChangeDirectory ++
	ChangeDirectory(szName, &nDrv, szCWD, sizeof(szCWD), szFile, sizeof(szFile), 0);
	// -- 24.11.2006 -- End Path name --

	// if the wildcard option is selected, first collect all of the filenames,
	// then do the rename on each.

#ifdef _DEBUG
	{
		char szTemp[MAX_PATH];
		_getcwd(szTemp, sizeof(szTemp));
		printf("... CWD: %s\n", szTemp);
		
		printf("... szFile = '%s'\n", szFile);
	}
#endif

	if (nFlags & FLG_DYNAMIC) {	// 2015-01-04 -- count how big the directory is...
		UINT nCount = 0;

		lFind = _findfirst(szFile, &fi);
		if (lFind == -1) {
			int err = errno;
			fprintf(stderr, "FAILED: %s: %s\n", szFile, strerror(err));
			return err;
		}
		
		do {
			if ((fi.attrib & _A_SUBDIR) && 
				(!(nFlags & FLG_RENDIR) || ((strcmp(fi.name, ".") == 0) || (strcmp(fi.name, "..") == 0))))
					continue;
		
			if ((fi.attrib & (_A_HIDDEN | _A_SYSTEM)) &&
				(!(nFlags & FLG_SYSTEM))) 
				continue;
			
			nCount++;
		} while (_findnext(lFind, &fi) == 0);
		
		MAX_ENTRIES = max(++nCount, MAX_ENTRIES);

#if 0
		if (!(nFlags & FLG_QUIET)) {
			char szTemp[MAX_PATH];
			_getcwd(szTemp, sizeof(szTemp));
			fprintf(stderr, "DYNAMIC: Allocating space for %u files in '%s'\n", nCount, szTemp);
			fflush(stderr);
		}
#endif
	}
	
	bNames = (char *)calloc(MAX_ENTRIES, MAX_PATH);
	if (!bNames) {
		fprintf(stderr, "FAILED: Unable to allocate memory for %u files (%u Bytes total)\n", MAX_ENTRIES, MAX_ENTRIES*MAX_PATH);
		return -1;
	}
	
	bDate = (time_t *)calloc(MAX_ENTRIES, sizeof(time_t));
	if (!bDate) {
		fprintf(stderr, "FAILED: Unable to allocate memory for %u files (%u Bytes total)\n", MAX_ENTRIES, MAX_ENTRIES*sizeof(time_t));
		free(bNames);
		return -1;
	}

	lFind = _findfirst(szFile, &fi);
	if (lFind == -1) {
		int err = errno;
		fprintf(stderr, "FAILED: %s: %s\n", szFile, strerror(err));
		free(bNames);
		return err;
	}

	do {
		if (fi.attrib & _A_SUBDIR) {
			if (!(nFlags & FLG_RENDIR)) {
#ifdef _DEBUG
				printf("... skipping directory %s\n", fi.name);
#endif
				continue;
			}

			if ((strcmp(fi.name, ".") == 0) || (strcmp(fi.name, "..") == 0)) {
#ifdef _DEBUG
				printf("... skipping directory %s\n", fi.name);
#endif
				continue;
			}
		}

		if ((fi.attrib & (_A_HIDDEN | _A_SYSTEM)) &&
			(!(nFlags & FLG_SYSTEM))) {
#ifdef _DEBUG
			printf("... skipping system / hidden file %s\n", fi.name);
#endif
			continue;
		}

		ndstrcpy_s(bNames+(uCnt * MAX_PATH), MAX_PATH, fi.name);
		if (nFlags & FLG_EXIFDATE) {
			rtnExifDates rtn;
			if (ExifDates(fi.name, &rtn)) {
				if (rtn.tv_org) {
					ftime = rtn.tv_org;
					if (!(nFlags & FLG_QUIET))
						printf("%s: Using Exif OriginalDate value.\n", fi.name);
				} else if (rtn.tv_mod) {
					ftime = rtn.tv_mod;
					if (!(nFlags & FLG_QUIET))
						printf("%s: Using Exif ModifiedDate value.\n", fi.name);
				} else {
					ftime = fi.time_write;
					if (!(nFlags & FLG_QUIET))
						printf("%s: No Exif date information - using last modified date.\n", fi.name);
				}
			} else {
				ftime = fi.time_write;
					if (!(nFlags & FLG_QUIET))
						printf("%s: No Exif data - using last modified date.\n", fi.name);
			}
		} else if (nFlags & FLG_FILETIME)
			ftime = fi.time_write;
		else
			ftime = now;
			
		bDate[uCnt++] = timestamp(ftime, nFlags);
	} while ((uCnt < MAX_ENTRIES) && (_findnext(lFind, &fi) == 0));

	for (ri=rv=0 ; (ri < uCnt) && (rv != -2) ; ri++) 
		rv = dorename(bNames+(ri * MAX_PATH), bDate[ri], nFormat, nFlags, ri);

	free(bDate);
	free(bNames);
	nRV = 0;

	// ++ 24.04.2003 ++ restore the current working directory
	_chdrive(nDrv);
	if (*szCWD)
		_chdir(szCWD);
	// -- 24.04.2003 --

	*pnCount += uCnt;

	return nRV;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static	int		CreateTargetFolder(void) {
	char szRTD[MAX_PATH];
	char szCWD[MAX_PATH];
	int nCWDrv;
	UINT uvar;

	ChangeDirectory(szTargetFolder, &nCWDrv, szCWD, sizeof(szCWD), NULL, 0, 1);

	if (_getcwd(szRTD, sizeof(szRTD)) == NULL)
		*szRTD = 0;

	_chdrive(nCWDrv);
	if (*szCWD)
		_chdir(szCWD);

	ndstrcpy_s(szTargetFolder, sizeof(szTargetFolder), szRTD);

	uvar = strlen(szTargetFolder) - 1;
	if ((szTargetFolder[uvar] != '\\') && (szTargetFolder[uvar] != ':'))
		ndstrcat_s(szTargetFolder, _countof(szTargetFolder), "\\");

	return 1;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// COMMAND LINE PROCESSING
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// for various reasons, the directory name will be generated as soon as /SZ 
// and /S or /SM are defined. This function handles that.
// strIn and strOut can be the same string - this won't cause any problems.

static	void	SZHandler(char *strOut, UINT cbOut, FLAGTYPE nFlags, char *strIn) {
	time_t now; 
	char strTmp[MAXFORM];
	char strTwo[MAXFORM];
	unsigned uvar;
	
#ifdef _DEBUG
	fprintf(stderr, "** Resolving dynamic target folder: '%s'\n", strIn);
#endif
	
	ndstrcpy_s(strTwo, _countof(strTwo), strIn);
	uvar = strlen(strTwo);
	if (uvar > 2) {
		uvar--;
		if ((strTwo[uvar-1] != '\\') && (strTwo[uvar] == '\\'))
			ndstrcat_s(strTwo, _countof(strTwo), "\\");
	}
	
	now = timestamp(time(NULL), nFlags); 
	FormDate(strTwo, strTmp, _countof(strTmp), FALSE);
	NeoDateTime(now, strTmp, strOut, cbOut, nFlags);

#ifdef _DEBUG
	fprintf(stderr, "** Dynamic target folder: '%s'\n", strOut);
#endif
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BOOL	ndProcessCommand(myCLParams *pmclp, int mclpret, FLAGTYPE *pFlags, UINT *pFormat) {
	BOOL bret = TRUE;
	int ntmp;
	unsigned uvar;
	
#ifdef _DEBUG
	fprintf(stderr, "--- processing command: -%c%c\n", pmclp->uCmd % 256, pmclp->uCmd / 256);
	if (pmclp->flgParam & OPT_INT)
		fprintf(stderr, "--- with parameter (int) %ld\n", pmclp->nParam);
	else if (pmclp->flgParam & OPT_REAL)
		fprintf(stderr, "--- with parameter (real) %f\n", pmclp->fParam);
	else if (pmclp->flgParam & OPT_STRING)
		fprintf(stderr, "--- with parameter (string) '%s'\n", pmclp->strParam);
#endif
	
	switch (pmclp->uCmd) {
		case CFG_BUFSZ :	// -B:[n]
			MAX_ENTRIES = max(pmclp->nParam, 256);
			*pFlags &= ~FLG_DYNAMIC;
			break;
			
		case CFG_COPY : 	// -C
			*pFlags |= FLG_COPYFILE;	
			break;
		
		case CFG_RENDIR :	// -D
			*pFlags |= FLG_RENDIR;	
			break;
			
		case CFG_RGNDOS :	// -EE
			*pFlags |= FLG_OEMCONV;
			// fall through 
		case CFG_RGN :		// -E=[rgn]
			if (mclpret > 1) {	// paramter received
				*pFlags |= FLG_LOCALE;
				ndstrcpy_s(strLocale, _countof(strLocale), pmclp->strParam);
			} else 
				*pFlags &= ~FLG_LOCALE;
			break;

		case CFG_PREFIX : 	// -F
			if ((*pFormat & FMT_USEREX) == 0)
				*pFlags |= FLG_FRONT;
			else {
				fprintf(stderr, "-- Depreciated Parameter: -F not compatible with -ZZ\n");
				bret = FALSE;
			}
			break;
		
		case CFG_HIDDEN :	// -H
			*pFlags |= FLG_SYSTEM;	
			break;
			
		case CFG_REPL :		// -K
			*pFlags |= FLG_FORCE;
			break;
			
		case CFT_REPLDIR : 	// -KK
			*pFlags |= FLG_OVERKILL;
			break;
		
		case CFG_LASTDOT:	// -L
			*pFlags |= FLG_LASTEXT;	
			break;

		case CFG_USEMTIME :	// -M
			*pFlags |= FLG_FILETIME;	
			break;

		case CFG_EXIFDATE : // -MX
			*pFlags |= FLG_EXIFDATE;
			break;

		case CFG_UTC :		// -G
			*pFlags |= FLG_UTC;
			break;
			
		case CFG_OFFD :		// -OD[n]
		case CFG_OFFW :		// -OW[n]
		case CFG_OFFM :		// -OM[n]
		case CFG_OFFY :		// -OY[n]
			switch (pmclp->uCmd) {
				case CFG_OFFY : 	uvar = OFFSET_YEAR;		break;
				case CFG_OFFM : 	uvar = OFFSET_MONTH;	break;
				default		  :
				case CFG_OFFD : 	uvar = OFFSET_DAY;		break;
				case CFG_OFFW : 	uvar = OFFSET_WEEK;		break;
			}
			ntmp = (mclpret == 1) ? 1 : pmclp->nParam;		// mclpret==1 means no paramter received, so assume +1
			anOffset[uvar] = ntmp;
			*pFlags |= FLG_OFFSET;
			break;
			
		case CFG_ADJH:		// -AH[n]
		case CFG_ADJM:		// -AM[n]
		case CFG_ADJS:		// -AS[n]
			switch (pmclp->uCmd) {
				case CFG_ADJH : 	uvar = OFFTIME_HOUR;	break;
				case CFG_ADJM : 	uvar = OFFTIME_MIN;		break;
				default		  :
				case CFG_ADJS :		uvar = OFFTIME_SEC;		break;
			}
			ntmp = (mclpret == 1) ? 1 : pmclp->nParam;		// mclpret==1 means no paramter received, so assume +1
			anOfftime[uvar] = ntmp;
			*pFlags |= FLG_OFFSET;
			break;

		case CFG_SIMULATE :	// -P
			*pFlags |= FLG_SIMULATE;	
			break;
			
		case CFG_QUIET :	// -Q
			*pFlags |= FLG_QUIET;		
			break;
			
		case CFG_FOLDFMT :	// -SZ
			if (*pFlags & FLG_CPY2DIR)
				SZHandler(szTargetFolder, _countof(szTargetFolder), *pFlags, szTargetFolder);
			else
				*pFlags |= FLG_FORMDIR;
			break;
			
		case CFG_TGT 	 :	// -S=[dir]
		case CFG_TGTMOVE :	// -SM=[dir]
			if (mclpret == 1) {		// no parameter defined
				fprintf(stderr, "-- Missing Parameter: Directory not specified with /%s command\n", (pmclp->uCmd == CFG_TGT ? "S" : "SM"));
				bret = FALSE;
				break;
			}

			if (*pFlags & FLG_FORMDIR) {
				SZHandler(szTargetFolder, _countof(szTargetFolder), *pFlags, pmclp->strParam);
				*pFlags &= ~FLG_FORMDIR;
			} else 
				ndstrcpy_s(szTargetFolder, _countof(szTargetFolder), pmclp->strParam);
			
			uvar = strlen(szTargetFolder);
			if (uvar > 1) {
				uvar--;
				if ((szTargetFolder[uvar] != '\\') && (szTargetFolder[uvar] != ':'))
					ndstrcat_s(szTargetFolder, _countof(szTargetFolder), "\\");
			}
			if (pmclp->uCmd == CFG_TGTMOVE)
				*pFlags |= FLG_MOVEFILE;
			*pFlags |= FLG_CPY2DIR;
			break;
			
		case CFG_CVTSPC :	// -U
			*pFlags |= FLG_CVTSPC;	
			break;
			
		case CFG_ASSUMEYES :	// -Y
			*pFlags |= FLG_YES;		
			break;
		
		case CFG_FMTDATE :		// -Z=[fmt]
		case CFG_FMTFULLNAME :	// -ZZ=[fmt]
			if (mclpret == 1) {		// no parameter defined
				fprintf(stderr, "-- Missing Parameter: Format not specified with /%s command\n", (pmclp->uCmd == CFG_FMTDATE ? "Z" : "ZZ"));
				bret = FALSE;
				break;
			}
			*pFormat = (pmclp->uCmd == CFG_FMTFULLNAME) ? FMT_USEREX : FMT_USER;
			FormDate(pmclp->strParam, strFormat, _countof(strFormat), (pmclp->uCmd == CFG_FMTFULLNAME));
			break;
			
		case CFG_8DOT3 :		// -8
			if ((*pFormat & FMT_USER) == 0)
				*pFlags |= FLG_8DOT3;
			else {
				fprintf(stderr, "-- Depreciated Parameter: /8 not compatible with /Z\n");
				bret = FALSE;
			}
			break;
			
		case CFG_HELP :		// -?
			Usage(pszExe);
			break;
			
		case CFG_VERSION :	// -V
			Version();
			break;
		
		case CFG_DMY :		// -N
			if ((*pFormat & FMT_USER) == 0) {
				*pFormat &= ~FMT_YMD ; 
				*pFormat |= FMT_DMY;
			} else {
				fprintf(stderr, "-- Depreciated Parameter: /N not compatible with /Z\n");
				bret = FALSE;
			}
			break;
		
		case CFG_YMD :		// -R
			if ((*pFormat & FMT_USER) == 0) {
				*pFormat &= ~FMT_DMY; 
				*pFormat |= FMT_YMD;
			} else {
				fprintf(stderr, "-- Depreciated Parameter: /R not compatible with /Z\n");
				bret = FALSE;
			}
			break;
			
		case CFG_ADDTIME :	// -T
			if ((*pFormat & FMT_USER) == 0)
				*pFormat |= FMT_TIMEBIT;
			else {
				fprintf(stderr, "-- Depreciated Parameter: /T not compatible with /Z\n");
				bret = FALSE;
			}
			break;
			
		case CFG_ASEXT :	// -X
			if ((*pFormat & FMT_USEREX) == 0)
				*pFlags |= FLG_ASEXT;
			else {
				fprintf(stderr, "-- Depreciated Parameter: /X not compatible with /ZZ\n");
				bret = FALSE;
			}
			break;
			
		case CFG_STOPCLP :	// --
			pmclp->flgStopCLP = 1;
			break;
	}
	
	return bret;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

UINT		ndRenameThis(myCLParams *pmclp, FLAGTYPE nFlags, UINT nFormat) {
	UINT nCount = 0;
	
	if (!(nFormat & FMT_USER))
		LegacyDate(nFormat);

	if (nFlags & FLG_8DOT3)	// override /U if /8 is specified
		EightDotThreeDate();
		
	if (nFlags & FLG_CPY2DIR)	// make the target directory a full path, and create the directory if needed.
		CreateTargetFolder();

    namedate(nFormat, nFlags, pmclp->strParam, &nCount);
    
    return nCount;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	ncCommandFile - Open and process a command file (@filename or @"filename") 

UINT	ndCommandFile(myCLParams *pmclp, int nDepth, FLAGTYPE *pFlags, UINT *pFormat) {
	// open and read the command file
	// we'll need to strtok/grep each line in case multiple commands are included
	// and be concious of whitespaces like --command = "something readable"
	// while also processing stuff like --command = one --command2 file1 file2
	// most of the work is handled in nexttoken() in the ndutils file

	FILE *fin;
	char strIn[MAXFORM];
	char strCF[MAXFORM];
	char *nxt, *last;
	int rval;
	UINT uCount = 0;
	
	ndstrcpy_s(strCF, MAXFORM, pmclp->strParam);
	
	if (nDepth > 2) {		// don't allow more than 2 levels of recursion
		fprintf(stderr, "** Command file '%s' not processed. Too many levels of recursion. **\n", strCF);
		return 0;
	}
	
	fin = fopen(strCF, "r");
	if (fin == NULL) {
		fprintf(stderr, "** Command file '%s' could not be opened: %s **\n", strCF, strerror(errno));
		return 0;
	}
	
	if (!(*pFlags & FLG_QUIET))
		fprintf(stdout, "NAMEDATE: Processing command file '%s'.\n", strCF);
	
	while (fgets(strIn, MAXFORM, fin)) {
		nxt = nexttoken(strIn, FALSE, &last);

		while (nxt != NULL) {
			rval = cmdltInterpretOne(nxt, pmclp);
			if (rval == -1)
				fprintf(stderr, "** Parameter %s not recognized **\n", nxt);			
			else if ((pmclp->uCmd == CFG_CFGFILE) && (rval > 1)) {
				unsigned holdFlag = pmclp->flgStopCLP;
				ndCommandFile(pmclp, nDepth+1, pFlags, pFormat);
				pmclp->flgStopCLP = holdFlag;
			} else if (pmclp->uCmd != CFG_ORIGFILE)
				ndProcessCommand(pmclp, rval, pFlags, pFormat);
			else
				uCount += ndRenameThis(pmclp, *pFlags, *pFormat);
			
			nxt = nexttoken(NULL, FALSE, &last);
		}
	}
	
	fclose(fin);
	
	if (!(*pFlags & FLG_QUIET))
		fprintf(stdout, "NAMEDATE: Finished processing command file '%s'.\n", strCF);

	return uCount;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

UINT	ndCommandLine(int argc, char *argv[]) {
	int rval;
//	unsigned uvar;
	UINT nFormat = FMT_TEXT;
	FLAGTYPE nFlags = FLG_DYNAMIC;		// new in 7.0 - default value
	myCLParams clparam;
	UINT uCount = 0;
	
	// zero out the structure
	memset(&clparam, 0, sizeof(clparam));
	
	while (--argc) {
		rval = cmdltInterpretOne(*++argv, &clparam);

		if (rval == -1) 
			fprintf(stderr, "** Parameter %s not recognized **\n", *argv);			
		else if ((clparam.uCmd == CFG_CFGFILE) && (rval > 1)) 
			uCount += ndCommandFile(&clparam, 0, &nFlags, &nFormat);
		else if (clparam.uCmd != CFG_ORIGFILE)
			ndProcessCommand(&clparam, rval, &nFlags, &nFormat);
		else
			uCount += ndRenameThis(&clparam, nFlags, nFormat);
	}
	
	if (!(nFlags & FLG_QUIET)) {
		fputs("NAMEDATE: Finished.", stdout); 
		if (uCount)
			fprintf(stdout, " %u file%s processed.", uCount, (uCount != 1 ? "s" : ""));
		fputs("\n", stdout);
	}
	
	return uCount;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int main(int argc, char *argv[]) {
//	char *ap, *exe, ch;
//	UINT nFormat = FMT_TEXT;
//	FLAGTYPE nFlags = 0;
//	BOOL flagTime = FALSE;
//	char szOffset[128], *pOffset, chQ;
//	UINT uvar;
	UINT uCount;

	printf("%-62s %14s\n", "NAMEDATE: Add a date to a given filename", VER_STRING);
	fputs(COPR_TEXT, stdout);

	if (argc < 2) {
		Usage(*argv);
		return 1;
	}

	pszExe = *argv;
	
	uCount = ndCommandLine(argc, argv);
	
	return uCount;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
