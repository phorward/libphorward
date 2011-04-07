/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009, 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	direct.c
Author:	Jan Max Meyer
Usage:	Direct regular expression access functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pregex.h"

/*
 * Global variables
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_match()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs a regular expression search on a string, and
					returns an array of matches via a pregex_result-structure,
					which holds pointers to the begin- and end-addresses
					of the matches.
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			Searchstring the pattern
												will be ran on.
					int				flags		Flags for regular
												expression mode switching
					pregex_result**	results		Array of results to the
												matched substrings within
												str. results must be freed
												after usage.
																	
	Returns:		int							Returns the amount of matches.
												If the value is negative,
												it is an error define.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_match( uchar* regex, uchar* str, int flags, pregex_result** results )
{
	int			matches	= 0;
	pregex		re;

	PROC( "pregex_match" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );
	PARMS( "results", "%p", results );
	
	pregex_comp_init( &re, flags );
	if( ( matches = pregex_comp_compile( &re, regex, 0 ) ) < 0 )
		RETURN( matches );

	matches = pregex_comp_match( &re, str, REGEX_NO_CALLBACK, results );
	pregex_comp_free( &re );
	
	VARS( "matches", "%d", matches );

	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_split()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs a regular expression search on a string and uses
					the expression as separator; All strings that where split
					are returned as results-array.
					
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			Searchstring the pattern
												will be ran on.
					int				flags		Flags for regular
												expression mode switching
					pregex_result**	results		Array of results to the
												split substrings. Each element
												of this array contains begin-
												and end-pointer to the
												related strings within the
												input-string str.
																	
	Returns:		int							Returns the amount of matches,
												which is the amount of items
												within the returned
												results-array. If the 
												value is negative,
												it is an error define.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_split( uchar* regex, uchar* str, int flags, pregex_result** results )
{
	int			matches	= 0;
	pregex		re;

	PROC( "pregex_split" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );
	PARMS( "results", "%p", results );
	
	pregex_comp_init( &re, flags );
	if( ( matches = pregex_comp_compile( &re, regex, 0 ) ) < 0 )
		RETURN( matches );

	matches = pregex_comp_split( &re, str, REGEX_NO_CALLBACK, results );
	pregex_comp_free( &re );

	VARS( "matches", "%d", matches );	
	RETURN( matches );	
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_replace()
	
	Author:			Jan Max Meyer
	
	Usage:			Replaces all matches of a regular expression pattern within
					a string with the replacement. Backreferences can be used
					with $x for each opening bracket within the regular 
					expression.
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			String the pattern
												will be ran on.
					uchar*			replacement	String that will be inserted
												as replacement for each pattern
												match. $x backreferences
												can be used
					int				flags		Flags for regular
												expression mode switching
					uchar**			result		Returns a pointer to the result
												string.
																	
	Returns:		int							Returns the amount of matches,
												which is the amount of items
												within the returned
												results-array. If the 
												value is negative,
												it is an error define.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_replace( uchar* regex, uchar* str, uchar* replacement,
	int flags, uchar** result )
{
	int			matches		= 0;
	pregex		re;

	PROC( "pregex_replace" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "replacement", "%s", replacement );
	PARMS( "flags", "%d", flags );
	PARMS( "result", "%p", result );

	pregex_comp_init( &re, flags );
	if( ( matches = pregex_comp_compile( &re, regex, 0 ) ) < 0 )
		RETURN( matches );

	matches = pregex_comp_replace( &re, str, replacement,
					REGEX_NO_CALLBACK, result );
	pregex_comp_free( &re );

	VARS( "matches", "%d", matches );
	RETURN( matches );	
}

