/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	bitset.c
Author:	Jan Max Meyer
Usage:	Bitset processing library
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pbasis.h"

/*
 * Global variables
 */


/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		bitset_create()
	
	Author:			Jan Max Meyer
	
	Usage:			Allocates a new set.
					
	Parameters:		int			size			Number of bits to be accessable
												within the created set.
	
	Returns:		bitset						Pointer to the allocated bitset
												with the given site of bits.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
bitset bitset_create( int size )
{
	if( size <= 0 )
		return (bitset)NULL;

	return (bitset)calloc( (size>>3) + (size % 8 ? 2 : 1), 1);
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		bitset_set()
	
	Author:			Jan Max Meyer
	
	Usage:			Toggles a bit within a set to a desired state
					(ON or OFF).
					
	Parameters:		bitset		set				Pointer to the set containing
												the bit to be toggled.
					int			bit				The bit which should be switched.
					int			state			State - 0 or 1.
	
	Returns:		int							0 in case of an error, else 1.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int bitset_set( bitset set, int bit, int state )
{
	int idx = ( ( bit>>3 ) );

	if( set == (bitset)NULL || bit < 0 )
		return 0;

	if( state )
		set[idx] |= ( 1 << ( bit % 8 ) );
	else
		set[idx] &= ( 0xFF ^ (1 << ( bit % 8 ) ) );

	return 1;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		bitset_get()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the state of a desired bit in a set.
					
	Parameters:		bitset		set			Pointer to the set containing
												the bit to be gotten.
					int			bit				The bit which should be checked.
	
	Returns:		int							0 = Bit is not set
												1 = Bit is set
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int bitset_get( bitset set, int bit )
{
	return (int)( set[(bit>>3)] & ( 1 << ( bit % 8 ) ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		bitset_copy()
	
	Author:			Jan Max Meyer
	
	Usage:			Copies a set.
					
	Parameters:		int			size			Size of the set
					bitset		source			Source set
	
	Returns:		bitset						Pointer to the copy of the set.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
bitset bitset_copy( int size, bitset source )
{
	bitset	dest;

	if( !( dest = bitset_create( size ) ) )
		return (bitset)NULL;

	memcpy( dest, source, (size>>3) + (size % 8 ? 2 : 1) );

	return dest;
}

