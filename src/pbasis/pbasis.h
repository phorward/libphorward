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
#include <stdarg.h>
#include <limits.h>
#include <time.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

/*
 * Universal Data Types
 */

/* uchar - universal character type */
#ifndef uchar
#define uchar char
#endif

/* u_int - Unsigned int */
#define u_int	unsigned int

/* pccl - Character classes */
#define pccl	u_int*

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
#include "bitset.h"
#include "dbg.h"
#include "hashtab.h"
#include "llist.h"

#ifndef MAKE_PROTOTYPES
#include "pbasis.proto.h"
#endif

#endif /* _PBASIS_H */
