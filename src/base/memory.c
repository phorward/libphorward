/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	memory.c
Author:	Jan Max Meyer
Usage:	Memory management functions / malloc replacements
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Dynamically allocate heap memory.

The function is a wrapper for the system function malloc(), but with memory
initialization to zero, and immediatelly stops the program if no more memory
can be allocated.

//size// is the size of memory to be allocated, in bytes.

The function returns the allocated heap memory pointer. The returned memory
address should be freed using pfree() after it is not required anymore.
*/
void* pmalloc( size_t size )
{
	void*	ptr;

	if( !( ptr = malloc( size ) ) )
	{
		OUTOFMEM;
		return (void*)NULL;
	}

	memset( ptr, 0, size );
	return ptr;
}

/** Dynamically (re)allocate memory on the heap.

The function is a wrapper to the system-function realloc(), but always
accepts a NULL-pointer and immediatelly stops the program if no more memory
can be allocated.

//oldptr// is the pointer to be reallocated. If this is (void*)NULL,
prealloc() works like a normal call to pmalloc().

//size// is the size of memory to be reallocated, in bytes.

The function returns the allocated heap memory pointer. The returned memory
address should be freed using pfree() after it is not required anymore.
*/
void* prealloc( void* oldptr, size_t size )
{
	void*	ptr;

	if( !oldptr )
		return pmalloc( size );

	if( !( ptr = realloc( oldptr, size ) ) )
	{
		OUTOFMEM;
		return (void*)NULL;
	}

	return ptr;
}

/** Free allocated memory.

The function is a wrapper for the system-function free(), but accepts
NULL-pointers and returns a (void*)NULL pointer for direct pointer memory reset.

It could be used this way to immedatelly reset a pointer to NULL:

``` ptr = pfree( ptr );

//ptr// is the pointer to be freed.

Returns always (void*)NULL.
*/
void* pfree( void* ptr )
{
	if( ptr )
		free( ptr );

	return (void*)NULL;
}

/** Duplicates a memory entry onto the heap.

//ptr// is the pointer to the memory to be duplicated.
//size// is the size of pointer's data storage.

Returns the new pointer to the memory copy. This should be casted back to the
type of //ptr// again.
*/
void* pmemdup( void* ptr, size_t size )
{
	void*	ret;

	if( !( ptr && size ) )
	{
		WRONGPARAM;
		return (void*)NULL;
	}

	ret = pmalloc( size );
	memcpy( ret, ptr, size );

	return ret;
}
