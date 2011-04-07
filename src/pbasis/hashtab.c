/* -MODULE----------------------------------------------------------------------
Hash Table Library
Copyright (C) 2007 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>.com

File:	hashtab.c (created on 01.07.2007)
Author:	Jan Max Meyer
Usage:	Hash Table Library / Structures and Definitions
----------------------------------------------------------------------------- */

/* ---------------- */
/* --- Includes --- */
/* ---------------- */
#include "pbasis.h"

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
int hashtab_bucket_idx( HASHTAB* hashtab, uchar* key )
{
	int i;
	long hashval	= 0L;
	
	for( i = 0; i < (signed int)strlen( key ); i++ )
		hashval += (long)key[i];

	return (int)( hashval % hashtab->size );
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_create()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets up a new hash table.
					
	Parameters:		int			size				Number of buckets
					int			external_keys		Flag, if keys are handled
													internal or external
	
	Returns:		HASHTAB*						Pointer to newly allocated
													hash table, NULL in error
													case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
HASHTAB* hashtab_create( int size, int external_keys )
{
	HASHTAB* ret;
	
	ret = (HASHTAB*)malloc( sizeof( HASHTAB ) );
	
	if( ret )
	{
		memset( ret, 0, sizeof( HASHTAB ) );
	
		ret->size = size;
		ret->external_keys = external_keys;
		ret->buckets = (BUCKET*)malloc( size * sizeof( BUCKET ) );
		if( ret->buckets )
			memset( ret->buckets, 0, size * sizeof( BUCKET ) );
		else
		{
			free( ret );
			ret = (HASHTAB*)NULL;
		}
	}

	return ret;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees a hash table.
					
	Parameters:		HASHTAB*	hashtab				Hash table to be freed
					void		(*freefct)(void*)	Optional pointer to data
													element cleanup function.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void hashtab_free( HASHTAB* hashtab, void (*freefct)(void*) )
{	
	HASHELEM*	elem,
			*	next;
	int 		i;
	
	if( hashtab )
	{
		for( i = 0; i < hashtab->size; i++ )
		{
			for( elem = hashtab->buckets[i].start; elem; elem = next )
			{
				if( freefct && elem->data )
					(*freefct)( elem->data );

				if( elem->key && !( hashtab->external_keys ) )
					free( elem->key );

				next = elem->next;
				free( elem );
			}
		}
		
		for( elem = hashtab->discarded; elem; elem = next )
		{
			next = elem->next;
			free( elem );
		}
		
		free( hashtab->buckets );
		free( hashtab );
	}
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
	int			bucket;
	
	if( hashtab && key )
	{
		if( hashtab->discarded )
		{
			elem = hashtab->discarded;
			hashtab->discarded = elem->next;
		}
		else
			elem = (HASHELEM*)malloc( sizeof( HASHELEM ) );
		
		if( elem )
		{
			memset( elem, 0, sizeof( HASHELEM ) );
			
			if( !( hashtab->external_keys ) )
				elem->key = strdup( key );
			else
				elem->key = key;

			elem->data = data;
			
			bucket = hashtab_bucket_idx( hashtab, key );

			/*
				Insert new elements at the beginning of the bucket
			*/
			if( hashtab->buckets[bucket].start )
				hashtab->buckets[bucket].start->prev = elem;

			elem->next = hashtab->buckets[bucket].start;
			hashtab->buckets[bucket].start = elem;
		}
	}
	
	return elem;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_get()
	
	Author:			Jan Max Meyer
	
	Usage:			Gets an element from the hash table.
					
	Parameters:		HASHTAB*	hashtab				Hash-table to update
													element at
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
			if( strcmp( elem->key, key ) == 0 )
				break;
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
	HASHELEM*	elem		= (HASHELEM*)NULL;
	int			bucket;
	
	if( hashtab && key )
	{
		bucket = hashtab_bucket_idx( hashtab, key );
		
		for( elem = hashtab->buckets[ bucket ].start; elem; elem = elem->next )
		{
			if( strcmp( elem->key, key ) == 0 )
			{			
				elem->data = data;
				break;
			}
		}
	}
	
	return elem;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_discard()
	
	Author:			Jan Max Meyer
	
	Usage:			Discards an element within the hash table.
					Discarding an element means that the element is hold in
					memory, to be re-used at a (possibly later) hastab_insert()
					call.
					
	Parameters:		HASHTAB*	hashtab				Hash-table to discard element
													from
					uchar*		key					Element key
					void		(*freefct)(void*)	Optional pointer to data
													element cleanup function.
	
	Returns:		HASHELEM*						Discarded hash table element,
													NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
HASHELEM* hashtab_discard( HASHTAB* hashtab, uchar* key, void (*freefct)(void*) )
{
	HASHELEM*	elem		= (HASHELEM*)NULL;
	HASHELEM*	delem		= (HASHELEM*)NULL;
	int			bucket;
	
	if( hashtab && key )
	{
		bucket = hashtab_bucket_idx( hashtab, key );
		
		for( elem = hashtab->buckets[ bucket ].start; elem; elem = elem->next )
		{
			if( strcmp( elem->key, key ) == 0 )
			{			
				if( !( hashtab->external_keys ) )
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
					for( delem = hashtab->discarded; delem->next; delem = delem->next )
						;
					
					delem->next = elem;
				}
				else
					hashtab->discarded = elem;

				break;
			}
		}
	}
	
	return elem;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		hashtab_delete()
	
	Author:			Jan Max Meyer
	
	Usage:			Deletes an element within the hash table.
					When using this function, the HASHELEM-node is completely
					freed in memory; This function is not recommended for
					a frequently use, instead, use hashtab_discard().
					
	Parameters:		HASHTAB*	hashtab				Hash-table to discard element
													from
					uchar*		key					Element key
					void		(*freefct)(void*)	Optional pointer to data
													element cleanup function.
	
	Returns:		int								-1 on error,
													>= 0 on success
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int hashtab_delete( HASHTAB* hashtab, uchar* key, void (*freefct)(void*) )
{
	HASHELEM*	elem		= (HASHELEM*)NULL;
	int			bucket		= -1;
	
	if( hashtab && key )
	{
		bucket = hashtab_bucket_idx( hashtab, key );
		
		for( elem = hashtab->buckets[ bucket ].start; elem; elem = elem->next )
		{
			if( strcmp( elem->key, key ) == 0 )
			{
				if( !( hashtab->external_keys ) )
					free( elem->key );
				
				if( freefct && elem->data )
					(*freefct)( elem->data );
					
				if( elem == hashtab->buckets[ bucket ].start )
					hashtab->buckets[ bucket ].start = elem->next;
				else
					 elem->prev = elem->next;
					 
				memset( elem, 0, sizeof( HASHELEM ) );
				
				free( elem );
				break;
			}
		}
	}
	
	return bucket;
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
	
	if( !channel )
		channel = stderr;
	
	if( hashtab )
	{
		fprintf( channel, "Hash table %p, size %d buckets\n", hashtab, hashtab->size );

		for( i = 0; i < hashtab->size; i++ )
		{
			fprintf( channel, "\tBucket %d\n", i );
			
			if( hashtab->buckets[i].start )
			{
				for( elem = hashtab->buckets[i].start; elem; elem = elem->next )
					fprintf( channel, "\t\t%p \"%s\": %p\n", elem, elem->key, elem->data );
			}
			else
				fprintf( channel, "\t\t--- EMPTY ---\n" );
		}
		
		fprintf( channel, "\n" );
		for( i = 0, elem = hashtab->discarded; elem; elem = elem->next, i++ )
			;
		
		fprintf( channel, "\t%d discarded elements\n\n", i );
	}
}

