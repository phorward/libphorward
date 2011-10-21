/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: String Object Library
Copyright (C) 2010, 2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	pstring.c
Author:	Jan Max Meyer
Usage:	General string object functions & features
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>
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
	Function:		pstring_new()
	
	Author:			Jan Max Meyer
	
	Usage:			pstring-object constructor, which allocates and
					initializes a new pstring-object.
					
	Parameters:		pchar*		str				A pchar-string to be used
												as pstring-object initalizer
												value
	
	Returns:		pstring						The newly allocated
												pstring-object.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pstring pstring_new( pchar* str )
{
	pstring		newstr;

	PROC( "pstring_new" );
	PARMS( "str", "%S", str );

	if ( !( newstr = (pstring)pmalloc( sizeof( pstring ) ) ) )
		RETURN( (pstring)NULL );

	pstring_init( newstr );

	if( str )
		pstring_set( newstr, str );

	RETURN( newstr );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_free()
	
	Author:			Jan Max Meyer
	
	Usage:			pstring-object destructor, which frees memory and
					deletes a pstring-object.
					
	Parameters:		pstring	obj					pstring-object to be freed.
	
	Returns:		pstring						Always (pstring*)NULL
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pstring pstring_free( pstring obj )
{
	PROC( "pstring_free" );
	PARMS( "str", "%p", obj );

	if( !obj )
		RETURN( (pstring)NULL );

	pstring_reset( obj );
	pfree( obj );

	RETURN( (pstring)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a pstring-object.
					
	Parameters:		pstring		obj				Pointer to a pstring-object to
												be initialized.
	
	Returns:		pstring						Same as input-pointer.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pstring_init( pstring obj )
{
	PROC( "pstring_init" );
	PARMS( "obj", "%p", obj );

	if( obj )
		memset( obj, 0, sizeof( pstring ) );
	
	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_reset()
	
	Author:			Jan Max Meyer
	
	Usage:			Resets a pstring-object, frees possibly used content memory.
					
	Parameters:		pstring		obj			Pointer to a pstring-object to
											be reset.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pstring_reset( pstring obj )
{
	PROC( "pstring_reset" );
	PARMS( "obj", "%p", obj );

	if( !obj )
		VOIDRET;
	
	if( obj->str )
		pfree( obj->str );
		
	pstring_init( obj );

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_len()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the length of a given string-object instance.
					
	Parameters:		pstring		obj			The pstring-object which length
											should be returned.
	
	Returns:		psize					Length of obj in size_t.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
psize pstring_len( pstring obj )
{
	PROC( "pstring_len" );
	PARMS( "obj", "%p", obj );

	if( !obj )
	{
		MSG( "Object is zero, returning 0!" );
		RETURN( (size_t)0 );
	}
	
	VARS( "obj->len", "%ld", obj->len );
	RETURN( obj->len );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_GET()
	
	Author:			Jan Max Meyer
	
	Usage:			Gets the raw pchar-content of the given pstring-object.
					
	Parameters:		pstring		obj			The pstring-object whose raw string
											pointer should be returned.
	
	Returns:		pchar*					Pointer to data-pointer of the
											pstring-object.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pstring_GET( pstring obj )
{
	PROC( "pstring_GET" );
	PARMS( "obj", "%p", obj );

	if( !obj )
	{
		MSG( "Object is zero, returning (pchar*)NULL!" );
		RETURN( (pchar*)NULL );
	}
	
	VARS( "obj->str", "%p", obj->str );
	RETURN( obj->str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_PUT()
	
	Author:			Jan Max Meyer
	
	Usage:			Puts the raw pchar-content for a given pstring-object.
					
	Parameters:		pstring		obj			The pstring-object whose raw string
											pointer should be returned.
					pchar*		ptr			Pointer to read the content from
											which is put into the object.
					psize		len			The length of the data in ptr.
											If len is 0 and ptr is not
											(pchar*)NULL, it is assumed that
											ptr is zero-terminated.
	
	Returns:		int						Returns ERR_OK on success,
											any ERR-define on failure.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_PUT( pstring obj, pchar* ptr, psize len )
{
	PROC( "pstring_PUT" );
	PARMS( "obj", "%p", obj );
	PARMS( "ptr", "%p", ptr );
	PARMS( "len", "%ld", len );

	if( !obj )
	{
		MSG( "Object is zero, returning (pchar*)NULL!" );
		RETURN( ERR_PARMS );
	}

	pstring_reset( obj );

	if( ptr )
	{
		if( !len )
			len = Pstrlen( ptr );

		obj->len = len;
		if( !ALLOC_STR( obj, obj->len ) )
			RETURN( ERR_MEM );

		memcpy( obj->str, ptr, len * sizeof( pchar ) );
		obj->str[ len ] = L'\0';
			
		VARS( "obj->str", "%p", obj->str );
	}

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_empty()
	
	Author:			Jan Max Meyer
	
	Usage:			Checks if a pstring-object is empty.
					
	Parameters:		pstring		obj			The pstring-object to test.
	
	Returns:		pboolean				TRUE: Object is empty
											FALSE: Object is not empty
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean pstring_empty( pstring obj )
{
	PROC( "pstring_empty" );
	PARMS( "obj", "%p", obj );
	
	if( !( obj && ( obj->str && *( obj->str ) && obj->len ) ) )
		RETURN( TRUE );
		
	RETURN( FALSE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_copy()
	
	Author:			Jan Max Meyer
	
	Usage:			Copyies the content of two pstring-objects.
					
	Parameters:		pstring		dst			The pstring-object to act as
											destination.
					pstring		src			The pstring-object from which to
											copy from.
	
	Returns:		int						Returns ERR_OK on success,
											any ERR-define on failure.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_copy( pstring dst, pstring src )
{
	PROC( "pstring_copy" );
	PARMS( "dst", "%p", dst );
	PARMS( "src", "%p", src );
	
	if ( !( dst && src ) )
	{
		MSG( "Parameters are invalid" );
		RETURN( ERR_PARMS );
	}

	RETURN( pstring_PUT( dst, src->str, src->len ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_dup()
	
	Author:			Jan Max Meyer
	
	Usage:			Duplicates a pstring-object in the memory.
					
	Parameters:		pstring		obj			The pstring-object to be duplicated.
	
	Returns:		pstring					A newly created object which is a
											duplicate of obj. In case of a 
											memory allocation error, the
											return result is (pstring)NULL.
											It is also (pstring)NULL if obj
											is a NULL-pointer.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pstring pstring_dup( pstring obj )
{
	pstring		dup;

	PROC( "pstring_dup" );
	PARMS( "obj", "%p", obj );
	
	if ( ! obj )
	{
		MSG( "Nothing to copy!" );
		RETURN( (pstring)NULL );
	}

	if( !( dup = pstring_new( (pchar*)NULL ) ) )
	{
		MSG( "Am I out of memory?" );
		RETURN( (pstring)NULL );
	}

	if( pstring_PUT( dup, obj->str, obj->len ) < ERR_OK )
	{
		pstring_free( dup );
		RETURN( (pstring)NULL );
	}

	RETURN( dup );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_APPEND()
	
	Author:			Jan Max Meyer
	
	Usage:			Universal function which appends characters from a
					pchar-pointer and a length-information to a pstring-object.
					
	Parameters:		pstring		obj			The pstring-object where content
											will be copied and appended to.
											If obj is (pstring)NULL, the
											function will create a new
											object with the character, and
											returns the pointer to it.
					pchar*		ptr			Pointer to the pchar-content to be
											appended.
					psize		len			The length of the data in ptr.
											If len is 0 and ptr is not
											(pchar*)NULL, it is assumed that
											ptr is zero-terminated.
	
	Returns:		pstring					Pointer to dst.
											In error case, (pstring)NULL is
											returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pstring pstring_APPEND( pstring obj, pchar* ptr, psize len )
{
	pboolean	save		= FALSE;

	PROC( "pstring_APPEND" );
	PARMS( "obj", "%p", obj );
	PARMS( "ptr", "%p", ptr );
	PARMS( "len", "%ld", len );

	if( !obj )
	{
		if( !( obj = pstring_new( (pchar*)NULL ) ) )
			RETURN( (pstring)NULL );

		RETURN( pstring_APPEND( obj, ptr, len ) );
	}

	if( !ptr )
		RETURN( obj );

	if( !len )
		len = Pstrlen( ptr );

	VARS( "new length will be", "%ld", obj->len + len );
	VARS( "ALLOC", "%ld", GET_ALLOC_SIZE( obj->len + len ) );
	/* Special-case: Appending this pointer to itself - it may be lost! */
	if( obj->str == ptr )
		save = TRUE;

	if( !obj->str )
		obj->str = (pchar*)pmalloc(
						GET_ALLOC_SIZE( obj->len + len )
							* sizeof( pchar ) );
	else
		obj->str = (pchar*)prealloc( obj->str,
						GET_ALLOC_SIZE( obj->len + len )
							* sizeof( pchar ) );

	if( !obj->str )
		RETURN( (pstring)NULL );

	if( save )
		ptr = obj->str;

	memcpy( obj->str + obj->len, ptr, len * sizeof( pchar ) );

	obj->len += len;
	VARS( "obj->len", "%ld", obj->len );

	obj->str[ obj->len ] = L'\0';

	RETURN( obj );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_append()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends the content of one pstring-object to the content
					of another pstring-object. Latter object can be optionally
					freed by the function.
					
	Parameters:		pstring		dst			The pstring-object where content
											will be copied and appended to.
											If dst is (pstring)NULL, then
											src will be returned if it should
											be freed, else a duplicate of src
											is returned.

					pstring		src			The pstring-object from which to
											copy from.
					BOOLEAN		drop_src	TRUE: Destruct object src after
													append operation.
											FALSE: Leave src as it is.
	
	Returns:		pstring					Pointer to dst. If dst is NULL,
											then src will be duplicated. In
											error case, (pstring)NULL is
											returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pstring pstring_append( pstring dst, pstring src, pboolean drop_src )
{
	PROC( "pstring_append" );
	PARMS( "dst", "%p", dst );
	PARMS( "src", "%p", src );
	PARMS( "drop_src", "%s", BOOLEAN_STR( drop_src ) );

	if( !dst && src )
	{
		if( drop_src )
			RETURN( src );

		RETURN( pstring_dup( src ) );
	}

	if( !pstring_empty( src ) )
		dst = pstring_APPEND( dst, src->str, src->len );

	if( src && drop_src )
		pstring_free( src );

	RETURN( dst );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_charat()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns a string object's character at a speicfied offset.
					
	Parameters:		pstring		obj			The pstring-object from which to
											return the character from.
					psize		position	The offset of the character.
	
	Returns:		pchar*					Returns a pointer to the expected
											character. If the positon index is
											out of string bounds, (pchar*)NULL
											will be returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pstring_charat( pstring obj, psize position )
{
	pchar*		ret;
#if !defined( UNICODE ) && defined( UTF8 )
	psize		i;
#endif
	PROC( "pstring_charat" );
	PARMS( "obj", "%p", obj );
	PARMS( "position", "%ld", position );
	
	if( !( obj && obj->str ) )
	{
		MSG( "Invalid string object" );
		RETURN( (pchar*)NULL );
	}
	
	if( position > obj->len )
	{
		MSG( "Position is out of bounds!" );
		RETURN( (pchar*)NULL );
	}

#if defined( UNICODE ) || !defined( UTF8 )
	ret = obj->str + position;
#else
	MSG( "UTF-8 mode" );
	for( i = 0, ret = obj->str; i < position && i < obj->len; i++ )
		ret += u8_seqlen( ret );
	
	if( i == obj->len )
		ret	= (pchar*)NULL;
#endif

	RETURN( ret );
}

