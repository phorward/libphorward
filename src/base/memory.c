/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	memory.c
Author:	Jan Max Meyer
Usage:	Memory management functions / malloc replacements
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */

/*
 * Defines
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pmalloc()

	Author:			Jan Max Meyer

	Usage:			Like malloc(), but with memory initialization to zero.

	Parameters:		psize		size		Size of memory to be allocated,
											in bytes.

	Returns:		void*					Allocated heap memory pointer on
											success, (void*)NULL else.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* pmalloc( psize size )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		prealloc()

	Author:			Jan Max Meyer

	Usage:			Realloc replacement.

	Parameters:		void*		oldptr		Pointer to be reallocated, if this
											is (void*)NULL, prealloc() will act
											like pmalloc().
	 				psize		size		Size of memory to be reallocated,
											in bytes.

	Returns:		void*					Allocated heap memory pointer on
											success, (void*)NULL else.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* prealloc( void* oldptr, psize size )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pfree()

	Author:			Jan Max Meyer

	Usage:			An implementation of free(), but accepts NULL-pointers
					and returns a (void*)NULL pointer for direct pointer memory
					reset.

					Use it always with

					ptr = pfree( ptr );

	Parameters:		void*	ptr				Pointer to be freed.

	Returns:		void*					Returns always (void*)NULL.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* pfree( void* ptr )
{
	if( ptr )
		free( ptr );

	return (void*)NULL;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pmemdup()

	Author:			Jan Max Meyer

	Usage:			Duplicates a memory entry onto the heap.

	Parameters:		void*		ptr				Pointer to memory
					size_t		size			Size of pointer

	Returns:		void*						Pointer to memory copy.
												Cast this with your type!
												Returns (void*)NULL on error!

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* pmemdup( void* ptr, psize size )
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
