/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2009-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	direct.c
Author:	Jan Max Meyer
Usage:	Direct regular expression access functions running an internal pregex
		object.
----------------------------------------------------------------------------- */

#include <phorward.h>

/** Performs a regular expression match on a string, and returns an array of
matches via a pregex_range-structure, which holds pointers to the begin- and
end-addresses of all matches.

//regex// is the regular expression pattern to be processed.
//str// is the string on which the pattern will be executed on.
//flags// are the flags for regular expression mode switching.
//results// is the array of results to the matched substrings within //str//.
//results// must be released using pfree() after its usage.

Returns the number of matches, which is the amount of result entries in the
returned array //results//. If the value is negative, an error occured, and
an ERR_-define can be used to find out the reason.
*/
int pregex_qmatch( uchar* regex, uchar* str,
					int flags, pregex_range** results )
{
	int				matches		= 0;
	pregex*			re;

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

/** Performs a regular expression search on a string and uses the expression as
separator; All strings that where split are returned as //results//-array.

//regex// is the regular expression pattern to be processed.
//str// is the string on which the pattern will be executed on.
//flags// are the flags for regular expression mode switching.
//results// is the array of results to the matched substrings within //str//.
//results// must be released using pfree() after its usage.

Returns the number of split substrings, which is the amount of result entries in
the returned array //results//. If the value is negative, an error occured, and
an ERR_-define can be used to find out the reason.
*/
int pregex_qsplit( uchar* regex, uchar* str,
					int flags, pregex_range** results )
{
	int				matches	= 0;
	pregex*			re;

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

/** Replaces all matches of a regular expression pattern within a string with
the replacement. Backreferences can be used with $x for each opening bracket
within the regular expression.

//regex// is the regular expression pattern to be processed.
//str// is the string on which the pattern will be executed on.
//replace// is the string that will be inserted as replacement for each pattern
match. $x backreferences can be used.
//flags// are the flags for regular expression mode switching.

Returns an allocated pointer to the generated string with the replacements.
This string must be released after its existence is no longer required by the
caller using pfree().
*/
uchar* pregex_qreplace( uchar* regex, uchar* str, uchar* replace, int flags )
{
	uchar*			ret;
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

