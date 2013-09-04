/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	stack.h
Author:	Jan Max Meyer
Usage:	Universal, dynamic stack management functions
----------------------------------------------------------------------------- */

#ifndef STACK_H
#define STACK_H

typedef struct
{
	pbyte*	stack;
	psize	size;
	psize	top;
	psize	count;
	psize	step;
} STACK;

#define STACK_CALLBACK		void(*) ( pbyte* )
#define STACK_NO_CALLBACK	( (STACK_CALLBACK)NULL )

#endif
