/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: String Object Library
Copyright (C) 2010-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	get.c
Author:	Jan Max Meyer
Usage:	String object getter functions
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a pstring-object as an
					zero-terminated pchar-string.
					
	Parameters:		pstring		obj				The pstring-object which content
												will be returned.
	
	Returns:		pchar*						Returns a pointer to the string.
												If the content is empty (NULL),
												an empty pchar-string will be
												returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pstring_get( pstring obj )
{
	PROC( "pstring_set_pchar" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( L"" );

	RETURN( obj->str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_int()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a pstring-object as an int-value.
					
	Parameters:		pstring		obj				The pstring-object which content
												will be set.
	
	Returns:		pint						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_get_int( pstring obj )
{
	PROC( "pstring_get_int" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( 0 );

	RETURN( (pint)Pstrtol( obj->str, (pchar**)NULL, 0 ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_long()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a pstring-object as a long-value.
					
	Parameters:		pstring		obj				The pstring-object which content
												will be set.
	
	Returns:		plong						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong pstring_get_long( pstring obj )
{
	PROC( "pstring_get_long" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( (long)0 );

	RETURN( Pstrtol( obj->str, (pchar**)NULL, 0 ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_ulong()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a pstring-object as a pulong-value.
					
	Parameters:		pstring		obj				The pstring-object which content
												will be set.
	
	Returns:		pulong						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong pstring_get_ulong( pstring obj )
{
	PROC( "pstring_get_ulong" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( (pulong)0 );

	RETURN( Pstrtoul( obj->str, (pchar**)NULL, 0 ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_float()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a pstring-object as a float-value.
					
	Parameters:		pstring		obj				The pstring-object which content
												will be set.
	
	Returns:		pfloat						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat pstring_get_float( pstring obj )
{
	PROC( "pstring_get_float" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( 0 );

	RETURN( (pfloat)Pstrtod( obj->str, (pchar**)NULL ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_double()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a pstring-object as a double-value.
					
	Parameters:		pstring		obj				The pstring-object which content
												will be set.
	
	Returns:		pdouble						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble pstring_get_double( pstring obj )
{
	PROC( "pstring_get_double" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( 0 );

	RETURN( Pstrtod( obj->str, (pchar**)NULL ) );
}

