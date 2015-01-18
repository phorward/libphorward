/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	array.h
Author:	Jan Max Meyer
Usage:	Universal, dynamic array management functions
----------------------------------------------------------------------------- */

#ifndef PARRAY_H
#define PARRAY_H

typedef struct
{
	void*	array;
	size_t	first;
	size_t	last;
	size_t	count;

	size_t	size;
	size_t	step;
} parray;

#endif
