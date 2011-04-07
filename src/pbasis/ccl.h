/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: Chraracter class handling
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

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
	wchar	begin;
	wchar	end;
};

/* Macros */
#define ccl_free( ccl )				pfree( ccl )
#define ccl_replace( ccl, nccl )	free( ccl ), ccl = nccl

#endif
