/* -HEADER----------------------------------------------------------------------
Phorward Basis Library
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

File:	pbasis.h
Usage:	Main include file for pbasis
----------------------------------------------------------------------------- */

#ifndef _PBASIS_H
#define _PBASIS_H

/*
 * Standard System Includes
 * They're always required, in all programs!
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>

#ifdef UTF8
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#define u8_isutf(c) (((c)&0xC0)!=0x80)

/*
 * Universal Data Types
 */

/* uchar - universal character type */
#ifndef uchar
#define uchar char
#endif

/* pchar - universal (wide?) character type */
#ifdef UTF8
#define wchar wchar_t
#else
#define wchar uchar
#endif

/* boolean - Just saving true or false ;) */
#define BOOLEAN	pboolean
#define boolean pboolean
typedef int 	pboolean;	/* Phorward Boolean */

#define TRUE	1
#define FALSE	0

/*
 * Dynamic memory management macros
 */

/* This can be replaced by some special malloc macros or functions later on */
#define pmalloc( size )			malloc( size )
#define prealloc( ptr, size )	realloc( ptr, size )
#define pfree( ptr )			do { free( ( ptr ) ); \
								( ptr ) = NULL; } while( 0 )

/*
 * Error codes
 */
#define ERR_OK					0		/* OK */
#define ERR_FAILURE				-1		/* Failure */
#define ERR_PARMS				-2		/* Wrong parameters */
#define ERR_MEM					-3		/* Memory allocation error */
#define ERR_SYSTEM				-4		/* System call error */
#define ERR_UNIMPL				-5		/* Unimplemented */
#define ERR_OTHER				-6		/* Any other error */

/*
 * Path separator
 */
#ifndef _WIN32
#define PPATHSEP				'/'
#else
#define PPATHSEP				'\\'
#endif

/*
 * Basis Library modules
 */
#include "ccl.h"
#include "bitset.h"
#include "dbg.h"
#include "hashtab.h"
#include "llist.h"

#ifndef MAKE_PROTOTYPES
#include "pbasis.proto.h"
#endif

#endif /* _PBASIS_H */
