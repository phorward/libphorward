/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	union.c
Author:	Jan Max Meyer
Usage:	punion implements a variant data type, which can hold any of
		Phorward Foundation Toolkit' generic types as declared in pbasis.h.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Initializes a punion-element.

//var// is the pointer to the punion-structure to be initialized.
*/
pboolean punion_init( punion* var )
{
	PROC( "punion_init" );
	PARMS( "var", "%p", var );

	if( !var )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( var, 0, sizeof( punion ) );
	var->type = PUNION_NULL;

	RETURN( TRUE );
}

/** Creates a new punion-object.

This object must be released after usage using punion_free(). */
punion* punion_create( void )
{
	punion*		u;

	u = (punion*)pmalloc( sizeof( punion ) );
	punion_init( u );

	return u;
}

/** Frees all memory used by a punion-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PUNION_NULL.

//var// is the pointer to punion structure. */
pboolean punion_reset( punion* var )
{
	PROC( "punion_free" );
	PARMS( "var", "%p", var );

	if( !( var ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	switch( punion_type( var ) )
	{
		case PUNION_STRING:
			pfree( var->val.s );
			break;

		case PUNION_WSTRING:
			pfree( var->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	var->type &= ~0x0F;

	RETURN( TRUE );
}

/** Frees an allocated punion object and all its used memory.

The function always returns (punion*)NULL. */
punion* punion_free( punion* u )
{
	if( !u )
		return (punion*)NULL;

	punion_reset( u );
	pfree( u );

	return (punion*)NULL;
}

