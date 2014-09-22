/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	array.c
Author:	Jan Max Meyer
Usage:	Universal, dynamic array management functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Performs an array initialization.

//array// is the pointer to the array to be initialized.

//size// defines the size of one array element, in bytes.
This should be evaluated using the sizeof()-macro.

//step// defines the step size, where an array-(re)allocation will be performed.
If, e.g. this is set to 128, then, if the 128th item is created within the
array, a realloction is done. Once allocated memory remains until the array is
freed again.
*/
pboolean parray_init( parray* array, size_t size, size_t step )
{
	PROC( "parray_init" );
	PARMS( "array", "%p", array );
	PARMS( "size", "%ld", size );
	PARMS( "step", "%ld", step );

	if( !( array && size > 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( step <= 0 )
		step = 1;

	memset( array, 0, sizeof( parray ) );
	array->size = size;
	array->step = step;

	RETURN( TRUE );
}

/** Create a new parray as an object with an element allocation size //size//
and a reallocation-step-size of //step//.

The returned memory must be released with parray_free().  */
parray* parray_create( size_t size, size_t step )
{
	parray*	array;

	if( size <= 0 )
	{
		WRONGPARAM;
		return (parray*)NULL;
	}

	array = (parray*)pmalloc( sizeof( parray ) );
	parray_init( array, size, step );

	return array;
}

/** Erase a dynamic array.
The array must not be reinitialized after destruction, using parray_init().

//array// is the pointer to the array to be erased. */
pboolean parray_erase( parray* array )
{
	PROC( "parray_free" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	array->array = pfree( array->array );
	array->top = 0;
	array->count = 0;

	RETURN( TRUE );
}

/** Releases all the memory //array// uses and destroys the array object.

The function always returns (plist*)NULL. */
parray* parray_free( parray* array )
{
	if( !array )
		return (parray*)NULL;

	parray_erase( array );
	pfree( array );

	return (parray*)NULL;
}

/** Pushes an element to the end of the array.

The element's memory is copied during the push. The item must be of the same
memory size as used at array initialization.

//array// is the pointer to array where to push an item on.

//item// is the pointer to the memory of the item that should be pushed onto the
array. The caller should cast his type into void, or wrap the push-operation
with a macro. It can be left (void*)NULL, so no memory will be copied.

The function returns the address of the newly pushed item, and (void*)NULL if
the item could not be pushed.
*/
void* parray_push( parray* array, void* item )
{
	PROC( "parray_push" );
	PARMS( "array", "%p", array );
	PARMS( "item", "%p", item );

	if( !( array ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	/* Is memory (re-)allocation required? */
	VARS( "array->count", "%d", array->count );
	if( array->count == 0 )
	{
		MSG( "Performing first allocation" );
		VARS( "Allocating bytes", "%d", array->step * array->size );
		if( !( array->array = (void*)pmalloc( array->step * array->size ) ) )
			RETURN( (void*)NULL );

		array->count = array->step;
		array->top = 0;
	}
	else if( array->top + 1 == array->count )
	{
		MSG( "A array expansion is required" );

		array->count += array->step;

		VARS( "Re-allocating bytes", "%d", array->step * array->size );
		if( !( array->array = (void*)prealloc( (void*)array->array,
					array->count * array->size ) ) )
			RETURN( (void*)NULL );
	}

	/* Copy item into top of array */
	if( item )
	{
		memcpy( (char*)array->array + array->top * array->size,
					item, array->size );
	}

	RETURN( (char*)array->array + array->top++ * array->size );
}

/** Pushes and "allocates" an empty element on the array.

This function is just a shortcut to ```parray_push( array, (void*)NULL )```,
and the memory of the pushed element is initialized to zero. */
void* parray_malloc( parray* array )
{
	void*	ptr;

	PROC( "parray_malloc" );
	PARMS( "array", "%p", array );

	if( !( array ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( !( ptr = parray_push( array, (void*)NULL ) ) )
		return ptr;

	memset( ptr, 0, array->size );
	return ptr;
}

/** Pops an element off the array.

The function returns a pointer to the popped item. Because dynamic arrays only
grows and no memory is freed, the returned data pointer is still valid, and will
only be overwritten with the next push operation.

//array// is the pointer to array where to pop an item off.

The function returns the address of the popped item, and (void*)NULL if the
item could not be popped (e.g. array is empty).
*/
void* parray_pop( parray* array )
{
	PROC( "parray_pop" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->top == 0 )
	{
		MSG( "top is zero, no items on the array." );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)array->array + ( --array->top * array->size ) );
}

/** Access an element from the array via its offset position from the left.

//array// is the pointer to array where to access the element from.
//offset// is the offset of the element to be accessed from the array's
base address.

Returns the address of the accessed item, and (void*)NULL if the item could not
be accessed (e.g. if the array is empty or offset is beyond the top of array).

Use parray_raccess() for access items from the top.
*/
void* parray_access( parray* array, size_t offset )
{
	PROC( "parray_access" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%d", offset );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->top == 0 || offset >= array->top )
	{
		MSG( "offset defines an item that is out of bounds within array" );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)array->array + offset * array->size );
}

/** Access an element from the array via its offset position from the right.

//array// is the pointer to array where to access the element from.
//offset// is the offset of the element to be accessed from the array's
base address.

Returns the address of the accessed item, and (void*)NULL if the item could not
be accessed (e.g. if the array is empty or offset is beyond the bottom of
the array).

Use parray_access() for access items from the bottom.
*/
void* parray_raccess( parray* array, size_t offset )
{
	PROC( "parray_raccess" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%d", offset );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	RETURN( parray_access( array, array->top - 1 - offset ) );
}

/** Access top element of the array.

Returns the address of the accessed item, and (void*)NULL if nothing is on
the array.
*/
void* parray_top( parray* array )
{
	PROC( "parray_top" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->count == 0 )
		RETURN( (void*)NULL );

	RETURN( parray_access( array, array->top - 1 ) );
}

/** Access bottom element of the array.

Returns the address of the accessed item, and (void*)NULL if nothing is on
the array.
*/
void* parray_bottom( parray* array )
{
	PROC( "parray_bottom" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->count == 0 )
		RETURN( (void*)NULL );

	RETURN( (char*)array->array );
}


/** Returns the number of elements in a array. */
int parray_count( parray* array )
{
	if( !array )
		return 0;

	return array->top;
}
