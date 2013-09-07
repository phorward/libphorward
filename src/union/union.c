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

#include <phorward.h>

/** Initializes a punion-element.

//var// is the pointer to the punion-structure to be initialized.
*/
void punion_init( punion* var )
{
	PROC( "punion_init" );
	PARMS( "var", "%p", var );

	memset( var, 0, sizeof( punion ) );
	var->type = PUNION_NULL;

	VOIDRET;
}

/** Frees all memory used by a punion-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PUNION_NULL.

//var// is the Pointer to punion structure. */
void punion_reset( punion* var )
{
	PROC( "punion_free" );
	PARMS( "var", "%p", var );

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

	VOIDRET;
}

