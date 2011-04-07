/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	util.c
Author:	Jan Max Meyer
Usage:	Utiliy functions for various usages.
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
	Function:		memdup()
	
	Author:			Jan Max Meyer
	
	Usage:			Duplicates a memory entry onto the heap.
					
	Parameters:		void*		ptr				Pointer to memory
					size_t		size			Size of pointer
	
	Returns:		void*						Pointer to memory copy.
												Cast this with your type!
												Returns (void*)NULL on error!
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* memdup( void* ptr, psize size )
{
	void*	ret;
	
	if( !( ptr || size ) )
		return (void*)NULL;
		
	ret = pmalloc( size );
	memcpy( ret, ptr, size );
		
	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hash_from_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Duplicates a memory entry onto the heap.
					
	Parameters:		uchar*		str				String value to be hashed
					int			size			Hash table size
	
	Returns:		int							The hashed index value
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int hash_from_str( uchar* str, int size )
{
	int		v = 0;
	uchar*	p;

	for( p = str; *str; str++ )
		v += (int)*str;

	return v % size;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		map_file()
	
	Author:			Jan Max Meyer
	
	Usage:			Maps the content of a file into memory.
					
	Parameters:		uchar**		cont			The file content, return
												pointer.
					uchar*		filename		Path to file to be mapped
	
	Returns:		int			ERR_OK			On success
								1				If the file could not be found
								ERR...			ERR-define according to its
												meaning
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int map_file( char** cont, uchar* filename )
{
	FILE*	f;
	uchar*	c;

	PROC( "map_file" );
	PARMS( "cont", "%p", cont );
	PARMS( "filename", "%s", filename );
	
	/* Check parameters */
	if( !( cont && filename && *filename ) )
	{
		MSG( "Incomplete parameters!" );
		RETURN( ERR_PARMS );
	}
	
	/* Open file */
	if( !( f = fopen( filename, "rb" ) ) )
	{
		MSG( "File could not be opened - wrong path?" );
		RETURN( 1 );
	}
	
	/* Allocate memory for file */
	fseek( f, 0L, SEEK_END );
	if( !( c = *cont = (uchar*)pmalloc( ( ftell( f ) + 1 )
			* sizeof( uchar ) ) ) )
	{
		MSG( "Unable to allocate required memory" );
		
		fclose( f );
		RETURN( ERR_MEM );
	}
	
	/* Read entire file into buffer */
	fseek( f, 0L, SEEK_SET );

	while( !feof( f ) )
		*(c++) = fgetc( f );

	/* Case: File is empty */
	if( c == *cont )
		c++;
		
	*( c - 1 ) = '\0';
	
	fclose( f );
	
	VARS( "Entire file", "%s", *cont );
	MSG( "All right!" );
	
	RETURN( ERR_OK );
}

