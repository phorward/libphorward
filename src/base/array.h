/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
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
	size_t	chunk;
} parray;

typedef void (*parrayfn)	( void* );		/* Array element callback */

/* Macros */

/** Macro that expands into a for-loop iterating a parray-object //array// using
and pointer variable //ptr// as walker.

This macro expands into a for-loop in the format

``` for( ptr = parray_first( array ); ptr; ptr = parray_next( array, ptr ) )

where //ptr// is a walker variable of the same type size of the array's size
for each element.

**Example:**

```
usertype* 	t;
parray		a;

parray_init( &a, sizeof( usertype ), 0 );

parray_for( a, t )
{
	\* Do something with t *\
}
```
*/
/*MACRO:parray_for( parray* array, void* ptr )*/
#define parray_for( array, ptr )	\
	for( (ptr) = parray_first( array ); (ptr); \
			(ptr) = parray_next( array, ptr ) )

#endif
