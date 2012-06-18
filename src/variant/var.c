/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	var.c
Author:	Jan Max Meyer
Usage:	pvar implements a variant data type, which can hold any of
		Phorward Foundation Toolkit' generic types as declared in pbasis.h.
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
	Function:		pvar_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a pvar-element.
					
	Parameters:		pvar*		var			Pointer to pvar structure.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pvar_init( pvar* var )
{
	PROC( "pvar_init" );
	PARMS( "var", "%p", var );

	memset( var, 0, sizeof( pvar ) );
	var->type = PVAR_NULL;

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_reset()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees all memory used by a pvar-element.
					All memory used by the element is freed, and the variant
					structure is reset to be of type PVAR_NULL.
					
	Parameters:		pvar*		var			Pointer to pvar structure.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pvar_reset( pvar* var )
{
	PROC( "pvar_free" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_STRING:
			pfree( var->val.s );
			break;

		case PVAR_WSTRING:
			pfree( var->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	var->type &= ~0x0F;

	VOIDRET;
}

