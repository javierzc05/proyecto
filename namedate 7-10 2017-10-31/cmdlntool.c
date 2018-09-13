/*
 * cmdlntool.c
 *
 *  Created on: 17.12.2016
 *      Author: James Greene
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "cmdlntool.h"		// self-reference

extern int	strcpy_s(char *, size_t, const char *);
extern int	strcat_s(char *, size_t, const char *);

//***************************************************************************
// Command Line Options for NAMEDATE
//***************************************************************************

myCLOps myclops[] = {									//  "----.----1----.----2----.----3----.----4-!"
		{48, 	"OriginalFile", 4, OPT_STRING|OPT_HIDDEN, 	"Source filename"},	// -0
		{64, 	"ConfigFile", 	4, OPT_STRING, 				"Read configuration options from a file"},	// -@
		{66, 	"BufferSize", 	3, OPT_INT, 				"Maximum <n> files per folder"},	// -B
		{17474, "AutoBuffer", 	2, OPT_NONE, 				"(maintained for backward compatibility)"},		// -BD
		{67, 	"CopyFiles", 	3, OPT_NONE, 				"Copy (instead of renaming) files"},	// -C
		{68, 	"RenameFolders", 3, OPT_NONE, 				"Rename folders as well as files"},	// -D
		{69, 	"Region", 		3, OPT_STRING, 				"Set the region/language"},			// -E
		{17733, "RegionASCII", 	7, OPT_STRING, 				"Set the region/language (ASCII)"},	// -EE
		{70, 	"PrefixDate", 	3, OPT_NONE, 				"Put the date before the original filename"},	// -F
		{72, 	"HiddenFiles", 	3, OPT_NONE, 				"Include hidden and system files"},	// -H
		{75, 	"OverwriteFiles", 4, OPT_NONE, 				"Replace (overwrite) existing files"},	// -K
		{19275, "ReplaceFolders", 3, OPT_NONE, 				"Replace (overwrite) files and folders"},	// -KK
		{76, 	"LastDot", 		1, OPT_NONE, 				"Use the LAST dot for the extension"},	// -L
		{77, 	"ModifyTime", 	3, OPT_NONE, 				"Use the file modification time"},	// -M
		{17741, "Exif", 		3, OPT_NONE, 				"Use the Exif date/time if present"},	// -ME
		{71, 	"UTC", 			3, OPT_NONE, 				"Use UTC instead of the local timezone"},	// -G
		{17487, "OffsetDays", 	7, OPT_INT, 				"Adjust the date by <n> days"},	// -OD
		{22351, "OffsetWeeks", 	7, OPT_INT, 				"Adjust the date by <n> weeks"},	// -OW
		{19791, "OffsetMonths", 7, OPT_INT, 				"Adjust the date by <n> months"},	// -OM
		{22863, "OffsetYears", 	7, OPT_INT, 				"Adjust the date by <n> years"},	// -OY
		{18497, "AdjustHours", 	7, OPT_INT, 				"Adjust the time by <n> hours"},	// -AH
		{19777, "AdjustMinutes", 7, OPT_INT, 				"Adjust the time by <n> minutes"},	// -AM
		{21313, "AdjustSeconds", 7, OPT_INT, 				"Adjust the time by <n> seconds"},	// -AS
		{80, 	"Simulate", 	3, OPT_NONE, 				"Simulate (doesn't rename the files)"},	// -P
		{81, 	"Quiet", 		2, OPT_NONE, 				"Quiet mode"},	// -Q
		{19795, "MoveToTarget", 4, OPT_STRING, 				"Move the files to the target folder"},	// -SM
		{23123, "FolderFormat", 3, OPT_NONE, 				"Specifies that the folder name is dynamic"},	// -SZ
		{83, 	"TargetFolder", 3, OPT_STRING,  			"Specify the target folder"},	// -S
		{85, 	"ConvertSpaces", 4, OPT_NONE,  				"Convert spaces to underscores"},	// -U
		{89, 	"Yes", 			3, OPT_NONE,  				"Answer YES to all questions"},	// -Y
		{90, 	"FormatDate", 	4, OPT_STRING,  			"Specify the date/time format"},	// -Z
		{23130, "NameFormat", 	4, OPT_STRING,  			"Specify the filename format"},	// -ZZ
		{56, 	"DOSName", 		3, OPT_NONE,  				"Use DOS (8.3) filename"},	// -8
		{63, 	"Help", 		1, OPT_NONE,  				"Display this help text"},	// -?
		{86, 	"Version", 		3, OPT_NONE,  				"Display the version of NameDate"},	// -V
		{78, 	"DateDMY", 		5, OPT_NONE,  				"Use the DD-MM-YYYY date format (L)"},	// -N
		{82, 	"DateYMD", 		5, OPT_NONE,  				"Use the YYYY-MM-DD date format (L)"},	// -R
		{84, 	"AppendTime", 	3, OPT_NONE,  				"Add the time to the filename (L)"},	// -T
		{88, 	"DateAsExt", 	5, OPT_NONE,  				"Add the date as a file extension (L)"},	// -X
		{45,  	"", 			0, OPT_NONE,  				"Stop processing command line options"}	// --
};

unsigned NUM_MYCLOPS = (sizeof(myclops)/sizeof(myCLOps));


//***************************************************************************

/*
 * Helper function that interprets/tokenizes the long commands into short ones
 * Entry Point:
 * 	char *arg (input) points to the first character after the -- or //
 * 	myCLParams *clparams (output) is returned with the uCmd and flgParams fields filled (when the command is found)
 * 	Return value: char * returns the new value for arg - the first character after the command
 */
static	char 	*cmdltMapLongToShort(char *arg, myCLParams *clparam) {
	char cmdbuf[MAX_LONGCMD];
	unsigned lv;
	unsigned ri;

	lv = 0;
	for (lv = 0; *arg && (strchr(":=+-", *arg) == 0L) && (lv < MAX_LONGCMD-1); arg++)				// eat up to the : or = sign
		cmdbuf[lv++] = toupper(*arg);
	cmdbuf[lv] = 0;
	
	// lv = the length of the command given by the user
	// cmdbuf is the uppercase text
	
	if (lv == 0) {		// special case - we see "--"
		ri = NUM_MYCLOPS - 1;	// point to the special case
		clparam->uCmd = myclops[ri].uCmd;
		clparam->flgParam = myclops[ri].flgParam;
		clparam->ixCLOpts = ri;
		clparam->flgStopCLP = 1;
		return arg;
	}

	// now, search myclops[] for a matching long command and return when found

	for (ri = 0; (ri < NUM_MYCLOPS-1); ++ri) {
		if ((lv >= myclops[ri].uMinLen) && (strncasecmp(myclops[ri].strLong, cmdbuf, lv) == 0)) {
    		clparam->uCmd = myclops[ri].uCmd;
    		clparam->flgParam = myclops[ri].flgParam;
    		clparam->ixCLOpts = ri;
    		return arg;
		}
	}

	// if we're here, it means that no matching command was found, so return an error condition
	return 0L;
}

//***************************************************************************

/*
 * Function cmdltInterpretOne - Parse a single command line item
 * 	const char *clarg (input) - the command line argument
 * 	myCLParams *clparam (output) - the command and parameter, tokenized
 * Return value (int) =
 * 		-1 command not recognized (error condition)
 * 		1 = command alone (no parameter)
 * 		2 = command and parameter
 */

int		cmdltInterpretOne(const char *clarg, myCLParams *clparam) {
	unsigned ri;
	char *arg = (char *)clarg;
	char cmdbuf[MAX_PARAM];
	unsigned short uval;
	char chq;
	int haspar = 0;
	
	clparam->nParam = 0;
	clparam->fParam = 0.0;
	*clparam->strParam = 0;

    if (*arg == '\'')		// workaround for my debugger, which encloses command line arguments in single quotes
    	++arg;

    if (((*arg == '/') || (*arg == '-')) && !clparam->flgStopCLP) {   		// short form
    	++arg;
        if ((*arg == '/') || (*arg == '-')) {   	// long form
        	arg = cmdltMapLongToShort(++arg, clparam);
        	if (arg == 0L)	// command not found
        		return -1;							// return error condition to caller
        } else {	// deal with the short form here
        	uval = toupper(*arg);
        	++arg;
        	if (isalpha(*arg)) {
        		uval += toupper(*arg) << 8;
        		++arg;
        	}

        	// find the respective entry in the myclops array
        	for (ri = 0; (ri < NUM_MYCLOPS) && (myclops[ri].uCmd != uval); ++ri)
        		;

        	// if we have a match, then transfer to clparam and deal with any parameters
        	if (myclops[ri].uCmd == uval) {
        		clparam->uCmd = uval;
        		clparam->flgParam = myclops[ri].flgParam;
        		clparam->ixCLOpts = ri;
        	} else {
				clparam->uCmd = CFG_ERROR;
        		clparam->flgParam = OPT_NONE;
				clparam->ixCLOpts = NUM_MYCLOPS - 1;
        		return -1;
			}
        }
    } else if ((*arg == '@') && !clparam->flgStopCLP) {
    	++arg;
    	ri = CLOPT_CMDFILE;
    	clparam->uCmd = myclops[ri].uCmd;
    	clparam->flgParam = myclops[ri].flgParam;
		clparam->ixCLOpts = ri;
    } else if ((*arg == '#') && !clparam->flgStopCLP) {		// if we hit a #, assume the rest of the line is a comment
		clparam->uCmd = CFG_ERROR;
		clparam->flgParam = OPT_NONE;
		clparam->ixCLOpts = NUM_MYCLOPS - 1;
		return -1;
    } else {    	
    	// deal with a non-escaped parameter, e.g. a filename
    	ri = CLOPT_FILE;
    	clparam->uCmd = myclops[ri].uCmd;
    	clparam->flgParam = myclops[ri].flgParam;
		clparam->ixCLOpts = ri;
    }

    // at this point, arg points to the first character AFTER the command
	
	haspar = 1;

	switch (clparam->flgParam & OPT_TYPMASK) {
		case OPT_INT :
		case OPT_REAL :
			ri = 0;
			while (*arg && (strchr(":=", *arg) != 0L))				// eat the : or = sign
				++arg;
																	// copy all numeric characters to cmdbuf
			while (*arg && (strchr("+-0123456789.e", *arg) != 0L) && (ri < MAX_PARAM-1))
				cmdbuf[ri++] = *arg++;
			cmdbuf[ri] = 0;											// null-terminate the buffer

			if (clparam->flgParam & OPT_INT)
				clparam->nParam = strtol(cmdbuf, &arg, 10);			// convert to an integer (base 10)
			else
				clparam->fParam = strtof(cmdbuf, &arg);				// convert to a float

			if ((arg != cmdbuf) && (errno != ERANGE))
				haspar = 2;											// signal we have processed a parameter
			else if (!(clparam->flgParam & OPT_NOTREQ))
				haspar = -1;										// signal that a required parameter was missing
			break;

		case OPT_STRING :
			ri = 0;
			while (*arg && (strchr(":=", *arg) != 0L))				// eat the : or = sign
				++arg;

			chq = 0;
			if (strchr("\"'", *arg) != 0)							// if the argument is quoted, note this
				chq = *arg++;

			while (*arg && (*arg != chq) && (ri < MAX_PARAM-1))	// copy to cmdbuf
				cmdbuf[ri++] = *arg++;
			cmdbuf[ri] = 0;											// and null-terminate it

			strcpy_s(clparam->strParam, MAX_PARAM, cmdbuf);			// copy to the clparam structure

			if (ri > 0)
				haspar = 2;											// signal we have processed a parameter
			else if (!(clparam->flgParam & OPT_NOTREQ))
				haspar = -1;										// signal that a required parameter was missing
			break;
	}

	return haspar;
}

//***************************************************************************

void	cmdUsage(char *exe) {
	int ri;
	char szShort[4] = {0, 0, 0, 0};
	char *szPar;
	unsigned flg;
	myCLOps *pcl;
	char strOne[16], strTwo[24];
	
    fprintf(stderr, "Usage: %s [switches] filename(s)\n\n", exe);
    fprintf(stderr, "where [switches] may include\n");

    for (ri = 0, pcl = myclops ; (ri < NUM_MYCLOPS) ; ++ri, ++pcl) {
		if (pcl->flgParam & OPT_HIDDEN)
			continue;
		
		szShort[0] = pcl->uCmd % 256;
		szShort[1] = pcl->uCmd / 256;
		flg = pcl->flgParam;		
		if (flg & OPT_INT) 
			szPar = "=n";
		else if (flg & OPT_REAL)
			szPar = "=n.n";
		else if (flg & OPT_STRING)
			szPar = "=\"str\"";
		else
			szPar = "";

		strcpy_s(strOne, sizeof(strOne), szShort);
		strcat_s(strOne, sizeof(strOne), szPar);
		strcpy_s(strTwo, sizeof(strTwo), pcl->strLong);
		strcat_s(strTwo, sizeof(strTwo), szPar);
					

	    fprintf(stderr, " -%-8s or --%-18s %s\n", strOne, strTwo, pcl->strHelp);
//	    fprintf(stderr, "<tr><td>-%-8s</td><td>--%-18s</td><td>%s</td></tr>\n", strOne, strTwo, pcl->strHelp);
    }
}

//***************************************************************************
#ifdef TEST_CLT
/*
 * main entry point (for testing purposes)
 */

int main (int argc, char **argv) {
	int rval;
	myCLParams clparam;
	unsigned short uv;

	while (--argc) {
		rval = cmdltInterpretOne(*++argv, &clparam);

		if (rval == -1) {
			fprintf(stderr, "Oops: Command %s wasn't understood!\n", *argv);
		} else {
			fprintf(stderr, "Yeah! Command %s was interpreted as follows:\n", *argv);
			fprintf(stderr, "      Command: %s\n", myclops[clparam.ixCLOpts].strLong);
			uv = myclops[clparam.ixCLOpts].flgParam;
			if (uv & OPT_INT)
				fprintf(stderr, "      Parameter (INT): %ld\n", clparam.nParam);
			else if (uv & OPT_REAL)
				fprintf(stderr, "      Parameter (REAL): %f\n", clparam.fParam);
			else if (uv & OPT_STRING)
				fprintf(stderr, "      Parameter (TEXT): [%s]\n", clparam.strParam);
			else
				fprintf(stderr, "      No Parameter\n");
		}
	}
}

#endif // TEST_CLT
