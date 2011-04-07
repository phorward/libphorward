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
#define CCL_MAX			0xffff

/* Typedefs */
typedef struct _crange	CRANGE;
typedef crange*			CCL;

struct _crange
{
	u_int	begin;
	u_int	end;
};

#endif
