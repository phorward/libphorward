/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bitset.c
Author:	Jan Max Meyer
Usage:	Bitset processing library
----------------------------------------------------------------------------- */

#include <phorward.h>

/** Allocates a new bitset.

//size// is the number of bits to be accessable within the created bitset.

Returns a pointer of type **bitset** which is the allocated bitset with the
given site of bits.
*/
bitset bitset_create( int size )
{
	if( size <= 0 )
	{
		WRONGPARAM;
		return (bitset)NULL;
	}

	return (bitset)calloc( (size>>3) + (size % 8 ? 2 : 1), 1);
}

/** Toggles a bit within a set to a desired state (ON or OFF).

//set// is a pointer to the set containing the bit to be toggled.
//bit// is a the offset of the bit which shall be switched.
//state// is the state - FALSE or TRUE.
*/
void bitset_set( bitset set, int bit, pboolean state )
{
	int idx = ( ( bit>>3 ) );

	if( !( set && bit >= 0 ) )
	{
		WRONGPARAM;
		return;
	}

	if( state )
		set[idx] |= ( 1 << ( bit % 8 ) );
	else
		set[idx] &= ( 0xFF ^ (1 << ( bit % 8 ) ) );
}

/** Returns the state of a desired bit in a set.

//set// is the pointer to the set containing the bit to be gotten.
//bit// is the offset of the bit which should be checked.

Returns a **pboolean** if the desired bit is set or not.
*/
pboolean bitset_get( bitset set, int bit )
{
	if( !( set && bit >= 0 ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return (pboolean)( set[(bit>>3)] & ( 1 << ( bit % 8 ) ) );
}

/** Copies a bitset into a new one.

//size// is the size of the bitset //source//.
//source// is the source set

Returns a **bitset** to the copy of the set.
*/
bitset bitset_copy( int size, bitset source )
{
	bitset	dest;
	
	if( !( size > 0 && source ) )
	{
		WRONGPARAM;
		return (bitset)NULL;
	}

	if( !( dest = bitset_create( size ) ) )
		return (bitset)NULL;

	memcpy( dest, source, (size>>3) + (size % 8 ? 2 : 1) );

	return dest;
}

