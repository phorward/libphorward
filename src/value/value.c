/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	union.c
Author:	Jan Max Meyer
Usage:	pgvalue implements a variant data type, which can hold any of
		Phorward Foundation Toolkit' generic types as declared in pbasis.h.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Initializes a pgvalue-element.

//val// is the pointer to the pgvalue-structure to be initialized.
*/
pboolean pgvalue_init( pgvalue* val )
{
	PROC( "pgvalue_init" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( val, 0, sizeof( pgvalue ) );
	val->type = PUNION_NULL;

	RETURN( TRUE );
}

/** Creates a new pgvalue-object.

This object must be released after usage using pgvalue_free(). */
pgvalue* pgvalue_create( void )
{
	pgvalue*		u;

	u = (pgvalue*)pmalloc( sizeof( pgvalue ) );
	pgvalue_init( u );

	return u;
}

/** Frees all memory used by a pgvalue-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PUNION_NULL.

//val// is the pointer to pgvalue structure. */
pboolean pgvalue_reset( pgvalue* val )
{
	PROC( "pgvalue_free" );
	PARMS( "val", "%p", val );

	if( !( val ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	switch( pgvalue_type( val ) )
	{
		case PUNION_STRING:
			pfree( val->val.s );
			break;

		case PUNION_WSTRING:
			pfree( val->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	val->type &= ~0x0F;

	RETURN( TRUE );
}

/** Frees an allocated pgvalue object and all its used memory.

The function always returns (pgvalue*)NULL. */
pgvalue* pgvalue_free( pgvalue* u )
{
	if( !u )
		return (pgvalue*)NULL;

	pgvalue_reset( u );
	pfree( u );

	return (pgvalue*)NULL;
}

