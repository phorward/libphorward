/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	error.c
Usage:	Error reporting facilities
----------------------------------------------------------------------------- */

#include "phorward.h"

void PGERR( pggrammar* g, char* file, int line, char* fmt, ... )
{
	va_list		args;

	va_start( args, fmt );

	if( file && line > 0 )
		fprintf( stderr, "%s, %d: ", file, line );

	vfprintf( stderr, fmt, args );
	va_end( args );
	fprintf( stderr, "\n" );
}
