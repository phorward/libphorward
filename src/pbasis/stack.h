/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: Universal, dynamic stack management functions
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

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

#endif
