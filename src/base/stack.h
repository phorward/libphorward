/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
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
	void*	stack;
	size_t	size;
	size_t	top;
	size_t	count;
	size_t	step;
} pstack;

#endif
