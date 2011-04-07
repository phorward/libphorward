/* -MODULE----------------------------------------------------------------------
Phorward String Object Library
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:	pstring.c
Author:	Jan Max Meyer
Usage:	General string object functions & features
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
	Function:		pstring_new()
	
	Author:			Jan Max Meyer
	
	Usage:			Pstring-object constructor, which allocates and
					initializes a new Pstring-object.
					
	Parameters:		pchar*		str				A pchar-string to be used
												as Pstring-object initalizer
												value
	
	Returns:		Pstring						The newly allocated
												Pstring-object.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
Pstring pstring_new( pchar* str )
{
	Pstring		newstr;

	PROC( "pstring_new" );
	PARMS( "str", "%S", str );

	if ( !( newstr = (Pstring)pmalloc( sizeof( pstring ) ) ) )
		RETURN( (Pstring)NULL );

	pstring_init( newstr );

	if( str )
		pstring_set_pchar( newstr, str );

	RETURN( newstr );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Pstring-object destructor, which frees memory and
					deletes a Pstring-object.
					
	Parameters:		Pstring	obj					Pstring-object to be freed.
	
	Returns:		Pstring						Always (Pstring*)NULL
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
Pstring pstring_free( Pstring obj )
{
	PROC( "pstring_free" );
	PARMS( "str", "%p", obj );

	if( !obj )
		RETURN( (Pstring)NULL );

	pstring_reset( obj );
	pfree( obj );

	RETURN( (Pstring)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a Pstring-object.
					
	Parameters:		Pstring		obj				Pointer to a Pstring-object to
												be initialized.
	
	Returns:		Pstring						Same as input-pointer.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pstring_init( Pstring obj )
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
	
	Usage:			Resets a Pstring-object, frees possibly used content memory.
					
	Parameters:		Pstring		obj			Pointer to a Pstring-object to
											be reset.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pstring_reset( Pstring obj )
{
	PROC( "pstring_init" );
	PARMS( "obj", "%p", obj );

	if( !obj )
		VOIDRET;
	
	if( obj->str )
		pfree( obj->str );
		
	pstring_drop_tmp( obj, TRUE );
	pstring_init( obj );

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_len()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the length of a given string-object instance.
					
	Parameters:		Pstring		obj			The Pstring-object which length
											should be returned.
	
	Returns:		psize					Length of obj in size_t.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
psize pstring_len( Pstring obj )
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
	
	Usage:			Gets the raw pchar-content of the given Pstring-object.
					
	Parameters:		Pstring		obj			The Pstring-object whose raw string
											pointer should be returned.
	
	Returns:		pchar*					Pointer to data-pointer of the
											Pstring-object.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pstring_GET( Pstring obj )
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
	
	Usage:			Puts the raw pchar-content for a given Pstring-object.
					
	Parameters:		Pstring		obj			The Pstring-object whose raw string
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
int pstring_PUT( Pstring obj, pchar* ptr, psize len )
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
	Function:		pstring_drop_tmp()
	
	Author:			Jan Max Meyer
	
	Usage:			Drops the temporary conversion pointer of a string-object.
					It can be specified if the pointer only will be zero'd, or
					if also its associated memory should be freed.
					
	Parameters:		Pstring		obj			The Pstring-object whose conversion
											string pointer should be freed.
					pboolean	memdrop		TRUE: Drop even memory of tmp,
											FALSE: Hold memory, clear pointer.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pstring_drop_tmp( Pstring obj, pboolean memdrop )
{
	PROC( "pstring_drop_tmp" );
	PARMS( "obj", "%p", obj );
	PARMS( "memdrop", "%s", BOOLEAN_STR( memdrop ) );

	if( !obj )
	{
		MSG( "Object is zero" );
		VOIDRET;
	}
	
	if( obj->tmp )
	{
		if( memdrop )
			pfree( obj->tmp );

		obj->tmp = (pbyte*)NULL;
	}

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_empty()
	
	Author:			Jan Max Meyer
	
	Usage:			Checks if a Pstring-object is empty.
					
	Parameters:		Pstring		obj			The Pstring-object to test.
	
	Returns:		pboolean				TRUE: Object is empty
											FALSE: Object is not empty
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean pstring_empty( Pstring obj )
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
	
	Usage:			Copyies the content of two Pstring-objects.
					
	Parameters:		Pstring		dst			The Pstring-object to act as
											destination.
					Pstring		src			The Pstring-object from which to
											copy from.
	
	Returns:		int						Returns ERR_OK on success,
											any ERR-define on failure.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstring_copy( Pstring dst, Pstring src )
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
	
	Usage:			Duplicates a Pstring-object in the memory.
					
	Parameters:		Pstring		obj			The Pstring-object to be duplicated.
	
	Returns:		Pstring					A newly created object which is a
											duplicate of obj. In case of a 
											memory allocation error, the
											return result is (Pstring)NULL.
											It is also (Pstring)NULL if obj
											is a NULL-pointer.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
Pstring pstring_dup( Pstring obj )
{
	Pstring		dup;

	PROC( "pstring_dup" );
	PARMS( "obj", "%p", obj );
	
	if ( ! obj )
	{
		MSG( "Nothing to copy!" );
		RETURN( (Pstring)NULL );
	}

	if( !( dup = pstring_new( (pchar*)NULL ) ) )
	{
		MSG( "Am I out of memory?" );
		RETURN( (Pstring)NULL );
	}

	if( pstring_PUT( dup, obj->str, obj->len ) < ERR_OK )
	{
		pstring_free( dup );
		RETURN( (Pstring)NULL );
	}

	RETURN( dup );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstring_APPEND()
	
	Author:			Jan Max Meyer
	
	Usage:			Universal function which appends characters from a
					pchar-pointer and a length-information to a Pstring-object.
					
	Parameters:		Pstring		obj			The Pstring-object where content
											will be copied and appended to.
											If obj is (Pstring)NULL, the
											function will create a new
											object with the character, and
											returns the pointer to it.
					pchar*		ptr			Pointer to the pchar-content to be
											appended.
					psize		len			The length of the data in ptr.
											If len is 0 and ptr is not
											(pchar*)NULL, it is assumed that
											ptr is zero-terminated.
	
	Returns:		Pstring					Pointer to dst.
											In error case, (Pstring)NULL is
											returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
Pstring pstring_APPEND( Pstring obj, pchar* ptr, psize len )
{
	pboolean	save		= FALSE;

	PROC( "pstring_APPEND" );
	PARMS( "obj", "%p", obj );
	PARMS( "ptr", "%p", ptr );
	PARMS( "len", "%ld", len );

	if( !obj )
	{
		if( !( obj = pstring_new( (pchar*)NULL ) ) )
			RETURN( (Pstring)NULL );

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
		RETURN( (Pstring)NULL );

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
	
	Usage:			Appends the content of one Pstring-object to the content
					of another Pstring-object. Latter object can be optionally
					freed by the function.
					
	Parameters:		Pstring		dst			The Pstring-object where content
											will be copied and appended to.
											If dst is (Pstring)NULL, then
											src will be returned if it should
											be freed, else a duplicate of src
											is returned.

					Pstring		src			The Pstring-object from which to
											copy from.
					pboolean	drop_src	TRUE: Destruct object src after
													append operation.
											FALSE: Leave src as it is.
	
	Returns:		Pstring					Pointer to dst. If dst is NULL,
											then src will be duplicated. In
											error case, (Pstring)NULL is
											returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
Pstring pstring_append( Pstring dst, Pstring src, pboolean drop_src )
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

