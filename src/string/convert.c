/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
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

/*REMOVE?*/
/** Converts a long-value into an allocated string buffer.

//l// is the long value to be converted.

Returns a pointer to the newly allocated string, which contains the
string-representation of the long value.
*/
char* plong_to_uchar( plong l )
{
	PROC( "plong_to_uchar" );
	PARMS( "l", "%ld", l );

	RETURN( pasprintf( "%ld", l ) );
}

/*REMOVE?*/
/** Converts long-value into an allocated wide-character string buffer.

//l// is the long value to become converted.

Returns a pointer to the newly allocated wide-character string
(if compiled with the UNICODE flag!), which contains the wide-character
representation of the long value.
*/
pchar* plong_to_pchar( plong l )
{
	PROC( "plong_to_pchar" );
	PARMS( "l", "%ld", l );

	RETURN( pawcsprintf( L"%ld", l ) );
}

/*REMOVE?*/
/** Converts unsigned long-value into an allocated string buffer.

//l// is the long value to become converted.

Returns a pointer to the newly allocated string, which contains the
string-representation of the long value.
*/
char* pulong_to_uchar( pulong ul )
{
	PROC( "pulong_to_uchar" );
	PARMS( "l", "%ld", ul );

	RETURN( pasprintf( "%ld", ul ) );
}

/*REMOVE?*/
/** Converts unsigned long-value into an allocated char wide-character buffer.

//l// is the long value to become converted.

Returns a pointer to the newly allocated wide-character string (if compiled
with the UNICODE flag!), which contains the wide-string-representation of the
long value.
*/
pchar* pulong_to_pchar( pulong ul )
{
	PROC( "pulong_to_pchar" );
	PARMS( "ul", "%ld", ul );

	RETURN( pawcsprintf( L"%ld", ul ) );
}

/*REMOVE?*/
/** Converts a double-value into an allocated string buffer.

//d// is the double value to become converted. Zero-digits behind the decimal
dot will be removed after conversion, so "1.65000" will become to "1.65" in its
string representation.

Returns a pointer to the newly allocated string, which contains the
string-representation of the double value.
*/
char* pdouble_to_uchar( pdouble d )
{
	char*		ret;
	char*		trail;

	PROC( "pdouble_to_uchar" );
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

/*REMOVE?*/
/** Converts a double-value into an allocated wide-character string buffer.

//d// is the double value to become converted. Zero-digits behind the decimal
dot will be removed after conversion, so "1.65000" will become to "1.65" in its
string representation.

Returns a pointer to the newly allocated wide-character string, which contains
the string-representation of the double value.
*/
pchar* pdouble_to_pchar( pdouble d )
{
	pchar*		ret;
	pchar*		trail;

	PROC( "pdouble_to_pchar" );
	PARMS( "d", "%lf", d );

	if( !( ret = pawcsprintf( L"%lf", d ) ) )
		RETURN( (pchar*)NULL );

	VARS( "ret", "%ls", ret );

	for( trail = ret + pwcslen( ret ) - 1;
			*trail == '0'; trail-- )
		;

	*( trail + 1 ) = '\0';

	VARS( "ret", "%ls", ret );
	RETURN( ret );
}

