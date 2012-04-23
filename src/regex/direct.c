/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	direct.c
Author:	Jan Max Meyer
Usage:	Direct regular expression access functions running an internal pregex
		object.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_qmatch()

	Author:			Jan Max Meyer

	Usage:			Performs a regular expression search on a string, and
					returns an array of matches via a pregex_range-structure,
					which holds pointers to the begin- and end-addresses
					of the matches.

	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			Searchstring the pattern
												will be ran on.
					int				flags		Flags for regular
												expression mode switching
					pregex_range**	results		Array of results to the
												matched substrings within
												str. results must be freed
												after usage.

	Returns:		int							Returns the amount of matches.
												If the value is negative,
												it is an error define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_qmatch( uchar* regex, uchar* str,
					int flags, pregex_range** results )
{
	int				matches		= 0;
	pregex*			re;
	pregex_range*	range;

	PROC( "pregex_qmatch" );
	PARMS( "regex", "%s", pgetstr( regex ) );
	PARMS( "str", "%s", pgetstr( str ) );
	PARMS( "flags", "%d", flags );
	PARMS( "results", "%p", results );

	if( !( regex && str ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	if( results )
		*results = (pregex_range*)NULL;

	re = pregex_create();
	pregex_set_flags( re, flags );

	if( ( matches = pregex_compile( re, regex, 0 ) ) < 0 )
		RETURN( matches );

	matches = pregex_match( re, str, results );

	re = pregex_free( re );

	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_qsplit()

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
					pregex_range**	results		Array of results to the
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
int pregex_qsplit( uchar* regex, uchar* str,
					int flags, pregex_range** results )
{
	int				matches	= 0;
	pregex*			re;
	pregex_range*	range;

	PROC( "pregex_qsplit" );
	PARMS( "regex", "%s", pgetstr( regex ) );
	PARMS( "str", "%s", pgetstr( str ) );
	PARMS( "flags", "%d", flags );
	PARMS( "results", "%p", results );

	if( !( regex && str ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	re = pregex_create();
	pregex_set_flags( re, flags );

	if( results )
		*results = (pregex_range*)NULL;

	if( ( matches = pregex_compile( re, regex, 0 ) ) < 0 )
		RETURN( matches );

	matches = pregex_split( re, str, results );

	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_qreplace()

	Author:			Jan Max Meyer

	Usage:			Replaces all matches of a regular expression pattern within
					a string with the replacement. Backreferences can be used
					with $x for each opening bracket within the regular
					expression.

	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			String the pattern
												will be ran on.
					uchar*			replace		String that will be inserted
												as replacement for each pattern
												match. $x backreferences
												can be used
					int				flags		Flags for regular
												expression mode switching

	Returns:		uchar*						Returns the string with the
												replacements. This string must
												be freed manually by the caller.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	22.04.2012	Jan Max Meyer	Changed entire function calling style to new
								regular expression object handling.
----------------------------------------------------------------------------- */
uchar* pregex_qreplace( uchar* regex, uchar* str, uchar* replace, int flags )
{
	uchar*			ret;
	int				matches		= 0;
	pregex*			re;

	PROC( "pregex_qreplace" );
	PARMS( "regex", "%s", pgetstr( regex ) );
	PARMS( "str", "%s", pgetstr( str ) );
	PARMS( "replace", "%s", pgetstr( replace ) );
	PARMS( "flags", "%d", flags );

	if( !( regex && str ) )
	{
		WRONGPARAM;
		RETURN( (uchar*)NULL );
	}

	re = pregex_create();
	pregex_set_flags( re, flags );

	if( pregex_compile( re, regex, 0 ) < 0 )
	{
		pregex_free( re );
		RETURN( (uchar*)NULL );
	}

	if( !( ret = pregex_replace( re, str, replace ) ) )
	{
		pregex_free( re );
		RETURN( (uchar*)NULL );
	}

	re->tmp_str = (uchar*)NULL;
	pregex_free( re );

	VARS( "ret", "%s", ret );
	RETURN( ret );
}

