/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit :: General Header
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	phorward.h
Usage:	Main include file for phorward library
----------------------------------------------------------------------------- */

#ifndef _PHORWARD_H
#define _PHORWARD_H

/*
 * Standard System Includes
 * They're always required, in all of our programs!
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <stdarg.h>

#ifndef va_copy
#define va_copy( a, b )	(a) = (b)
#endif

#include <limits.h>
#include <time.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef UNICODE
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

/*
 * Boolean
 */

typedef char 					pboolean;	/* Phorward Boolean */
#ifndef BOOLEAN
#define BOOLEAN					pboolean
#endif

#ifndef boolean
#define boolean 				pboolean
#endif

#ifndef TRUE
#define TRUE					1
#endif

#ifndef FALSE
#define FALSE					0
#endif

#define BOOLEAN_STR( b ) 		( ( b ) ? "TRUE" : "FALSE" )
#define TRUEBOOLEAN( b ) 		( ( b ) ? TRUE : FALSE )

/*
 * Generic error case macros
 */
#define OUTOFMEM				fprintf( stderr, \
									"%s, %d: Ran out of memory\n", \
										__FILE__, __LINE__ ), exit( 1 )

#define WRONGPARAM				fprintf( stderr, \
									"%s, %d: Function called with wrong or " \
									"incomplete parameters, fix your call!\n", \
										__FILE__, __LINE__ )

#define MISSINGCASE				fprintf( stderr, \
									"%s, %d: Missing case enganged, " \
									"please check for correctness.\n", \
									__FILE__, __LINE__ )

#define TODO					fprintf( stderr, \
									"%s, %d: TODO alert! The program ran into "\
									"a module that is not finished yet!\n", \
											__FILE__, __LINE__ )

#define CORE					{ 	int* x = NULL; \
									fprintf( stderr, \
									"%s, %d: !CORE!\n", __FILE__, __LINE__ ); \
									*x = 123; \
								}

/*
 * Path separator
 */
#ifndef _WIN32
#define PDIRSEP					':'
#define PPATHSEP				'/'
#else
#define PDIRSEP					';'
#define PPATHSEP				'\\'
#endif

/*
 * Including the different library modules
 */

#include "base/dbg.h"
#include "base/array.h"
#include "base/list.h"
#include "xml/xml.h"

#include "ccl/ccl.h"
#include "regex/regex.h"

#include "value/value.h"

/* Phorward Parsing Library */
#include "parse/parse.h"

/*
 * Function Prototypes
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "proto.h"

#ifdef __cplusplus
}
#endif

#endif /* _PHORWARD_H */

