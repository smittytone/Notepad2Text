========================================
Amstrad NotePad Word Processor Format 
to Rich Text Format Conversion Utility

Version 1.0

by Maksim Lin
========================================

Version 1.0 Changes:
====================

The main change I have made is that I have changed filename input to be from commandline
only.  Note that the program will automatically generate the outfilename, which will be 
input filename plus an extension of .RTF (regardless of the original extension).

The main bug fix is that the RTF special characters of {.} & \ are all now handled properly
(thanks for picking that up Mark).

I've also decided not to put in wildcard expansion in, since most people will only be
converting one or two files at a time anyway.

Finally, since this version now does pretty much all I need it to do, it's probably the
last time that I will work on it (unless any big bugs turn up :), but as it's GPL'ed please
feel free to extend/modify it, though I would appreciate it if you use it to do anything
interesting (if such a thing is possible ;)

PS. If you didn't get this from the NC100 web page, please note that a BBC Basic version 
by Mark Ray is also available.

Maksim Lin.
======================================================================

Zip Package Contents:

README.txt	: this file
Np-rtf.exe	: DOS executable
Np-rtf.c	: C source code
Test.npd	: test Notepad WP file
Test.rtf	: rtf converted version of test file


This utility pretty much does as the name says, just run it and give it the 
appropraite file names.  It is still very much a "work in progress" and so 
should be considered an alpha version.
At the moment, I have only test compiled it under DOS (Turbo C, ver 1) and 
checked the rtf output with QuickView, Wordpad and MSWorks 4 in Win95.
As such I would very much like to get any comments, criticisms, bug reports etc.
If you do find a bug (which is quite likely at this stage) if possible, please 
include a copy of the document that you were converting.
(my email address is at the bottom of this file)

However, at the moment my net access is restricted to weekends so it might take me 
a few days to reply.

The program is not very pretty at the moment, and I intend to add things like better 
filename input (ie, command line arguements) amd batch conversions in the very near future, 
and if I get time, a reverse capability, ie RTF to Notepad WP.  
Any suggestions for new features would be welcome.

Also I am releasing this program under GPL, and if anyone is interested in making a BBC Basic 
version to run on the Notepad itself, please feel free (since I don't know Basic very well), 
but just let me know that you are doing it and if you need any info on my code.

Maksim Lin
9337849@bud.swin.edu.au

