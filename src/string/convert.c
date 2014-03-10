/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	convert.c
Author:	Jan Max Meyer
Usage:	Conversion functions for data type and storage type conversion.
		All functions within this module reserve memory for their returned
		data, so there is not always a pendant-function right in here for
		every conversion function. For example, plong_to_uchar() converts a
		long-value into an allocated char-string, but the C standard library
		wrapper strtol() is the pendant to convert a string into a long value.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** This functions converts a wide-character string into an UTF-8 string.

The string conversion is performed into dynamically allocated memory.
The function wraps the system function wcstombs(), so set_locale() must be
called before this function works properly.

//str// is the zero-terminated string to be converted to UTF-8.
//freestr// defines if the input-string shall be freed after successfull
conversion, if set to TRUE.

Returns the UTF-8 character pendant of //str// as pointer to dynamically
allocated memory.
*/
char* wchar_to_u8( pchar* str, pboolean freestr )
{
	psize	size;
	char*	retstr;

	PROC( "pchar_to_uchar" );
	PARMS( "str", "%ls", str );
	PARMS( "freestr", "%s", BOOLEAN_STR( freestr ) );

#ifdef UNICODE
	size = wcstombs( (char*)NULL, str, 0 );
	VARS( "size", "%ld", size );

	if( !( retstr = (char*)pmalloc( ( size + 1 ) * sizeof( char ) ) ) )
	{
		MSG( "Out of memory?" );
		RETURN( (char*)NULL );
	}

	wcstombs( retstr, str, size + 1 );

	if( freestr )
		pfree( str );
#else
	MSG( "Nothing to do, this function is compiled without UNICODE-flag!" );

	if( freestr )
		retstr = str;
	else
		retstr = pstrdup( str );
#endif

	VARS( "retstr", "%s", retstr );
	RETURN( retstr );
}

/** This functions converts a UTF-8-multi-byte string into an Unicode
wide-character string.

The string conversion is performed into dynamically allocated memory.
The function wraps mbstowcs(), so set_locale() must be done before this
function works properly.

//str// is the zero-terminated multi-byte-character string to be converted
into a wide-character string.
//freestr// defines if the input-string shall be freed after successfull
conversion, if set to TRUE.

Returns the wide-character pendant of //str// as pointer to dynamically
allocated memory.
*/
pchar* u8_to_wchar( char* str, pboolean freestr )
{
	psize	size;
	pchar*	retstr;

	PROC( "uchar_to_pchar" );
	PARMS( "str", "%s", str );
	PARMS( "freestr", "%s", BOOLEAN_STR( freestr ) );

#ifdef UNICODE
	size = mbstowcs( (pchar*)NULL, str, 0 );
	VARS( "size", "%ld", size );

	if( !( retstr = (pchar*)pmalloc( ( size + 1 ) * sizeof( pchar ) ) ) )
	{
		MSG( "Out of memory?" );
		RETURN( (pchar*)NULL );
	}

	mbstowcs( retstr, str, size + 1 ); /* JMM 23.09.2010 */

	if( freestr )
		pfree( str );
#else
	MSG( "Nothing to do, this function is compiled without UNICODE-flag!" );

	if( freestr )
		retstr = str;
	else
		retstr = pstrdup( str );
#endif

	VARS( "retstr", "%ls", retstr );
	RETURN( retstr );
}
