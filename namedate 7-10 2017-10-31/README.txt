NAMEDATE: Add a date to a given filename                         Version 7.10
Copyright © 1999, 2002-2009, 2014-2017 by James Greene. All rights reserved.
james.greene@outlook.com - www.informatics-consulting.de - ASP Member #06128

Usage: namedate [switches] filename(s)

where [switches] may include
 @filename or --ConfigFile="file"  Read configuration options from a file
 -B=n      or --BufferSize=n       Maximum <n> files per folder
 -BD       or --AutoBuffer         (maintained for backward compatibility)
 -C        or --CopyFiles          Copy (instead of renaming) files
 -D        or --RenameFolders      Rename folders as well as files
 -E="str"  or --Region="str"       Set the region/language
 -EE="str" or --RegionASCII="str"  Set the region/language (ASCII)
 -F        or --PrefixDate         Put the date before the original filename
 -H        or --HiddenFiles        Include hidden and system files
 -K        or --OverwriteFiles     Replace (overwrite) existing files
 -KK       or --ReplaceFolders     Replace (overwrite) files and folders
 -L        or --LastDot            Use the LAST dot for the extension
 -M        or --ModifyTime         Use the file modification time
 -ME       or --Exif               Use the Exif date/time if present
 -G        or --UTC                Use UTC instead of the local timezone
 -OD=n     or --OffsetDays=n       Adjust the date by <n> days
 -OW=n     or --OffsetWeeks=n      Adjust the date by <n> weeks
 -OM=n     or --OffsetMonths=n     Adjust the date by <n> months
 -OY=n     or --OffsetYears=n      Adjust the date by <n> years
 -AH=n     or --AdjustHours=n      Adjust the time by <n> hours
 -AM=n     or --AdjustMinutes=n    Adjust the time by <n> minutes
 -AS=n     or --AdjustSeconds=n    Adjust the time by <n> seconds
 -P        or --Simulate           Simulate (doesn't rename the files)
 -Q        or --Quiet              Quiet mode
 -SM="str" or --MoveToTarget="str" Move the files to the target folder
 -SZ       or --FolderFormat       Specifies that the folder name is dynamic
 -S="str"  or --TargetFolder="str" Specify the target folder
 -U        or --ConvertSpaces      Convert spaces to underscores
 -Y        or --Yes                Answer YES to all questions
 -Z="str"  or --FormatDate="str"   Specify the date/time format
 -ZZ="str" or --NameFormat="str"   Specify the filename format
 -8        or --DOSName            Use DOS (8.3) filename
 -?        or --Help               Display this help text
 -V        or --Version            Display the version of NameDate
 -N        or --DateDMY            Use the DD-MM-YYYY date format (L)
 -R        or --DateYMD            Use the YYYY-MM-DD date format (L)
 -T        or --AppendTime         Add the time to the filename (L)
 -X        or --DateAsExt          Add the date as a file extension (L)
 --        or --                   Stop processing command line options

Notes for Version 7.10:
-----------------------
* The -BD option is now the default behaviour. -B=n option turns off the 
  dynamic allocation and limits NameDate to <n> files per directory.

* -E specifies the language to use when using -Z or -ZZ with the a, A, b, and B keys.
  -E specifies that the conversion will use ANSI characters (Windows character set),
  while -EE specifies that the conversion should use ASCII (DOS) characters. See the
  examples below for more clarity. For a complete list of languages, please see
  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt_language_strings.asp

* For formatting examples with -Z, -ZZ as well as the -S or -SM in combination with 
  -SZ, please see below. Keep in mind that any literal characters must be preceeded
  with a backslash (e.g. "\.\.\\\T\e\s\t-Ymd\\" will result in "..\Test-20161227\"
  See the examples below for more clarity.
  
* NameDate now supports command files and long commands (starting with two dashes).
  See below for more details. 


NAMEDATE Version 7.10
Copyright (c) 1999, 2002-2009, 2014-2016 by James Greene. All rights reserved.
james.greene@outlook.com - www.informatics-consulting.de - ASP Member #06128

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (COPYING.TXT), or see <http://www.gnu.org/licenses/>.

Notes:

	** In all of the following examples, the "current date" is Wednesday, 
	** December 27, 2016.

	/F and /X are exclusive. If neither is defined, the date is placed after 
	the original filename:

	Original Filename:	sample.txt

	Example 1:			NAMEDATE "sample.txt"
						renames "sample.txt" to "sample 27-Dec-2016.txt"

	Example 2:			NAMEDATE /F "sample.txt"
						renames "sample.txt" to "27-Dec-2016 sample.txt"

	Example 3:			NAMEDATE /X "sample.txt"
						renames "sample.txt" to "sample.27-Dec-2016.txt"

	-Z and --DateFormat define how the date should be formatted. The following
	placeholders are used:

		a	Abbreviated weekday name
		A	Full weekday name
		b	Abbreviated month name
		B	Full month name
		d	Day of month as decimal number (01-31)
		H	Hour in 24-hour format (00-23)
		I	Hour in 12-hour format (01-12)
		j	Day of year as decimal number (001-366)
		m	Month as decimal number (01-12)
		M	Minute as decimal number (00-59)
		p	Current locale’s A.M./P.M. indicator for 12-hour clock
		S	Second as decimal number (00-59)
		U	Week of year as decimal number, with Sunday as first day of week (00-53)
		w	Weekday as decimal number (0-6; Sunday is 0)
		W	Week of year as decimal number, with Monday as first day of week (00-53)
		y	Year without century, as decimal number (00-99)
		Y	Year with century, as decimal number
		z	Time-zone name or abbreviation; no characters if time zone is unknown
		Z	Same as z

	When -ZZ or --NameFormat is used, the following three options are available:
		F	The filename (without the extension)
		X	The file extension (without the leading dot).
		C	A 4-digit counter of the number of files renamed.
		
	(New in Version 7.10) If you enclose part of the filename in square brackets,
	those characters will be taken literally and not subject to replacement. E.g.
	NAMEDATE -ZZ:"Sample_Y-m-d.X" somefile.txt
	would result in something like "39Mon10PMle_2017-10-30.txt" because the S, a, 
	m and p of "Sample" would be intepreted as (S) seconds, (a) weekday, (m) month 
	and (p) AM/PM. Enclosing the first part of the name in square brackets prevents
	NameDate from interpreting these characters, as shown below:
		
		NAMEDATE -ZZ:"[Sample_]Y-m-d.X" somefile.txt
		will rename 'somefile.txt' to 'Sample_2017-10-30.txt'
	
	-E or --Region specifies the language to use when using -Z (--DateFormat) 
	or -ZZ (--NameFormat) with the a, A, b, and B placeholders.
	
	-E (--Region) specifies that the conversion will use ANSI characters 
	(Windows character set), while -EE (--RegionASCII) specifies that the 
	conversion should use ASCII (DOS) characters. See the examples below 
	for more clarity. For a complete list of languages, please see
	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt_language_strings.asp

	If you want to add text that uses any of the above characters, preceed the character
	with a backslash (see Example 6).

	Original Filename:	sample.txt

	Example 4:			NAMEDATE -X -Z:"Ymd" "sample.txt"
						renames "sample.txt" to "sample.20161227.txt"

	Example 5:			NAMEDATE -Z:"Y-m-d H-M-S" "sample.txt"
						renames "sample.txt" to "sample 2016-12-27 15-30-18.txt"

	Example 6:			NAMEDATE -Z:"Y-m-d \at I.M.S p" "sample.txt"
		Alternative:	NAMEDATE -Z:"Y-m-d [at] I.M.S p" "sample.txt"
						renames "sample.txt" to "sample 2016-12-27 at 03.30.18 PM.txt"

	Example 7:			NAMEDATE -ZZ:"F Y-m-d H-M-S.X" "sample.txt"
						renames "sample.txt" to "sample 2016-12-27 15-30-18.txt"

	Example 8:			NAMEDATE -ZZ:"F Y-m-d C.X" "sample*.txt"
						renames "sampleA.txt" to "sampleA 2016-12-27 0001.txt"
						renames "sampleB.txt" to "sampleB 2016-12-27 0002.txt"

	Example 9a:			NAMEDATE -Z:"A, d-B-Y" -E:"German" "sample.txt"
						renames "sample.txt" to "sample Dienstag, 27-Dezember-2016.txt"

	Example 9b:			NAMEDATE -Z:"A, d-B-Y" -E:"French" "sample.txt"
						renames "sample.txt" to "sample mardi, 27-décembre-2016.txt"

	Example 9c:			NAMEDATE -Z:"A, d-B-Y" -E:"Spanish_Mexico" "sample.txt"
						renames "sample.txt" to "ssample martes, 27-diciembre-2016.txt" (using the ANSI character set)

	Example 9d:			NAMEDATE -Z:"A, d-B-Y" -EE:"Spanish_Mexico" "sample.txt"
						renames "sample.txt" to "ssample martes, 27-diciembre-2016.txt" (using the ASCII character set)

	/M uses the file date and time instead of the current date and time.
		
	/OY=-1 = subtract 1 YEAR from the date
	/OM=+3 = add 3 MONTHS to the date
	/OD=-7 = subtract 7 DAYS from the date
	/OW=-2 = subtract 2 WEEKS from the date

	/AH-6  = subtract 6 HOURS from the time
	/AM-30 = subtract 30 MINUTES from the time
	/AS+15 = add 15 SECONDS to the time
	
	Original Filename:	sample.txt

	Example 10:			NAMEDATE -OM=+3 "sample.txt"
						renames "sample.txt" to "sample 27-Mar-2017.txt"

	Example 11:			NAMEDATE -AH=+4 -Z:"Y-m-d H-M-S" "sample.txt"
						renames "sample.txt" to "ssample 2016-12-27 22-05-55.txt"

	Some notes about the the -OM=[n] and -OY=[n] commands:
	! NameDate will ensure that the resulting date is valid. For example,
	! on March 30th, using the -OM-1 command (subtract 1 month from the date)
	! will result in the date being February 28th or 29th (if it happens to be
	! a leap year). Using the command -OM=-12 is functionally the same as -OY=-1, 
	! and -OM+12 is the same as -OY+1. The + sign can be left off of any positive
	! values, so -OM=12 is interpreted the same as -OM=+12.

	-U converts any spaces in the filename to underscores.

	Example 12:			NAMEDATE -U "sample.txt"
						renames "sample.txt" to "sample_27-Dec-2016.txt"

	-C creates a copy of the file instead of renaming it.

	Example 13:			NAMEDATE -C "sample.txt"
						*copies* "sample.txt" to "sample 27-Dec-2016.txt"


	-8 overrides the -Z command. The file will be renamed to
	"AAyymmdd.ext", where yymmdd is the date (yy=year mm=month
	dd=day). If more than one file is to be renamed, the second file
	will be named BAyymmdd.ext, the next CAyymmdd.ext, and so in.
	The 26th file will be ZAyymmdd.ext, the 27th then AByymmdd.ext.

	Example 14:			NAMEDATE -8 "sample.txt"
						renames "sample.txt" to "AA161227.txt"

	-S:"[path] is used to move the file into a new directory (or copy it, 
	when used in conjunction with the -C command) while renaming it.

	Example 15: 		NAMEDATE -C -S:"C:\BACKUP" "sample.txt"
						*copies* "sample.txt" to "C:\BACKUP\sample 27-Dec-2016.txt"

	The following examples demonstrate the -L parameter, which affects how 
	NAMEDATE deals with filenames that contain more than one dot. Previous 
	releases of NAMEDATE assumed that the extension began after the FIRST 
	dot in the filename. With the -L parameter, this is reversed, so that 
	NAMEDATE assumes that the filename ends with the LAST dot.

	Example 16a:		NAMEDATE -ZZ:"F.Ymd.X" "sample.file.txt"
						renames "sample.file.txt" to "sample.20161227.file.txt"
	Example 16b:		NAMEDATE -L -ZZ:"F.Ymd.X" "sample.file.txt"
						renames "sample.file.txt" to "sample.file.20161227.txt"
						
	New in Version 7.0 - NameDate now supports long commands and command files.
	
	Example 17:			NAMEDATE @ndoptions.txt sample.txt
						copies "sample.txt" to ".\20161227\sample 27-Dez-2016.txt"
	
	Contents of ndoptions.txt:
		# ndoptions.txt - A command file for NameDate
		--CopyFiles 
		--TargetFolder = "Ymd" 
		--FolderFormat
		--Yes
		--ModifyTime
		--Region = "German"
		# end of ndoptions.txt
		
NameDate Command Files
	As shown in the example above, command line parameters can now be saved in 
	a text file which is processed by NameDate. These command files can contain
	command line options, either as short (e.g. -C) or long (e.g. --CopyFiles)
	form. Parameters can be separated by spaces (unlike the command line 
	itself). 
	
	Comments are preceeded with a hash (#) and continue to the end of 
	the current line. 
	
	The following is functionally identical to the ndoptions.txt shown above, 
	but uses the short form:
	
		# ndopt.txt - A short form of ndoptions.txt
		-C -S="Ymd" -SZ -Y -M -E="German"
		# end of ndopt.txt
		
	You can also include the filenames to be converted in the command file,
	e.g.
	
		# ndsample.txt
		-C -S="Ymd" -SZ -Y -ME -E="German" C:\Incoming\cimg*.jpg
		# end of ndsample.txt
		
		C> namedate @ndsample.txt
		

Building NAMEDATE
	NAMEDATE version 7.10 has been complied with the MinGW C compiler.
	
	gcc (GCC) 6.3.0
	Copyright (C) 2016 Free Software Foundation, Inc.
	
	To build NAMEDATE, just expand the source archive and enter the command

		mingw32-make clean all
		
	NameDate.exe can then be found in the "Release" folder. 


Disclaimer / Warranty:
	This program is free software: you can redistribute it and/or modify it 
	under the terms of the GNU General Public License as published by the 
	Free Software Foundation, either version 3 of the License, or (at your 
	option) any later version.

	This program is distributed in the hope that it will be useful, but 
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
	or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
	for more details.

	You should have received a copy of the GNU General Public License
	along with this program (COPYING.TXT), or see <http://www.gnu.org/licenses/>.
	
	Please report any bugs to <james.greene@outlook.com>
