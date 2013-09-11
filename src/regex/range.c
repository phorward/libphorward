/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	range.c
Author:	Jan Max Meyer
Usage:	Functions for handling pregex_range-structures.
----------------------------------------------------------------------------- */

#include <phorward.h>

/** Extracts a string match described by a pregex_range-structure.

This function is used to quickly extract a substring by duplicating the
memory range described by the provided //range// structure. The function pays
attention if the provided range is a range for a wide-character or UTF-8
character range to be duplicated, so the returned pointer needs to be casted to
(pchar*) when working with wide-character strings.

The returned pointer is an allocated memory address where the zero-terminated
copy of the extracted string remains, and has to be released with pfree() when
its existence is not longer required.
*/
char* pregex_range_to_string( pregex_range* range )
{
	char*	str;

	PROC( "pregex_range_to_string" );

	if( !range )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( !range->pbegin )
	{
		MSG( "In utf-8/ascii character mode" );
		str = (char*)pmalloc( ( range->len + 1 ) * sizeof( char ) );
		psprintf( str, "%.*s", range->len, range->begin );
		VARS( "str", "%s", str );
	}
	else
	{
		MSG( "In wide-character mode" );
		str = (char*)pmalloc( ( range->len + 1 ) * sizeof( pchar ) );
		pwcsprintf( (pchar*)str, L"%.*s", range->len, range->pbegin );
		VARS( "str", "%ls", (pchar*)str );
	}

	RETURN( str );
}

