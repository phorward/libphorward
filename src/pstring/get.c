/* -MODULE----------------------------------------------------------------------
Phorward String Object Library
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:	get.c
Author:	Jan Max Meyer
Usage:	String object getter functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <pbasis.h>
#include "pstring.h"
#include "local.h"

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
	Function:		pstring_get_pchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as an
					zero-terminated pchar-string.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be returned.
	
	Returns:		pchar*						Returns a pointer to the string.
												If the content is empty (NULL),
												an empty pchar-string will be
												returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pstring_get_pchar( Pstring obj )
{
	PROC( "pstring_set_pchar" );
	PARMS( "obj", "%p", obj );
	
	if( pstring_empty( obj ) )
		RETURN( L"" );

	RETURN( obj->str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_uchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as an
					zero-terminated uchar-string. The pointer returned is
					associated by the Pstring-object via the tmp-pointer, and
					is freed with the next conversion call or the object
					destruction!
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be returned as uchar.
	
	Returns:		uchar*						Returns a pointer to the string.
												If the content is empty (NULL),
												an empty uchar-string will be
												returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstring_get_uchar( Pstring obj )
{
	size_t		alloc_size;

	PROC( "pstring_get_uchar" );
	PARMS( "obj", "%p", obj );
	
	VARS( "str", "%S", obj->str );
	
	pstring_drop_tmp( obj, TRUE );
		
	if( pstring_empty( obj ) )
	{
		RETURN( "" );
	}

#ifdef UNICODE
	if( ( alloc_size = wcstombs( (uchar*)NULL, obj->str, 0 ) ) < 0 )
	{
		MSG( "Can't convert according to the current locale!" );
		RETURN( (uchar*)NULL );
	}
#else
	alloc_size = Pstrlen( obj->str );
#endif

	VARS( "alloc_size", "%ld", alloc_size );
	if( !( obj->tmp = (pbyte*)pmalloc( ( alloc_size + 1 )
							* sizeof( uchar ) ) ) )
	{
		MSG( "Can't allocate memory" );
		RETURN( (uchar*)NULL );
	}
	
#ifdef UNICODE
	wcstombs( (uchar*)obj->tmp, obj->str, alloc_size );
#else
	Pstrcpy( (uchar*)obj->tmp, obj->str );
#endif

	VARS( "(uchar*)obj->tmp", "%s", (uchar*)obj->tmp );
	RETURN( (uchar*)obj->tmp );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_uchar_distinct()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as an
					zero-terminated uchar-string.
					Unlike pstring_get_uchar(), the memory of  the returned
					uchar-array is not associated with the Pstring-object, it
					is returned as a distinct unit, which must be manually
					freed by the caller.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be returned as uchar.
	
	Returns:		uchar*						Returns a pointer to the string.
												If the content is empty (NULL),
												an empty uchar-string will be
												returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstring_get_uchar_distinct( Pstring obj )
{
	uchar*		ret;

	PROC( "pstring_get_uchar_distinct" );
	PARMS( "obj", "%p", obj );
	
	VARS( "str", "%S", obj->str );
	
	if( !( ret = pstring_get_uchar( obj ) ) )
	{
		MSG( "pstring_get_uchar() returned NULL-pointer, not good!" );
		RETURN( (uchar*)NULL );
	}

	pstring_drop_tmp( obj, FALSE );	
	if( !*ret )
		ret = pstrdup( "" );
	
	VARS( "ret", "%s", ret );
	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_int()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as an int-value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
	
	Returns:		pint						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pstring_get_int( Pstring obj )
{
	PROC( "pstring_get_int" );
	PARMS( "obj", "%p", obj );
	
	pstring_drop_tmp( obj, TRUE );
	
	if( pstring_empty( obj ) )
		RETURN( 0 );

#ifdef UNICODE
	RETURN( (pint)wcstol( obj->str, (pchar**)NULL, 0 ) );
#else
	RETURN( (pint)strtol( obj->str, (pchar**)NULL, 0 ) );
#endif
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_long()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as a long-value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
	
	Returns:		plong						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong pstring_get_long( Pstring obj )
{
	PROC( "pstring_get_long" );
	PARMS( "obj", "%p", obj );
	
	pstring_drop_tmp( obj, TRUE );
	
	if( pstring_empty( obj ) )
		RETURN( (long)0 );

#ifdef UNICODE
	RETURN( wcstol( obj->str, (pchar**)NULL, 0 ) );
#else
	RETURN( strtol( obj->str, (pchar**)NULL, 0 ) );
#endif
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_ulong()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as a pulong-value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
	
	Returns:		pulong						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong pstring_get_ulong( Pstring obj )
{
	PROC( "pstring_get_ulong" );
	PARMS( "obj", "%p", obj );
	
	pstring_drop_tmp( obj, TRUE );
	
	if( pstring_empty( obj ) )
		RETURN( (pulong)0 );

#ifdef UNICODE
	RETURN( wcstoul( obj->str, (pchar**)NULL, 0 ) );
#else
	RETURN( strtoul( obj->str, (pchar**)NULL, 0 ) );
#endif
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_float()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as a float-value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
	
	Returns:		pfloat						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat pstring_get_float( Pstring obj )
{
	PROC( "pstring_get_float" );
	PARMS( "obj", "%p", obj );
	
	pstring_drop_tmp( obj, TRUE );
	
	if( pstring_empty( obj ) )
		RETURN( 0 );

#ifdef UNICODE
	RETURN( (pfloat)wcstod( obj->str, (pchar**)NULL ) );
#else
	RETURN( (pfloat)strtod( obj->str, (pchar**)NULL ) );
#endif
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_get_double()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the content of a Pstring-object as a double-value.
					
	Parameters:		Pstring		obj				The Pstring-object which content
												will be set.
	
	Returns:		pdouble						Returns the converted string
												content.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble pstring_get_double( Pstring obj )
{
	PROC( "pstring_get_double" );
	PARMS( "obj", "%p", obj );
	
	pstring_drop_tmp( obj, TRUE );
	
	if( pstring_empty( obj ) )
		RETURN( 0 );

#ifdef UNICODE
	RETURN( wcstod( obj->str, (pchar**)NULL ) );
#else
	RETURN( strtod( obj->str, (pchar**)NULL ) );
#endif
}

