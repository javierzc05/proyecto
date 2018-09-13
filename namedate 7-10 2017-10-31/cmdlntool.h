/*
 * cmdlntool.h
 *
 *  Created on: 17.12.2016
 *      Author: James Greene
 */

#ifndef CMDLNTOOL_H_
#define CMDLNTOOL_H_

#define	MAX_PARAM	256			// 	maximum length of the command line
#define MAX_LONGCMD	16			// 	maximum length of one command (without any parameters)
#define OPT_NONE	0			//	no parameters expected/accepted
#define	OPT_INT		1			//	integer (numeric) parameter expected/accepted
#define	OPT_REAL	2			//	real (numeric) parameter expected/accepted
#define OPT_STRING 	4			//	string parameter expected/accepted
#define OPT_TYPMASK	0x00FF		//	the AND mask for the parameter types
#define OPT_HIDDEN	0x0100		//	flag that the "help" text is not included when --help is processed
#define OPT_NOTREQ	0x0200		// 	flag that the parameter is optional (not required)
#define	CLOPT_FILE		0		// 	the entry in the myCLOpts array for filenames
#define	CLOPT_CMDFILE	1		// 	and the entry for a @filename

/*
 * myCLOps = My Command Line Parameters - The command lines my program 
 * understands.
 *   uCmd = the short form of the command (e.g. -?)
 *   strLong = the long form of the command (--help)
 *   uMinLen = the minimum number of characters needed to match strLong
 *   fParam  = OPT_NONE (none), OPT_INT (numeric) or OPT_STRING (text)
 *   strHelp = the help text to be displayed when -? or --help is used.
 */

typedef struct strCLOpts {
  unsigned uCmd;
  char *strLong;
  unsigned uMinLen;
  unsigned flgParam;
  char *strHelp;
} myCLOps;

/*
 * myCLParams = The actual command line parameters passed by the user
 *   uCmd = the command code, copied from myclops[xx].uCmd
 *	 flgParam = copied from myclops[xx].flgParam 
 *	 ixCLOPts = the index into myclops (xx in the above two lines)
 *	 flgStopCLP = set to TRUE when the "--" command is interpreted
 *   nParam = the integer parameter (when flgParam == OPT_INT)
 *   fParam = the real parameter (when flgParam == OPT_REAL)
 *   strParam = the string parameter (when fParam == OPT_STRING)
 */
typedef struct strCLParam {
  unsigned uCmd;
  unsigned flgParam;
  unsigned ixCLOpts;
  unsigned flgStopCLP;
  long  nParam;
  float fParam;
  char  strParam[MAX_PARAM];
} myCLParams;

//**************************************************************************

// Entry Points - cmd

int		cmdltInterpretOne(const char *clarg, myCLParams *clparam);
void	cmdUsage(char *exe);

//**************************************************************************

extern myCLOps	myclops[];
extern unsigned	NUM_MYCLOPS;

#define CFG_ORIGFILE     48        // OriginalFile
#define CFG_CFGFILE      64        // ConfigFile
#define CFG_BUFSZ        66        // BufferSize
#define CFG_BUFDY        17474        // AutoBuffer
#define CFG_COPY         67        // CopyFiles
#define CFG_RENDIR       68        // RenameFolders
#define CFG_RGN          69        // Region
#define CFG_RGNDOS       17733        // RegionASCII
#define CFG_PREFIX       70        // PrefixDate
#define CFG_HIDDEN       72        // HiddenFiles
#define CFG_REPL         75        // OverwriteFiles
#define CFT_REPLDIR      19275        // ReplaceFolders
#define CFG_LASTDOT      76        // LastDot
#define CFG_USEMTIME     77        // ModifyTime
#define CFG_EXIFDATE     17741        // ExifDate
#define CFG_UTC          71        // UTC
#define CFG_OFFD         17487        // OffsetDays
#define CFG_OFFW         22351        // OffsetWeeks
#define CFG_OFFM         19791        // OffsetMonths
#define CFG_OFFY         22863        // OffsetYears
#define CFG_ADJH         18497        // AdjustHours
#define CFG_ADJM         19777        // AdjustMinutes
#define CFG_ADJS         21313        // AdjustSeconds
#define CFG_SIMULATE     80        // Simulate
#define CFG_QUIET        81        // Quiet
#define CFG_TGTMOVE      19795        // MoveToTarget
#define CFG_FOLDFMT      23123        // FolderFormat
#define CFG_TGT          83        // TargetFolder
#define CFG_CVTSPC       85        // ConvertSpaces
#define CFG_ASSUMEYES    89        // Yes
#define CFG_FMTDATE      90        // FormatDate
#define CFG_FMTFULLNAME  23130        // NameFormat
#define CFG_8DOT3        56        // DOSName
#define CFG_HELP         63        // Help
#define CFG_VERSION      86        // Version
#define CFG_DMY          78        // DateDMY
#define CFG_YMD          82        // DateYMD
#define CFG_ADDTIME      84        // AppendTime
#define CFG_ASEXT        88        // DateAsExt
#define	CFG_STOPCLP		 45		   // -- (stop command line processing)
#define CFG_ERROR		 404	   // ERROR

#endif /* CMDLNTOOL_H_ */
