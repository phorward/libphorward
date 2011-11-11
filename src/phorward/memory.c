/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	memory.c
Author:	Jan Max Meyer
Usage:	Memory management functions / malloc replacements
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "phorward.h"

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
