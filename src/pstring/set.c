/* -MODULE----------------------------------------------------------------------
Phorward String Object Library
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:	set.c
Author:	Jan Max Meyer
Usage:	String object setter functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <pbasis.h>
#include "pstring.h"
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
static void _pstring_strip_floating_point( Pstring obj )
{
	pchar*		p;

	PROC( "_pstring_strip_floating_point" );
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
	*(p+1) = '\0';
	
	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according to a
					zero-terminated pchar-String.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					pchar*		str				A pointer to the zero-terminated
												pchar-string to be set.
	
	Returns:		int							Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_set_pchar( Pstring obj, pchar* str )
{
	PROC( "pstring_set_pchar" );
	PARMS( "obj", "%p", obj );
	PARMS( "str", "%S", str );
	
	pstring_reset( obj );

	if( !str )
		RETURN( ERR_OK );

	pstring_PUT( obj, str, Pstrlen( str ) );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according to a
					zero-terminated uchar-String.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					uchar*		str				A pointer to the zero-terminated
												uchar-string to be set.
	
	Returns:		int							Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_set_uchar( Pstring obj, uchar* str )
{
	PROC( "pstring_set_uchar" );
	PARMS( "obj", "%p", obj );
	PARMS( "str", "%s", str );
	
	pstring_reset( obj );

	if( !str )
		RETURN( ERR_OK );

#ifdef UTF8
	obj->len = u8_strlen( str );
#else
	obj->len = pstrlen( str );
#endif
	VARS( "obj->len", "%ld", obj->len );

	if( !ALLOC_STR( obj, obj->len ) )
		RETURN( ERR_MEM );

#ifdef UNICODE
	mbstowcs( obj->str, str, obj->len );
	VARS( "obj->str", "%S", obj->str );
#else
	strcpy( obj->str, str );
	VARS( "obj->str", "%s", obj->str );
#endif

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_int()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according
					to an integer-typed value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					pint			i			Integer value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_int( Pstring obj, pint i )
{
	PROC( "pstring_set_int" );
	PARMS( "obj", "%p", obj );
	PARMS( "i", "%d", i );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

#ifdef UNICODE
	swprintf( obj->str, NUMERIC_VALUE_LEN, L"%d", i );
	VARS( "obj->str", "%S", obj->str );
#else
	sprintf( obj->str, "%d", i );
	VARS( "obj->str", "%s", obj->str );
#endif

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_long()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according
					to an long-typed value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					long		l				Long value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_long( Pstring obj, plong l )
{
	PROC( "pstring_set_long" );
	PARMS( "obj", "%p", obj );
	PARMS( "l", "%ld", l );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

#ifdef UNICODE
	swprintf( obj->str, NUMERIC_VALUE_LEN, L"%ld", l );
	VARS( "obj->str", "%S", obj->str );
#else
	sprintf( obj->str, "%ld", l );
	VARS( "obj->str", "%s", obj->str );
#endif

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_ulong()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according
					to an pulong-typed value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					pulong		ul				Long value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_ulong( Pstring obj, pulong ul )
{
	PROC( "pstring_set_long" );
	PARMS( "obj", "%p", obj );
	PARMS( "ul", "%ld", ul );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

#ifdef UNICODE
	swprintf( obj->str, NUMERIC_VALUE_LEN, L"%ld", ul );
	VARS( "obj->str", "%S", obj->str );
#else
	sprintf( obj->str, "%ld", ul );
	VARS( "obj->str", "%s", obj->str );
#endif

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_float()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according
					to an float-typed value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					float		f				Float value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_float( Pstring obj, pfloat f )
{
	PROC( "pstring_set_float" );
	PARMS( "obj", "%p", obj );
	PARMS( "f", "%f", f );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

#ifdef UNICODE
	swprintf( obj->str, NUMERIC_VALUE_LEN, L"%f", f );
	_pstring_strip_floating_point( obj );
	VARS( "obj->str", "%S", obj->str );
#else
	sprintf( obj->str, "%f", f );
	_pstring_strip_floating_point( obj );
	VARS( "obj->str", "%s", obj->str );
#endif

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_set_double()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the content of a Pstring-object according
					to an double-typed value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
					double		d				Double value to be set.
	
	Returns:		pint						Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_set_double( Pstring obj, pdouble d )
{
	PROC( "pstring_set_double" );
	PARMS( "obj", "%p", obj );
	PARMS( "d", "%lf", d );
	
	pstring_reset( obj );

	if( !ALLOC_STR( obj, NUMERIC_VALUE_LEN ) )
		RETURN( ERR_MEM );

#ifdef UNICODE
	swprintf( obj->str, NUMERIC_VALUE_LEN, L"%lf", d );
	_pstring_strip_floating_point( obj );
	VARS( "obj->str", "%S", obj->str );
#else
	sprintf( obj->str, "%lf", d );
	_pstring_strip_floating_point( obj );
	VARS( "obj->str", "%s", obj->str );
#endif

	obj->len = Pstrlen( obj->str );
	VARS( "obj->len", "%ld", obj->len );

	RETURN( ERR_OK );
}

