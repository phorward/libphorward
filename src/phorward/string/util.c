/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: String Object Library
Copyright (C) 2010-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	util.c
Author:	Jan Max Meyer
Usage:	pstring-object utility functions
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
	Function:		pstring_compare()
	
	Author:			Jan Max Meyer
	
	Usage:			Function to compare pstring-objects.
					
	Parameters:		pstring		obj1			First compare operand.
					pstring		obj2			Second operand.
					BOOLEAN		caseins			TRUE: Case-insensitive 
												comparison
												FALSE: Case-insensitive
												comparison
	
	Returns:		int							Returns an integer less than,
												equal to or greater than
												zero.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_compare( pstring obj1, pstring obj2, pboolean case_ins )
{
	int		rc		= 0;
	psize	p;
	pchar	u1;
	pchar	u2;

	PROC( "pstring_compare" );
	PARMS( "obj1", "%p", obj1 );
	PARMS( "obj2", "%p", obj2 );
	PARMS( "case_ins", "%s", BOOLEAN_STR( case_ins ) );
	
	if( !( obj1 && obj1->str ) )
		rc = 1;
	else if( !( obj2 && obj2->str ) )
		rc = -1;
	else
	{
		if( obj1->len < obj2->len )
			rc = 1;
		else if( obj1->len > obj2->len )
			rc = -1;
		else
		{
			MSG( "Sequencial character scan required" );
			for( p = 0; p < obj1->len && !rc; p++ )
			{
				if( case_ins )
				{
					u1 = towupper( obj1->str[ p ] );
					u2 = towupper( obj2->str[ p ] );

					if( u1 > u2 )
						rc = -1;
					else if( u1 < u2 )
						rc = 1;
				}
				else
				{
					if( obj1->str[ p ] > obj2->str[ p ] )
						rc = -1;
					else if( obj1->str[ p ] < obj2->str[ p ] )
						rc = 1;
				}
			}
		}
	}

	VARS( "rc", "%d", rc );
	RETURN( rc * -1 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_from_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Turn a zero-terminated uchar-(multi)byte-character string
					into a pstring-Object's content.
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be set.
					uchar*		str				A pointer to the zero-terminated
												uchar-string to be set.
	
	Returns:		int							Returns an error-code, ERR_OK
												if everything was fine.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_from_uchar( pstring obj, uchar* str )
{
	PROC( "pstring_from_uchar" );
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
	VARS( "obj->str", "%ls", obj->str );
#else
	Pstrcpy( obj->str, str );
	VARS( "obj->str", "%s", obj->str );
#endif

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_to_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as an
					zero-terminated (multibyte)-uchar-string.
					The pointer is returned, and must be freed by the caller. 
					
	Parameters:		pstring		obj				The Pstring-object which content
												will be returned as uchar.
	
	Returns:		uchar*						Returns a pointer to the string.
												If the content is empty (NULL),
												an empty uchar-string will be
												returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstring_to_uchar( pstring obj )
{
	uchar*		ptr;

	PROC( "pstring_to_uchar" );
	PARMS( "obj", "%p", obj );
	
	VARS( "str", "%ls", obj->str );
	
	if( pstring_empty( obj ) )
	{
		RETURN( pstrdup( "" ) );
	}

	if( !( ptr = pchar_to_uchar( obj->str, FALSE ) ) )
	{
		MSG( "Am I out of memory?" );
		RETURN( (uchar*)NULL );
	}

	VARS( "ptr", "%s", ptr );
	RETURN( ptr );
}

