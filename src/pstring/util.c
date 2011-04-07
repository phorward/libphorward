/* -MODULE----------------------------------------------------------------------
Phorward String Object Library
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:	util.c
Author:	Jan Max Meyer
Usage:	Pstring-object utility functions
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_compare()
	
	Author:			Jan Max Meyer
	
	Usage:			Function to compare Pstring-objects.
					
	Parameters:		Pstring		obj1			First compare operand
					Pstring		obj2			Second operand
					pboolean	case_ins		TRUE: Case-insensitive 
													comparison
												FALSE: Case-insensitive
													comparison
	
	Returns:		int							Returns an integer less than,
												equal to or greater than
												zero.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_compare( Pstring obj1, Pstring obj2, pboolean case_ins )
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

