/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: String Object Library
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	local.h
Author:	Jan Max Meyer
Usage:	Some local macros and declarations used by Pstring-library
----------------------------------------------------------------------------- */

#ifndef PSTRING_LOCAL_H
#define PSTRING_LOCAL_H

/* 
 * memory (re)allocation is done in 16-character blocks
 */
#define MALLOC_STEP				16

/* 
 * numeric values are hold in a length of 128 chars?
 * must be enough...
 */
#define NUMERIC_VALUE_LEN		128

/*
 * macros for correct memory size computation
 */
#define GET_ALLOC_SIZE( size )	( ( (size) ? (size) + ( (size) % MALLOC_STEP ) \
									: MALLOC_STEP ) + 1 )
#define ALLOC_STR( obj, len )	( obj->str = (pchar*)pmalloc( \
									GET_ALLOC_SIZE( len ) \
										* sizeof( pchar ) ) )

#endif

