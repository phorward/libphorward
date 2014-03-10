/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	value.c
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
	val->type = PGVALUE_NULL;

	RETURN( TRUE );
}

/** Creates a new pgvalue-object.

This object must be released after usage using pgvalue_free(). */
pgvalue* pgvalue_create( void )
{
	pgvalue*		val;

	val = (pgvalue*)pmalloc( sizeof( pgvalue ) );
	pgvalue_init( val );

	return val;
}

/** Frees all memory used by a pgvalue-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PGVALUE_NULL.

//val// is the pointer to pgvalue structure. */
pboolean pgvalue_reset( pgvalue* val )
{
	PROC( "pgvalue_reset" );
	PARMS( "val", "%p", val );

	if( !( val ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	switch( pgvalue_type( val ) )
	{
		case PGVALUE_STRING:
			pfree( val->val.s );
			break;

		case PGVALUE_WSTRING:
			pfree( val->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	val->type = PGVALUE_NULL;

	RETURN( TRUE );
}

/** Frees an allocated pgvalue object and all its used memory.

The function always returns (pgvalue*)NULL. */
pgvalue* pgvalue_free( pgvalue* val )
{
	if( !val )
		return (pgvalue*)NULL;

	pgvalue_reset( val );
	pfree( val );

	return (pgvalue*)NULL;
}

/*
	Get & Set
*/

/* Attribute: constant */

pboolean pg_value_set_constant( pgvalue* val, pboolean constant )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( constant )
		val->flags |= PGVALUEFLAG_CONSTANT;
	else
		val->flags &= ~PGVALUEFLAG_CONSTANT;

	return TRUE;
}

pboolean pg_value_get_constant( pgvalue* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PGVALUEFLAG_CONSTANT );
}

/* Attribute: autoconvert */

pboolean pg_value_set_autoconvert( pgvalue* val, pboolean autoconvert )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( autoconvert )
		val->flags |= PGVALUEFLAG_AUTOCONVERT;
	else
		val->flags &= ~PGVALUEFLAG_AUTOCONVERT;

	return TRUE;
}

pboolean pg_value_get_autoconvert( pgvalue* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PGVALUEFLAG_AUTOCONVERT );
}
