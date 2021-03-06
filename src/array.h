/* -HEADER----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	array.h
Author:	Jan Max Meyer
Usage:	Universal, dynamic array management functions
----------------------------------------------------------------------------- */

#ifndef PARRAY_H
#define PARRAY_H

typedef struct _parray parray;

struct _parray
{
	char*	start;		/* Array memory address */

	char*	bottom;		/* = start + first * size */
	char*	top;		/* = start + last * size */

	size_t	first;		/* Offset of first element */
	size_t	last;		/* Offset of last element */
	size_t	count;      /* Total array elements size;
							this is NOT the actual element count;
							use parray_count() for correct value! */

	size_t	size;		/* Element size */
	size_t	chunk;		/* Chunk size */

	int		(*comparefn)( parray*, void*, void* );
	int		(*sortfn)( parray*, void*, void* );
};

typedef void (*parrayfn)	( parray*, void* );		/* Array element callback */

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
/*MACRO:parray_for( parray* array, usertype* ptr )*/
#define parray_for( array, ptr )	\
	for( (ptr) = (void*)(array)->bottom; \
		(ptr) && ((char*)(ptr) < (array)->top || ((ptr) = NULL)); \
			(ptr) = (void*)((char*)(ptr) + (array)->size) )

#endif
