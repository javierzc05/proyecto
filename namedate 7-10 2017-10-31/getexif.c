/*
 * dumpexif.c
 *
 *  Created on: 12.05.2017
 *      Author: James
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "getexif.h"

#define	INTEL		0x49	//'I'
#define	MOTO		0x4D	//'M'
#define MARK_SOI	0xFFD8	// start of input
#define MARK_EOI	0xFFD9	// end of input
#define MARK_EXIF	0xFFE1	// start of Exif block

#define	TAG_DTMOD		0x0132
#define	TAG_DTORG		0x9003
#define	TAG_DTDIG		0x9004
#define TAG_SUBIFD		0x8769

/*
 * ExifGrabWord - Read a 2-byte (word) value from the buffer, converting the byte order as needed.
 */

unsigned short ExifGrabWord(unsigned char *buf, char fFmt) {
	unsigned short us;
	if (fFmt == INTEL)
		us = *(unsigned short *)buf;
	else
		us = buf[0] << 8 | buf[1];
	return us;
}

/*
 * ExifGrabDWord - Read a 4-byte (double word) value from the buffer, converting the byte order as needed.
 */

unsigned long ExifGrabDWord(unsigned char *buf, char fFmt) {
	if (fFmt == INTEL)
		return *(unsigned long *)buf;
	else
		return ExifGrabWord(buf, fFmt) << 16 | ExifGrabWord(buf+2, fFmt);
}

/*
 * ExifReadWord - Read a 2-byte (word) value from a file stream, converting the byte order as needed.
 */
int	ExifReadWord(FILE *fin, char fFmt, unsigned short *rval) {
	unsigned char buf[4];

	fread(buf, 2, 1, fin);
	*rval = ExifGrabWord(buf, fFmt);
	return *rval;
}

/*
 * ExifCvtDate - Convert the string "YYYY:MM:DD  HH:MM:SS" to a time_t value.
 */

unsigned long ExifCvtDate(time_t *tmRet, char *szDT) {
	char *pCP;
	char *strDelim = " :";
	char *pTok;
	struct tm tmP;
	int n;

	memset(&tmP, 0, sizeof(tmP));	// zero this structure

	pCP = strdup(szDT);			// duplicate the input string

	pTok = strtok(pCP, strDelim);	// get the year
	n = atoi(pTok);
	if (n > 1900)
		tmP.tm_year = n - 1900;

	pTok = strtok(NULL, strDelim);	// get the month
	n = atoi(pTok);
	if (n)
		tmP.tm_mon = n - 1;

	pTok = strtok(NULL, strDelim);	// get the day
	tmP.tm_mday = atoi(pTok);

	pTok = strtok(NULL, strDelim);	// get the hour
	tmP.tm_hour = atoi(pTok);

	pTok = strtok(NULL, strDelim);	// get the minute
	tmP.tm_min = atoi(pTok);

	pTok = strtok(NULL, strDelim);	// get the second
	tmP.tm_sec = atoi(pTok);

	*tmRet = mktime(&tmP);			// convert to time_t

	free(pCP);						// release the duplicate string

#if 0
	printf("ExifCvtDate: '%s' ==> %08lx\n", szDT, (unsigned long)*tmRet);
#endif

	return (unsigned long)*tmRet;	// return to the caller
}

/*
 * ExifParseIFD - Parse an IFD (Image File Directory) in a memory buffer
 */
int ExifParseIFD(unsigned char *bData, unsigned long dwStart, char fFmt, unsigned cbLen, rtnExifDates *pDT) {
	unsigned char *bCur = bData + dwStart;
	unsigned short wTag, wFF, wCnt;
	unsigned long dwN, dwD;
	char szDT[24];
	time_t dtTmp;

#if 0
	printf("++ Entering ExifParseIFD...\n");
#endif

	// grab the number of entries in the directory (2 bytes)
	wCnt = ExifGrabWord(bCur, fFmt);
	bCur += 2;

	while (wCnt--) {
		// grab the directory entry
		wTag = ExifGrabWord(bCur, fFmt);		// tag
		wFF  = ExifGrabWord(bCur+2, fFmt);		// data format
		dwN  = ExifGrabDWord(bCur+4, fFmt);		// number of components
		dwD  = ExifGrabDWord(bCur+8, fFmt);		// data value
		bCur += 12;

		switch (wTag) {
			case TAG_DTMOD :
			case TAG_DTORG :
			case TAG_DTDIG :
				if ((wFF == 2) && (dwN == 20L) && (dwD+dwN < cbLen)) {
					strncpy(szDT, (char *)(bData + dwD), sizeof(szDT));
					if (ExifCvtDate(&dtTmp, szDT)) {
						if (wTag == TAG_DTMOD)
							pDT->tv_mod = dtTmp;
						else if (wTag == TAG_DTDIG)
							pDT->tv_dig = dtTmp;
						else
							pDT->tv_org = dtTmp;
					}
				}
				break;

			case TAG_SUBIFD :
				ExifParseIFD(bData, dwD, fFmt, cbLen, pDT);
				break;
		}
	}

#if 0
	printf("-- Exiting ExifParseIFD...\n");
#endif
	return 0;
}

/*
 * ExifParse - Check if we have an Exif header, parse it if this is the case.
 */

int ExifParse(unsigned char *bData, unsigned cbLen, rtnExifDates *pDT) {
	char fFmt;
	unsigned char *bCur = bData;
	unsigned char *bStart;
	unsigned short wTmp;
	unsigned long dwTmp;

	// Exif header is 6 bytes "Exif\0\0"
	if ((*bCur++ != 0x45) ||
		(*bCur++ != 0x78) ||
		(*bCur++ != 0x69) ||
		(*bCur++ != 0x66) ||
		(*bCur++ != 0x00) ||
		(*bCur++ != 0x00))
		return 0;
	// Exif header verified

	// now grab the TIFF Header
	bStart = bCur;

	wTmp = *(unsigned short *)bCur;
	if (wTmp == 0x4949)			// "II"
		fFmt = INTEL;
	else if (wTmp == 0x4d4d)	//	"MM"
		fFmt = MOTO;
	else
		return 0;
	bCur += 2;	// skip the byte align flag

	if (ExifGrabWord(bCur, fFmt) != 0x2a)
		return 0;
	bCur += 2;

	dwTmp = ExifGrabDWord(bCur, fFmt);
	return ExifParseIFD(bStart, dwTmp, fFmt, cbLen, pDT);
}

/*
 * ExifDates - Entry point for this module - open a named file and read the dates
 */

int ExifDates(char *szIn, rtnExifDates *pDT) {
	FILE *fin;
	unsigned short uwIn, uwSz;
	int floop = 1;

	memset(pDT, 0, sizeof(rtnExifDates));

	fin = fopen(szIn, "rb");
	if (fin == NULL)
		return 0;

	// get the first marker - if it isn't FFD8, we don't have a JPEG file
	ExifReadWord(fin, MOTO, &uwIn);
	if (uwIn != MARK_SOI) {
		fclose(fin);
		return 0;
	}

	// start parsing the IFDs
	while (floop) {
		// get a marker and size
		ExifReadWord(fin, MOTO, &uwIn);
		ExifReadWord(fin, MOTO, &uwSz);

#ifdef X_DEBUG
		printf("Marker: %04x -- Size: %04x\n", uwIn, uwSz);
#endif

		if (uwIn == MARK_EXIF) {		// parse the Exif block
			unsigned char *buf = malloc((long)uwSz);
			unsigned long lrd = fread(buf, 1, uwSz - 2, fin);
			if (lrd == (uwSz - 2))
				floop = ExifParse(buf, uwSz - 2, pDT);
			else {
#ifdef _DEBUG
				printf("UUps: fread() returned %lu instead of %u.\n", lrd, uwSz-2);
#endif
				floop = 0;
			}
			free(buf);
		} else if ((uwIn == MARK_EOI) || ((uwIn >> 8) != 0xFF)) 
			floop = 0;
		else
			floop = !fseek(fin, (long)(uwSz - 2), SEEK_CUR);
	}

	fclose(fin);
	return (pDT->tv_mod != 0);
}

#ifdef XX_DEBUG
int main(int argc, char *argv[]) {
	char *infile;
	int goon, ri;
	rtnExifDates rdt;
	struct tm *pltm;

	if (argc < 2) {
		printf("usage: dumpexif <filename>\n");
		exit(1);
	}

	for (ri = 1; ri < argc; ri++) {

		infile = "sample.jpg"; // argv[ri];

		printf("File: '%s'\n", infile);
		goon = ExifDates(infile, &rdt);

		if (goon) {
			pltm = localtime(&rdt.tv_mod);
			printf("- Modified %s", asctime(pltm));
			pltm = localtime(&rdt.tv_org);
			printf("- Original %s", asctime(pltm));
			pltm = localtime(&rdt.tv_dig);
			printf("- Digitize %s", asctime(pltm));
		} else
			printf("Error: '%s' has not Exif data\n", infile);
	}
}
#endif
