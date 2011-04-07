/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: Linked Pointer Lists
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

File:	pbasis.h
Usage:	Main include file for pbasis
----------------------------------------------------------------------------- */

#ifndef _PBASIS_H
#define _PBASIS_H

/*
 * Standard System includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

#ifdef __WINDOWS__
#include <process.h>
#endif

/*
 * Universal Data Types
 */

/* uchar - universal character type */
#ifndef uchar
#define uchar char
#endif

/* boolean - Just saving true or false ;) */
typedef enum _boolean BOOLEAN;
typedef enum _boolean boolean;
typedef enum _boolean pboolean;	/* Phorward Boolean */

enum _boolean
{
	FALSE	= 0,
	TRUE	= 1
};

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
 * Basis Library modules
 */
#include "bitset.h"
#include "dbg.h"
#include "llist.h"
#include "hashtab.h"

#ifndef MAKE_PROTOTYPES
#include "pbasis.proto.h"
#endif

#endif /* _PBASIS_H */
