/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit :: General Header
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
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
 * General Data Types & Structures
 */

/* wchar - universal (wide?) character type */
#ifdef UNICODE
typedef wchar_t					wchar;
#else
typedef int						wchar;
#endif

/* Phorward Software primitive type specifiers */
typedef unsigned char			pbyte;		/* Byte type */
typedef wchar					pchar;		/* Phorward char type (wide char) */
typedef	int						pint;		/* Integer type */
typedef	float					pfloat;		/* Floating point */
typedef double					pdouble;	/* Double type */
typedef long					plong;		/* Long-type */
typedef size_t					pulong;		/* Unsigned long */
typedef size_t					psize;		/* Size-type */

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
 * Generic error codes
 */
#define ERR_OK					0		/* OK */
#define ERR_FAILURE				-1		/* General/Unspecified failure */
#define ERR_PARMS				-2		/* Wrong parameters */
#define ERR_MEM					-3		/* Memory allocation error */
#define ERR_SYSTEM				-4		/* System call error */
#define ERR_UNIMPL				-5		/* Unimplemented */
#define ERR_OTHER				-6		/* Any other error */

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
									"please check for correctness.", \
									__FILE__, __LINE__ )

#define TODO					fprintf( stderr, \
									"%s, %d: TODO alert! The program ran into "\
									"a module that is not finished yet!\n", \
											__FILE__, __LINE__ )

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
#include "base/llist.h"
#include "base/plist.h"
#include "base/stack.h"
#include "union/union.h"
#include "xml/xml.h"

#include "regex/pregex.h"

#include "grammar/grammar.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

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

