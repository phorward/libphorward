/* -MODULE----------------------------------------------------------------------
Phorward Basis Library :: Conversion functions
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	convert.c
Author:	Jan Max Meyer
Usage:	Conversion functions for data type and storage type conversion
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pbasis.h"

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
	
	Returns:		uchar*						Returns the UTF-8 pendant of
												str as dynamically allocated
												memory area.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pchar_to_uchar( pchar* str, pboolean freestr )
{
	psize	size;
	uchar*	retstr;

	PROC( "pchar_to_uchar" );
	PARMS( "str", "%ls", str );
	PARMS( "freestr", "%s", BOOLEAN_STR( freestr ) );

#ifdef UNICODE
	size = wcstombs( (uchar*)NULL, str, 0 );
	VARS( "size", "%ld", size );

	if( !( retstr = (uchar*)pmalloc( ( size + 1 ) * sizeof( uchar ) ) ) )
	{
		MSG( "Out of memory?" );
		RETURN( (uchar*)NULL );
	}

	wcstombs( retstr, str, size );

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
					
	Parameters:		uchar*		str				Zero-terminated UTF-8 string to
												be converted into a wide
												character string.
					pboolean	freestr			Frees input-string after
												conversion, if TRUE.
	
	Returns:		pchar*						Returns the wide character
												pendant of str as dynamically
												allocated memory area.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* uchar_to_pchar( uchar* str, pboolean freestr )
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

	mbstowcs( retstr, str, size );

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
	Function:		uchar_to_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			If compiled with the -DUNICODE-flag, this functions
					converts a UTF-8-multi-byte string into an Unicode
					pchar-wide-character string.
					The string conversion is performed into dynamically
					allocated strings. The function wraps mbstowcs(), so
					set_locale() must be done before this function works
					properly.
					
	Parameters:		uchar*		str				Zero-terminated UTF-8 string to
												be converted into a wide
												character string.
					pboolean	freestr			Frees input-string after
												conversion, if TRUE.
	
	Returns:		pchar*						Returns the wide character
												pendant of str as dynamically
												allocated memory area.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
