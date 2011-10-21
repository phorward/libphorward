/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: Base Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	ccl.h
Author:	Jan Max Meyer
Usage:	Structures for character-class handling
----------------------------------------------------------------------------- */

#ifndef CCL_H
#define CCL_H

/* Defines */
#define CCL_MIN			0x0

#ifdef UTF8
#define CCL_MAX			0xffff
#else
#define CCL_MAX			0xff
#endif

/* Typedefs */
typedef struct _crange	CRANGE;
typedef struct _crange*	CCL;

struct _crange
{
	pchar	begin;
	pchar	end;
};

/* Macros */
#define ccl_free( ccl )				pfree( ccl )
#define ccl_replace( ccl, nccl )	free( ccl ), ccl = nccl
#define ccl_end( ccl )				( (ccl)->begin == CCL_MAX )

#endif
