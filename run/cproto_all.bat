@echo off
rem ----------------------------------------------------------------------------
rem  Phorward Software Development Kit
rem Copyright (C) 2009, 2010 by Phorward Software Technologies, Jan Max Meyer
rem http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
rem All rights reserved. See $PHOME/LICENSE for more information.
rem
rem Script:		cproto_all.bat
rem Author:		Jan Max Meyer
rem Usage:		A wrapper for mkproto (formerly cproto-wapper)
rem ----------------------------------------------------------------------------

set files=%1
set files=%files:~1,-1%

set tempfile=TEMP.PROTO.c

echo.
echo *** Generating prototypes ***
echo %files%
echo.

rem Run a preprocessor.
sh ppp -D MAKE_PROTOTYPES -I "%2" %files% >%tempfile%

rem cl /nologo /E /DMAKE_PROTOTYPES /I "%2" %files% >%tempfile%
rem mkptypes -A TEMP.PROTO.C

rem remove Windows line breaks
rem sed -e "s/\r//g" < %tempfile% >%tempfile%.1
sh dos_to_unix %tempfile% %tempfile%.1
move %tempfile%.1 %tempfile%

rem run mkproto
echo /* >%3
echo  * This file was automatically generated with mkproto >>%3
echo  *      MANUAL EDITING IS NOT RECOMMENDED! >>%3
echo  */ >>%3
echo. >>%3
sh mkproto %4 %tempfile% >>%3

rem Delete Temporary file
del %tempfile%
rem cproto %4 -DMAKE_PROTOTYPES -I %2 %files% >TEMP.PROTO1.c
