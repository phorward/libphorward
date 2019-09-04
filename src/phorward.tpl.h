/* -HEADER----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	phorward.h
Usage:	Template for the general include file for Phorward C/C++ Library
----------------------------------------------------------------------------- */

#ifndef _PHORWARD_H
#define _PHORWARD_H

/*
 * Standard System Includes
 * They're always required, in all of our programs!
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#ifdef UTF8
#include <wchar.h>
#endif

#ifdef UNICODE
#include <wctype.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

##include "boolean.h"

/*
 * Generic error case macros
 */

/** Prints "ran out of memory" into stderr and exits with status 1. */
/*MACRO:OUTOFMEM */
#define OUTOFMEM				fprintf( stderr, \
									"%s, %d: Ran out of memory\n", \
										__FILE__, __LINE__ ), exit( 1 )

/** Prints "function called with wrong or incomplete parameters" into stderr,
to indicate invalid function calls. */
/*MACRO:WRONGPARAM */
#define WRONGPARAM				fprintf( stderr, \
									"%s, %d: Function called with wrong or " \
									"incomplete parameters, fix your call!\n", \
										__FILE__, __LINE__ )

/** Prints "missing case engaged" into stderr, to indicate switch-case
constructs running into default-branch because any other branch is not engaged.
*/
/*MACRO:MISSINGCASE */
#define MISSINGCASE				fprintf( stderr, \
									"%s, %d: Missing case engaged, " \
									"please check for correctness.\n", \
									__FILE__, __LINE__ )

/** Prints "TODO alert!" into stderr, to inform about incomplete branches. */
/*MACRO:TODO */
#define TODO					fprintf( stderr, \
									"%s, %d: TODO alert! The program ran into "\
									"a module that is not finished yet!\n", \
											__FILE__, __LINE__ )

/** Enforces a SIGSEGV by writing to invalid memory. */
/*MACRO:CORE */
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

/* Watch for order! */
##include "version.h"

##include "dbg.h"
##include "array.h"
##include "list.h"
##include "ccl.h"

##include "regex/regex.h"

/*
 * Function Prototypes
 */

#ifdef __cplusplus
extern "C"
{
#endif

##include "proto.h"

#ifdef __cplusplus
}
#endif

#endif /* _PHORWARD_H */

