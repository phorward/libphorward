/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	range.c
Author:	Jan Max Meyer
Usage:	The pregex object functions.
----------------------------------------------------------------------------- */

#include <phorward.h>

uchar* pregex_range_to_string( pregex_range* range )
{
	uchar*	str;

	PROC( "pregex_range_to_string" );

	if( !range )
	{
		WRONGPARAM;
		RETURN( (uchar*)NULL );
	}

	if( !range->pbegin )
	{
		MSG( "In utf-8/ascii character mode" );
		str = (uchar*)pmalloc( ( range->len + 1 ) * sizeof( uchar ) );
		psprintf( str, "%.*s", range->len, range->begin );
		VARS( "str", "%s", str );
	}
	else
	{
		MSG( "In wide-character mode" );
		str = (uchar*)pmalloc( ( range->len + 1 ) * sizeof( pchar ) );
		Psprintf( (pchar*)str, L"%.*s", range->len, range->pbegin );
		VARS( "str", "%ls", (pchar*)str );
	}

	RETURN( str );
}

