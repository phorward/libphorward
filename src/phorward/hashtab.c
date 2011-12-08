/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	hashtab.c
Author:	Jan Max Meyer
Usage:	Hash Table Library
----------------------------------------------------------------------------- */

/* ---------------- */
/* --- Includes --- */
/* ---------------- */
#include "phorward.h"

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

/* ---------------------- */
/* --- Implementation --- */
/* ---------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_bucket_idx()
	
	Author:			Jan Max Meyer
	
	Usage:			Hash function to retrieve the bucket index.
					
	Parameters:		HASHTAB*	hashtab				Hash table pointer
					uchar*		key					Key string
	
	Returns:		int								Index of the bucket
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static int hashtab_bucket_idx( HASHTAB* hashtab, uchar* key )
{
	psize hashval	= 0L;
	psize len;
	
	if( hashtab->flags & HASHTAB_MOD_WCHAR )
	{
		len = (pchar)Pstrlen( (pchar*)key );
		for( ; len > 0; len-- )
			hashval += (int)( (pchar*)key )[len - 1];
	}
	else
	{
		len = (pchar)pstrlen( key );
		for( ; len > 0; len-- )
			hashval += (int)key[len - 1];
	}

	return (int)( hashval % hashtab->size );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a new hash table.
					
	Parameters:		HASHTAB*	ht					Hash table structure
													to be initialized.
					pint		size				Number of buckets
					pint		flags				Flags to influence/modify
													hash table behavior. Use
													HASHTAB_*-flags, and 
													specify multiple of them
													with the |-operator.
	
	Returns:		pint							Returns ERR_OK on success,
													else an adequate error
													code.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint hashtab_init( HASHTAB* ht, pint size, pint flags )
{
	PROC( "hashtab_init" );
	PARMS( "ht", "%p", ht );
	PARMS( "size", "%d", size );
	PARMS( "flags", "%d", flags );

	if( !ht )
		RETURN( ERR_PARMS );

	memset( ht, 0, sizeof( HASHTAB ) );
	ht->flags = flags;
	ht->size = size;

	if( !( ht->buckets = (BUCKET*)pmalloc( size * sizeof( BUCKET ) ) ) )
		RETURN( ERR_MEM );

	memset( ht->buckets, 0, size * sizeof( BUCKET ) );

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees all hash table's elements and buckets.
					
	Parameters:		HASHTAB*	hashtab				Hash table to be freed
					void		(*freefct)(void*)	Optional pointer to data
													element cleanup function.
	
	Returns:		int
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint hashtab_free( HASHTAB* hashtab, void (*freefct)(void*) )
{	
	HASHELEM*	elem,
			*	next;
	int 		i;
	
	PROC( "hashtab_free" );
	PARMS( "hashtab", "%p", hashtab );
	PARMS( "freefct", "%p", freefct );
	
	if( hashtab )
	{
		VARS( "hashtab->size", "%d", hashtab->size );
		for( i = 0; i < hashtab->size; i++ )
		{
			VARS( "i", "%d", i );
			for( elem = hashtab->buckets[i].start; elem; elem = next )
			{
				if( freefct && elem->data )
				{
					MSG( "Calling freefct" );
					(*freefct)( elem->data );
				}

				if( elem->key && !( hashtab->flags & HASHTAB_MOD_EXTKEYS ) )
				{
					MSG( "Freeing key" );
					pfree( elem->key );
				}

				next = elem->next;
				pfree( elem );
			}
		}
		
		for( elem = hashtab->discarded; elem; elem = next )
		{
			next = elem->next;
			pfree( elem );
		}
		
		pfree( hashtab->buckets );

		if( hashtab->flags & HASHTAB_MOD_LIST )
			hashtab->list = list_free( hashtab->list );
	}

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_insert()
	
	Author:			Jan Max Meyer
	
	Usage:			Inserts an element into the hash table.
					
	Parameters:		HASHTAB*	hashtab				Hash-table to insert
													element to
					uchar*		key					Element key
					void*		data				Optional data node
	
	Returns:		HASHELEM*						Inserted hash table element,
													NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
HASHELEM* hashtab_insert( HASHTAB* hashtab, uchar* key, void* data )
{
	HASHELEM*	elem		= (HASHELEM*)NULL;
	pint		bucket;

	PROC( "hashtab_insert" );
	PARMS( "hashtab", "%p", hashtab );
	PARMS( "key", "%s", key );
	PARMS( "data", "%p", data );
	
	if( hashtab && key )
	{
		if( hashtab->flags & HASHTAB_MOD_NO_COLL )
		{
			MSG( "Checking if there is already one element with this key" );

			if( hashtab_get( hashtab, key ) )
			{
				MSG( "There is already an element with this key - this is "
						"not wanted!" );
				RETURN( (HASHELEM*)NULL );
			}
		}
		
		if( hashtab->discarded )
		{
			elem = hashtab->discarded;
			hashtab->discarded = elem->next;
		}
		else
			elem = (HASHELEM*)pmalloc( sizeof( HASHELEM ) );
		
		if( elem )
		{
			memset( elem, 0, sizeof( HASHELEM ) );
			
			if( !( hashtab->flags & HASHTAB_MOD_EXTKEYS ) )
			{
				if( hashtab->flags & HASHTAB_MOD_WCHAR )
					elem->key = (uchar*)Pstrdup( (pchar*)key );
				else
					elem->key = strdup( key );
			}
			else
				elem->key = key;

			elem->data = data;
			
			bucket = hashtab_bucket_idx( hashtab, key );
			VARS( "bucket", "%d", bucket );

			/* Insert new elements at the beginning of the bucket */
			if( hashtab->buckets[bucket].start )
				hashtab->buckets[bucket].start->prev = elem;

			elem->next = hashtab->buckets[bucket].start;
			hashtab->buckets[bucket].start = elem;

			/* Insert into array, if configured so */
			if( hashtab->flags & HASHTAB_MOD_LIST )
				hashtab->list = list_push( hashtab->list, elem );
		}
	}
	
	VARS( "elem", "%p", elem );
	RETURN( elem );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_get()
	
	Author:			Jan Max Meyer
	
	Usage:			Gets an element from the hash table.
					
	Parameters:		HASHTAB*	hashtab				Hash-table to get element
													from
					uchar*		key					Element key
	
	Returns:		HASHELEM*						Desired hash table element,
													NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
HASHELEM* hashtab_get( HASHTAB* hashtab, uchar* key )
{
	HASHELEM*	elem		= (HASHELEM*)NULL;
	int			bucket;
	
	if( hashtab && key )
	{
		bucket = hashtab_bucket_idx( hashtab, key );

		for( elem = hashtab->buckets[ bucket ].start; elem; elem = elem->next )
		{
			if( ( ( hashtab->flags & HASHTAB_MOD_WCHAR ) && 
					!Pstrcmp( (pchar*)( elem->key ), (pchar*)key ) )
				|| !pstrcmp( elem->key, key ) )
			{
				break;
			}
		}
	}
	
	return elem;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_update()
	
	Author:			Jan Max Meyer
	
	Usage:			Updates an element within the hash table, with new data.
					
	Parameters:		HASHTAB*	hashtab				Hash-table to update
													element at
					uchar*		key					Element key
					void*		data				Data node
	
	Returns:		HASHELEM*						Updated hash table element,
													NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
HASHELEM* hashtab_update( HASHTAB* hashtab, uchar* key, void* data )
{
	HASHELEM*	elem;
	
	if( ( elem = hashtab_get( hashtab, key ) ) )
		elem->data = data;
	
	return elem;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_discard()
	
	Author:			Jan Max Meyer
	
	Usage:			Discards an element within the hash table.
					Discarding an element means that the element is hold in
					memory, to be re-used at a (possibly later) hastab_insert()
					call.
					
	Parameters:		HASHTAB*	hashtab				Hash-table to discard an
													element from
					uchar*		key					Element key
					void		(*freefct)(void*)	Optional pointer to data
													element cleanup function.
	
	Returns:		HASHELEM*						Discarded hash table
													element, (HASHELEM*)NULL
													in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
HASHELEM* hashtab_discard( HASHTAB* hashtab, uchar* key,
			void (*freefct)(void*) )
{
	HASHELEM*	elem		= (HASHELEM*)NULL;
	HASHELEM*	delem		= (HASHELEM*)NULL;
	pint		bucket;

	PROC( "hashtab_discard" );
	PARMS( "hashtab", "%p", hashtab );
	PARMS( "key", "%s", key );
	PARMS( "freefct", "%p", freefct );
	
	if( hashtab && key )
	{
		bucket = hashtab_bucket_idx( hashtab, key );
		
		for( elem = hashtab->buckets[ bucket ].start; elem; elem = elem->next )
		{
			if( ( ( hashtab->flags & HASHTAB_MOD_WCHAR ) && 
					!Pstrcmp( (pchar*)( elem->key ), (pchar*)key ) )
				|| !pstrcmp( elem->key, key ) )
			{			
				if( !( hashtab->flags & HASHTAB_MOD_EXTKEYS ) )
					free( elem->key );
				
				if( freefct && elem->data )
					(*freefct)( elem->data );
					
				if( elem == hashtab->buckets[ bucket ].start )
					hashtab->buckets[ bucket ].start = elem->next;
				else
					 elem->prev = elem->next;
					 
				memset( elem, 0, sizeof( HASHELEM ) );
				
				if( hashtab->discarded )
				{
					for( delem = hashtab->discarded; delem->next;
							delem = delem->next )
						;
					
					delem->next = elem;
				}
				else
					hashtab->discarded = elem;

				/* Remove from array, if configured so */
				if( hashtab->flags & HASHTAB_MOD_LIST )
					hashtab->list = list_remove( hashtab->list, elem );

				break;
			}
		}
	}
	
	RETURN( elem );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_delete()
	
	Author:			Jan Max Meyer
	
	Usage:			Deletes an element within the hash table.
					When using this function, the HASHELEM-node is completely
					freed in memory; This function is not recommended for
					a frequently use, instead, use hashtab_discard().
					
	Parameters:		HASHTAB*	hashtab				Hash-table to delete 
													element from
					uchar*		key					Element key
					void		(*freefct)(void*)	Optional pointer to data
													element cleanup function.
	
	Returns:		pint							Returns ERR_OK on success,
													else an adequate error
													define. 1 is returned if
													the item was not found.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint hashtab_delete( HASHTAB* hashtab, uchar* key, void (*freefct)(void*) )
{
	HASHELEM*	elem		= (HASHELEM*)NULL;
	pint		bucket;

	PROC( "hashtab_delete" );
	PARMS( "hashtab", "%p", hashtab );
	PARMS( "key", "%s", key );
	
	if( !( hashtab && key ) )
		RETURN( ERR_PARMS );

	bucket = hashtab_bucket_idx( hashtab, key );
	
	for( elem = hashtab->buckets[ bucket ].start; elem; elem = elem->next )
	{
		if( ( ( hashtab->flags & HASHTAB_MOD_WCHAR ) && 
				!Pstrcmp( (pchar*)( elem->key ), (pchar*)key ) )
			|| !pstrcmp( elem->key, key ) )
		{
			MSG( "Element found, will free this one!" );

			if( !( hashtab->flags & HASHTAB_MOD_EXTKEYS ) )
				pfree( elem->key );
			
			if( freefct && elem->data )
				(*freefct)( elem->data );
				
			if( elem == hashtab->buckets[ bucket ].start )
				hashtab->buckets[ bucket ].start = elem->next;
			else
				 elem->prev = elem->next;
				 
			memset( elem, 0, sizeof( HASHELEM ) );

			/* Remove from array, if configured so */
			if( hashtab->flags & HASHTAB_MOD_LIST )
				hashtab->list = list_remove( hashtab->list, elem );

			/* Entirely drop this item */
			pfree( elem );

			RETURN( ERR_OK );
		}
	}

	MSG( "Can't find element" );
	RETURN( 1 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_print()
	
	Author:			Jan Max Meyer
	
	Usage:			Prints the hash table, for debug purposes.
					
	Parameters:		HASHTAB*	hashtab				Hash table to be printed
					FILE*		channel				Channel, where to output
													to. If NULL, stderr is used.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void hashtab_print( HASHTAB* hashtab, FILE* channel )
{	
	HASHELEM*	elem;
	int 		i;
	LIST*		l;
	
	if( !channel )
		channel = stderr;
	
	if( hashtab )
	{
		fprintf( channel, "Hash table %p, size %d buckets, %d active entries\n",
					hashtab, hashtab->size, hashtab_count( hashtab ) );

		if( hashtab->flags & HASHTAB_MOD_WCHAR )
			fprintf( channel, "+ Table uses wide-character key values\n" );

		if( hashtab->flags & HASHTAB_MOD_EXTKEYS )
			fprintf( channel, "+ Key memory is stored externally\n" );

		if( hashtab->flags & HASHTAB_MOD_LIST )
		{
			fprintf( channel, "+ List-feature enabled:\n" );

			if( !hashtab_list( hashtab ) )
				fprintf( channel, "  List is empty\n" );

			for( l = hashtab_list( hashtab ), i = 0; l;
					l = list_next( l ), i++ )
			{
				elem = (HASHELEM*)list_access( l );
				fprintf( channel, "  %d: %p ", i, elem );

				if( hashtab->flags & HASHTAB_MOD_WCHAR )
					fprintf( channel, "\"%ls\"", (pchar*)( elem->key ) );
				else
					fprintf( channel, "\"%s\"", elem->key );

				fprintf( channel, ": %p\n", elem->data );
			}
		}

		fprintf( channel, "\n" );

		for( i = 0; i < hashtab->size; i++ )
		{
			fprintf( channel, " Bucket %d\n", i );
			
			if( hashtab->buckets[i].start )
			{
				for( elem = hashtab->buckets[i].start; elem; elem = elem->next )
				{
					fprintf( channel, " %p ", elem );

					if( hashtab->flags & HASHTAB_MOD_WCHAR )
						fprintf( channel, "\"%ls\"", (pchar*)( elem->key ) );
					else
						fprintf( channel, "\"%s\"", elem->key );

					fprintf( channel, ": %p\n", elem->data );
				}
			}
			else
				fprintf( channel, " --- EMPTY ---\n" );
		}
		
		fprintf( channel, "\n" );
		for( i = 0, elem = hashtab->discarded; elem; elem = elem->next, i++ )
			;
		
		fprintf( channel, "%d discarded elements\n\n", i );
	}
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_count()
	
	Author:			Jan Max Meyer
	
	Usage:			Count all items that are currently hold within the table.
					
	Parameters:		HASHTAB*	hashtab				Hash table to retrieve
													element count from.
	
	Returns:		pint							Returns the number of
													items hold.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint hashtab_count( HASHTAB* hashtab )
{
	HASHELEM*	elem;
	pint 		i;
	pint		count		= 0;

	PROC( "hashtab_count" );
	PARMS( "hashtab", "%p", hashtab );
	
	if( hashtab )
	{
		for( i = 0; i < hashtab->size; i++ )
			if( hashtab->buckets[i].start )
				for( elem = hashtab->buckets[i].start; elem; elem = elem->next )
					count++;
	}

	VARS( "count", "%ld", count );
	RETURN( count );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_fetch()
	
	Author:			Jan Max Meyer
	
	Usage:			Fetches the successor of one hashtable element. This
					function is designed to be called from within a for-loop,
					to loop trough all items of a hash-table.

					Initial call is done with (HASHELEM*)NULL. The function
					must then be called with the HASHELEM returned by each
					previous call until the function returns (HASHELEM*)NULL
					again.
					
	Parameters:		HASHTAB*	hashtab				Hash table to fetch next
													element from.
					HASHELEM*	current				HASHELEM-Parameter to be
													passed as described above.
	
	Returns:		HASHELEM*						Returns the next element,
													(HASHELEM*)NULL if no more
													hash elements can be
													fetched.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	PROC( "hashtab_fetch" );
	PARMS( "hashtab", "%p", hashtab );
----------------------------------------------------------------------------- */
HASHELEM* hashtab_fetch( HASHTAB* hashtab, HASHELEM* current )
{
	pint		i;
	HASHELEM*	elem;

	PROC( "hashtab_fetch" );
	PARMS( "hashtab", "%p", hashtab );
	PARMS( "current", "%p", current );

	if( hashtab )
	{
		for( i = 0; i < hashtab->size; i++ )
		{
			VARS( "i (bucket", "%d", i );

			if( hashtab->buckets[i].start )
			{
				if( !current )
				{
					MSG( "Returning first element of this bucket" );
					RETURN( hashtab->buckets[i].start );
				}

				for( elem = hashtab->buckets[i].start; elem; elem = elem->next )
				{
					if( elem == current )
					{
						if( !( hashelem_next( current ) ) )
						{
							MSG( "This is the last element in this bucket" );
							current = (HASHELEM*)NULL;
							break;
						}

						MSG( "Next element found" );
						RETURN( hashelem_next( current ) );
					}
				}
			}
		}
	}

	MSG( "No more items in table" );
	RETURN( (HASHELEM*)NULL );
}

