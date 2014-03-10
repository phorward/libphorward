/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	convert.c
Author:	Jan Max Meyer
Usage:	String conversion functions.

		All functions within this module reserve memory for their returned
		data, so there is not always a pendant-function right in here for
		every conversion function. For example, long_to_char() converts a
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
char* pwcs_to_str( wchar_t* str, pboolean freestr )
{
	size_t	size;
	char*	retstr;

	PROC( "pwcs_to_str" );
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

/** This functions converts an UTF-8-multi-byte string into an Unicode
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
wchar_t* pstr_to_wcs( char* str, pboolean freestr )
{
	size_t	size;
	wchar_t*	retstr;

	PROC( "pstr_to_wcs" );
	PARMS( "str", "%s", str );
	PARMS( "freestr", "%s", BOOLEAN_STR( freestr ) );

#ifdef UNICODE
	size = mbstowcs( (wchar_t*)NULL, str, 0 );
	VARS( "size", "%ld", size );

	if( !( retstr = (wchar_t*)pmalloc( ( size + 1 ) * sizeof( wchar_t ) ) ) )
	{
		MSG( "Out of memory?" );
		RETURN( (wchar_t*)NULL );
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

/** Converts a double-value into an allocated string buffer.

//d// is the double value to become converted. Zero-digits behind the decimal
dot will be removed after conversion, so 1.65000 will become "1.65" in its
string representation.

Returns a pointer to the newly allocated string, which contains the
string-representation of the double value. This pointer must be released
by the caller.
*/
char* pdbl_to_str( double d )
{
	char*		ret;
	char*		trail;

	PROC( "pdbl_to_str" );
	PARMS( "d", "%lf", d );

	if( !( ret = pasprintf( "%lf", d ) ) )
		RETURN( (char*)NULL );
	VARS( "ret", "%s", ret );

	for( trail = ret + pstrlen( ret ) - 1;
			*trail == '0'; trail-- )
		;

	*( trail + 1 ) = '\0';

	VARS( "ret", "%s", ret );
	RETURN( ret );
}

/** Converts a double-value into an allocated wide-character string buffer.

//d// is the double value to become converted. Zero-digits behind the decimal
dot will be removed after conversion, so 1.65000 will become L"1.65" in its
wide-character string representation.

Returns a pointer to the newly allocated wide-character string, which contains
the string-representation of the double value. This pointer must be released
by the caller.
*/
wchar_t* pdbl_to_wcs( double d )
{
	wchar_t*		ret;
	wchar_t*		trail;

	PROC( "pdbl_to_wcs" );
	PARMS( "d", "%lf", d );

	if( !( ret = pawcsprintf( L"%lf", d ) ) )
		RETURN( (wchar_t*)NULL );

	VARS( "ret", "%ls", ret );

	for( trail = ret + pwcslen( ret ) - 1;
			*trail == '0'; trail-- )
		;

	*( trail + 1 ) = '\0';

	VARS( "ret", "%ls", ret );
	RETURN( ret );
}

