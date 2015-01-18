/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	value.c
Author:	Jan Max Meyer
Usage:	pvalue implements a universal data type object, which can hold any of
		generic C type.

		The functions in value.conv.c, value.get.c and value.set.c are generated
		by an awk-script reading its definitions from value.h, so the supported
		data types can easily be extended without writing any convert/get/set
		functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Initializes a pvalue-element.

//val// is the pointer to the pvalue-structure to be initialized.
*/
pboolean pvalue_init( pvalue* val )
{
	PROC( "pvalue_init" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( val, 0, sizeof( pvalue ) );
	val->type = PGVALUETYPE_NULL;

	RETURN( TRUE );
}

/** Creates a new pvalue-object.

This object must be released after usage using pvalue_free(). */
pvalue* pvalue_create( void )
{
	pvalue*		val;

	val = (pvalue*)pmalloc( sizeof( pvalue ) );
	pvalue_init( val );

	return val;
}

/** Frees all memory used by a pvalue-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PGVALUETYPE_NULL.

//val// is the pointer to pvalue structure. */
pboolean pvalue_reset( pvalue* val )
{
	PROC( "pvalue_reset" );
	PARMS( "val", "%p", val );

	if( !( val ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	switch( val->type )
	{
		case PGVALUETYPE_STR:
			pfree( val->val.s );
			break;

		case PGVALUETYPE_WCS:
			pfree( val->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	val->type = PGVALUETYPE_NULL;

	RETURN( TRUE );
}

/** Frees an allocated pvalue object and all its used memory.

The function always returns (pvalue*)NULL. */
pvalue* pvalue_free( pvalue* val )
{
	if( !val )
		return (pvalue*)NULL;

	pvalue_reset( val );
	pfree( val );

	return (pvalue*)NULL;
}

/* Parse value or accept only preferred type */
/*
pboolean pvalue_parse( pvalue* val, char* str, pvaluetype prefer )
{
	if( !( val && str && *str ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( prefer != PGVALUETYPE_NULL )
	{
		switch( prefer )
		{

		}
	}
}
*/

/*
	Get & Set
*/

/* Attribute: constant */

pboolean pvalue_set_constant( pvalue* val, pboolean constant )
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

pboolean pvalue_get_constant( pvalue* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PGVALUEFLAG_CONSTANT );
}

/* Attribute: autoconvert */

pboolean pvalue_set_autoconvert( pvalue* val, pboolean autoconvert )
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

pboolean pvalue_get_autoconvert( pvalue* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PGVALUEFLAG_AUTOCONVERT );
}
