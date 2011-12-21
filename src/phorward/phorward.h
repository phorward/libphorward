/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: General Header
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

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
typedef unsigned int			wchar;
#endif

/* Phorward Software primitive type specifiers */
typedef char					pbyte;		/* Byte type */
typedef wchar					pchar;		/* Phorward char type (wide char) */
typedef char					uchar;		/* Universal char type (one byte) */
typedef	int						pint;		/* Integer type */
typedef	float					pfloat;		/* Floating point */
typedef double					pdouble;	/* Double type */
typedef long					plong;		/* Long-type */
typedef size_t					pulong;		/* Unsigned long */
typedef size_t					psize;		/* Size-type */

typedef int 					pboolean;	/* Phorward Boolean */
#define BOOLEAN					pboolean
#define boolean 				pboolean

#define BOOLEAN_STR( b ) ( ( b ) ? "TRUE" : "FALSE" )

#define TRUE	1
#define FALSE	0

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
 * Generic macros
 */
#define OUTOFMEM				fprintf( stderr, \
									"%s, %d: Ran out of memory\n", \
										__FILE__, __LINE__ ), exit( 1 )
#define WRONGPARAM				fprintf( stderr, \
									"%s, %d: Function called with wrong or " \
									"incomplete parameters, fix your call!\n", \
										__FILE__, __LINE__ )

/*
 * Path separator
 */
#ifndef _WIN32
#define PPATHSEP				'/'
#else
#define PPATHSEP				'\\'
#endif

/*
 * Bitwise value modifiers
 */
#define bit_set( val, bit )		( (val) |= (bit) )
#define bit_unset( val, bit )	( (val) &= ~(bit) )
#define bit_is_set( val, bit )	( ( (val) & (bit) ) == (bit) )
#define bit_is_unset( val, bit ) \
 								( ( (val) & (bit) ) == 0 )

/*
 * Basis Library modules
 */
#include "bitset.h"
#include "ccl.h"
#include "dbg.h"
#include "llist.h"
#include "hashtab.h"
#include "stack.h"
#include "string.h"
#include "utf8.h"
#include "var.h"
#include "xml.h"

/*
 * Regex Library modules
 */
#include "regex/regex.h"

/*
 * String Library modules
 */
#include "string/string.h"

/*
 * Function Prototypes
 */
#include "proto.h"

#endif /* _PHORWARD_H */

