/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: String Object Library
Copyright (C) 2010-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	set.c
Author:	Jan Max Meyer
Usage:	String object setter functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>
#include "local.h"

/*
 * Global variables
 */

/*
 * Defines
 */

/*
 * Functions
 */

/*
	Utility function for removing trailing zero-floating point digits
	after a float conversion
*/
static void strip_floating_point( pstring obj )
{
	pchar*		p;

	PROC( "strip_floating_point" );
	PARMS( "obj", "%p", obj );

	/* Strip trailing zeros */
	MSG( "Stripping trailing zeros, if any" );
	for( p = obj->str + Pstrlen( obj->str ) - 1; *p; p-- )
	{
		if( *p == '.' )
		{
			p--;
			break;
		}

		if( *p != '0' )
			break;
	}
	*( p + 1 ) = '\0';
	
	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a pstring-object according to a
					zero-terminated pchar-String.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					pchar*		str				A pointer to the zero-terminated
												pchar-string to be set.
	
	Returns:		int							Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_set( pstring obj, pchar* str )
{
	PROC( "pstring_set" );
	PARMS( "obj", "%p", obj );
	PARMS( "str", "%S", str );
	
	pstring_reset( obj );

	if( !str )
		RETURN( ERR_OK );

	pstring_PUT( obj, str, Pstrlen( str ) );

	RETURN( ERR_OK );
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_int()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a pstring-object according
					to an integer-typed value.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					pint			i			Integer value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_int( pstring obj, pint i )
{
	PROC( "pstring_set_int" );
	PARMS( "obj", "%p", obj );
	PARMS( "i", "%d", i );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

	Psprintf( obj->str, L"%d", i );
	VARS( "obj->str", "%S", obj->str );

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_long()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a pstring-object according
					to an long-typed value.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					long		l				Long value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_long( pstring obj, plong l )
{
	PROC( "pstring_set_long" );
	PARMS( "obj", "%p", obj );
	PARMS( "l", "%ld", l );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

	Psprintf( obj->str, L"%ld", l );

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_ulong()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a pstring-object according
					to an pulong-typed value.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					pulong		ul				Long value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_ulong( pstring obj, pulong ul )
{
	PROC( "pstring_set_long" );
	PARMS( "obj", "%p", obj );
	PARMS( "ul", "%ld", ul );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

	Psprintf( obj->str, L"%ld", ul );

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_float()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a pstring-object according
					to an float-typed value.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					float		f				Float value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_float( pstring obj, pfloat f )
{
	PROC( "pstring_set_float" );
	PARMS( "obj", "%p", obj );
	PARMS( "f", "%f", f );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

	Psprintf( obj->str, L"%f", f );
	strip_floating_point( obj );

	VARS( "obj->str", "%ls", obj->str );

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_double()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a pstring-object according
					to an double-typed value.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					double		d				Double value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_double( pstring obj, pdouble d )
{
	PROC( "pstring_set_double" );
	PARMS( "obj", "%p", obj );
	PARMS( "d", "%lf", d );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

	Psprintf( obj->str, L"%lf", d );
	strip_floating_point( obj );
	VARS( "obj->str", "%ls", obj->str );

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

