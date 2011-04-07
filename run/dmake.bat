@echo off
rem ----------------------------------------------------------------------------
rem Phorward Software Development Kit
rem Copyright (C) 2009, 2010 by Phorward Software Technologies, Jan Max Meyer
rem http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
rem All rights reserved. See $PHOME/LICENSE for more information.
rem
rem Script:		dmake.bat
rem Author:		Jan Max Meyer
rem Usage:		Run 'make' with Debug-Flag
rem ----------------------------------------------------------------------------
echo ********************
echo * DEBUG MAKE START *
echo ********************
nmake DEBUG=/D__WITH_TRACE %*
echo *******************
echo * DEBUG MAKE DONE *
echo *******************
