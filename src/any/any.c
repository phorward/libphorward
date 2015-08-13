/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	any.c
Author:	Jan Max Meyer
Usage:	pany implements a universal data type object, which can hold any of
		generic C type.

		The functions in any.conv.c, any.get.c and any.set.c are generated
		by an awk-script reading its definitions from any.h, so the supported
		data types can easily be extended without writing any convert/get/set
		functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Initializes a pany-element.

//val// is the pointer to the pany-structure to be initialized.
*/
pboolean pany_init( pany* val )
{
	PROC( "pany_init" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( val, 0, sizeof( pany ) );
	val->type = PANYTYPE_NULL;

	RETURN( TRUE );
}

/** Creates a new pany-object.

This object must be released after usage using pany_free(). */
pany* pany_create( void )
{
	pany*		val;

	val = (pany*)pmalloc( sizeof( pany ) );
	pany_init( val );

	return val;
}

/** Frees all memory used by a pany-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PANYTYPE_NULL.

//val// is the pointer to pany structure. */
pboolean pany_reset( pany* val )
{
	PROC( "pany_reset" );
	PARMS( "val", "%p", val );

	if( !( val ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	switch( val->type )
	{
		case PANYTYPE_STR:
			pfree( val->val.s );
			break;

		case PANYTYPE_WCS:
			pfree( val->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	val->type = PANYTYPE_NULL;

	RETURN( TRUE );
}

/** Frees an allocated pany object and all its used memory.

The function always returns (pany*)NULL. */
pany* pany_free( pany* val )
{
	if( !val )
		return (pany*)NULL;

	pany_reset( val );
	pfree( val );

	return (pany*)NULL;
}

/* Parse value or accept only preferred type */
/*
pboolean pany_parse( pany* val, char* str, panytype prefer )
{
	if( !( val && str && *str ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( prefer != PANYTYPE_NULL )
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

pboolean pany_set_constant( pany* val, pboolean constant )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( constant )
		val->flags |= PANYFLAG_CONSTANT;
	else
		val->flags &= ~PANYFLAG_CONSTANT;

	return TRUE;
}

pboolean pany_get_constant( pany* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PANYFLAG_CONSTANT );
}

/* Attribute: autoconvert */

pboolean pany_set_autoconvert( pany* val, pboolean autoconvert )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( autoconvert )
		val->flags |= PANYFLAG_AUTOCONVERT;
	else
		val->flags &= ~PANYFLAG_AUTOCONVERT;

	return TRUE;
}

pboolean pany_get_autoconvert( pany* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PANYFLAG_AUTOCONVERT );
}
