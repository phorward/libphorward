/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	convert.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

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
