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
		wrapper pstrtol() is the pendant to convert a string into a long value.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

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
	Function:		pchar_to_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			If compiled with the -DUNICODE-flag, this functions
					converts a pchar-wide-character string into an UTF-8
					string.
					The string conversion is performed into dynamically
					allocated strings. The function wraps wcstombs(), so
					set_locale() must be done before this function works
					properly.
					
	Parameters:		pchar*		str				Zero-terminated string to
												be converted to UTF-8.
					pboolean	freestr			Frees input-string after
												conversion, if TRUE.
	
	Returns:		char*						Returns the UTF-8 pendant of
												str as dynamically allocated
												memory area.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* pchar_to_uchar( pchar* str, pboolean freestr )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		uchar_to_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			If compiled with the -DUNICODE-flag, this functions
					converts a UTF-8-multi-byte string into an Unicode
					pchar-wide-character string.
					The string conversion is performed into dynamically
					allocated strings. The function wraps mbstowcs(), so
					set_locale() must be done before this function works
					properly.
					
	Parameters:		char*		str				Zero-terminated UTF-8 string to
												be converted into a wide
												character string.
					pboolean	freestr			Frees input-string after
												conversion, if TRUE.
	
	Returns:		pchar*						Returns the wide character
												pendant of str as dynamically
												allocated memory area.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	23.09.2010	Jan Max Meyer	Conversions avoided terminating null under some
								circumstances, adding +1 to the final conversion
								call resolved this problem.
----------------------------------------------------------------------------- */
pchar* uchar_to_pchar( char* str, pboolean freestr )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		plong_to_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts long-value into an allocated char string buffer.

	Parameters:		plong		l			Long value to become converted.
					
	Returns:		char*					Pointer to the newly allocated
											string, which contains the string-
											representation of the long value.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* plong_to_uchar( plong l )
{
	PROC( "plong_to_uchar" );
	PARMS( "l", "%ld", l );
	
	RETURN( pasprintf( "%ld", l ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		plong_to_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts long-value into an allocated pchar
					wide string buffer.

	Parameters:		plong		l			Long value to become converted.
					
	Returns:		pchar*					Pointer to the newly allocated
											wide-character string (if compiled
											with the UNICODE flag!), which 
											contains the wide-string-represen-
											tation of the long value.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* plong_to_pchar( plong l )
{
	PROC( "plong_to_pchar" );
	PARMS( "l", "%ld", l );

	RETURN( Pasprintf( L"%ld", l ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pulong_to_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts unsigned long-value into an allocated char
					string buffer.

	Parameters:		plong		l			Long value to become converted.
					
	Returns:		char*					Pointer to the newly allocated
											string, which contains the string-
											representation of the long value.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* pulong_to_uchar( pulong ul )
{
	PROC( "pulong_to_uchar" );
	PARMS( "l", "%ld", ul );

	RETURN( pasprintf( "%ld", ul ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pulong_to_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts unsigned long-value into an allocated pchar
					wide string buffer.

	Parameters:		pulong		l			Long value to become converted.
					
	Returns:		pchar*					Pointer to the newly allocated
											wide-character string (if compiled
											with the UNICODE flag!), which 
											contains the wide-string-represen-
											tation of the long value.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pulong_to_pchar( pulong ul )
{
	PROC( "pulong_to_pchar" );
	PARMS( "ul", "%ld", ul );
	
	RETURN( Pasprintf( L"%ld", ul ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pdouble_to_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts double-value into an allocated char string buffer.
					Zero-digits behind the decimal will be removed, so "1.65000"
					will become to "1.65" in its string representation.

	Parameters:		pdouble		d			Double value to become converted.
					
	Returns:		char*					Pointer to the newly allocated
											string, which contains the string-
											representation of the double value.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pdouble_to_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts double-value into an allocated pchar string buffer.
					Zero-digits behind the decimal will be removed, so "1.65000"
					will become to "1.65" in its wide string representation.

	Parameters:		pdouble		d			Double value to become converted.
					
	Returns:		char*					Pointer to the newly allocated
											wide-string, which contains the 
											wide string-representation of the
											double value, if compiled with
											the UNICODE-flag.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pdouble_to_pchar( pdouble d )
{
	pchar*		ret;
	pchar*		trail;

	PROC( "pdouble_to_pchar" );
	PARMS( "d", "%lf", d );
	
	if( !( ret = Pasprintf( L"%lf", d ) ) )
		RETURN( (pchar*)NULL );

	VARS( "ret", "%ls", ret );
	
	for( trail = ret + Pstrlen( ret ) - 1;
			*trail == '0'; trail-- )
		;

	*( trail + 1 ) = '\0';

	VARS( "ret", "%ls", ret );	
	RETURN( ret );
}

