/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	hashtab.c
Author:	Jan Max Meyer
Usage:	Hash Table Library
----------------------------------------------------------------------------- */

#include <phorward.h>

/* Hash function to retrieve the bucket index. */
static int hashtab_bucket_idx( HASHTAB* hashtab, char* key )
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

/** Initializes a new hash table.

//ht// is the hash table structure to be initialized. //size// is the number
of buckets that shall be configured. //flags// are the flags to influence/modify
the hash table behavior. Use the HASHTAB_*-flags, and specify multiple of them
with the |-operator.

Returns a **pint** ERR_OK on success, else an adequate error code.
*/
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

	if( !( ht->buckets = (HASHBUCKET*)pmalloc( size * sizeof( HASHBUCKET ) ) ) )
		RETURN( ERR_MEM );

	memset( ht->buckets, 0, size * sizeof( HASHBUCKET ) );

	RETURN( ERR_OK );
}

/** Frees all hash table elements and buckets.

//hashtab// is the hash table structure to be freed.
//(*freefct)(void*)// is the optional pointer to data element cleanup function.

Always returns ERR_OK.
*/
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

/** Inserts an element into the hash table.

//hashtab// is the hash-table to insert an element to. //key// is the element
key valze. //data// is the optional data node.

Returns a **HASHELEM** pointer to the inserted hash table element,
or NULL in error case.
*/
HASHELEM* hashtab_insert( HASHTAB* hashtab, char* key, void* data )
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
					elem->key = (char*)Pstrdup( (pchar*)key );
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

/** Gets an element from the hash table.

//hashtab// is the hash-table to get element from. //key// is the element key.

Returns a **HASHELEM** pointer to the desired hash table element, or
NULL in case that no element with //key// exists.
*/
HASHELEM* hashtab_get( HASHTAB* hashtab, char* key )
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
				|| !strcmp( elem->key, key ) )
			{
				break;
			}
		}
	}
	
	return elem;
}

/** Updates an element within a hash table with new data.

//hashtab// is the hash-table to update element in. //key// is the element key
to be updated. //data// is the data node that should become the new data element
of the entry //key//.

Returns the **HASHELEM** pointer of the updated hash table element, or
NULL in case the element can't be found to be updated.
*/
HASHELEM* hashtab_update( HASHTAB* hashtab, char* key, void* data )
{
	HASHELEM*	elem;
	
	if( ( elem = hashtab_get( hashtab, key ) ) )
		elem->data = data;
	
	return elem;
}

/** Discards an element within the hash table. Discarding an element means that
the element is hold in memory, to be re-used at a (possibly later)
hastab_insert() call.

//hashtab// is the hash-table to discard an element from. //key// is the element
key that should be discarded. //(*freefct)(void*)// is the otional pointer to
data element cleanup function.

Returns a //HASHELEM// pointer to the discarded hash table element, or
(HASHELEM*)NULL if the element //key// does not exist in the hash table.
*/
HASHELEM* hashtab_discard( HASHTAB* hashtab, char* key,
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
				|| !strcmp( elem->key, key ) )
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

/** Deletes an element within the hash table. When using this function, the
HASHELEM-node is completely freed in memory; This function is not recommended
for a frequently use, instead, use hashtab_discard().

//hashtab// is the hash-table to delete element from. //key// is the element
key to be deleted. //(*freefct)(void*)// is the optional pointer to data element
cleanup function.

Returns a **pint** ERR_OK on success, else an adequate error define. 1 is
returned if the item was not found under //key//.
*/
pint hashtab_delete( HASHTAB* hashtab, char* key, void (*freefct)(void*) )
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
			|| !strcmp( elem->key, key ) )
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

/** Prints the hash table, for debug purposes.

//hashtab// is the hash table to be printed
//channel// is the channel, where to output to. If NULL, stderr is used.
*/
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

/** Count all items that are currently hold within the hash table.

//hashtab// is the hash table to retrieve an element count from.

Returns a **pint** containing the number of items hold.
*/
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

/** Fetches the successor of one hashtable element. This function is designed
to be called from within a for-loop, to loop trough all items of a hash-table.
The initial call is done with (HASHELEM*)NULL. The function must then be called
with the HASHELEM returned by each previous call until the function returns
(HASHELEM*)NULL again.

//hashtab// is the hash table to fetch next element from. //current// is the
current HASHELEM-parameter to be passed as described above.

Returns a **HASHELEM** pointer to the next element, or (HASHELEM*)NULL if no
more elements in the hash table can be fetched.
*/
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

/** Function to access an hash table element's data part.

//hashelem// is the hash table element.

Returns a **void**-pointer to the hash table elements data part.
*/
void* hashelem_access( HASHELEM* hashelem )
{
	if( !( hashelem ) )
		return (void*)NULL;

	return hashelem->data;
}

/** Function to access an hash table element key string.

//hashelem// is the hash table element.

Returns a **char** pointer to the hash table element's key string.
*/
char* hashelem_key( HASHELEM* hashelem )
{
	if( !( hashelem ) )
		return (char*)NULL;

	return hashelem->key;
}

/** Access the next hash-table element from a current one.

//hashelem// is the hash table element.

Returns a **HASHELEM** pointer to the next hash table element within the bucket.
*/
HASHELEM* hashelem_next( HASHELEM* hashelem )
{
	if( !( hashelem ) )
		return (HASHELEM*)NULL;

	return hashelem->next;
}

/** Access the previous hash-table element from a current one.

//hashelem// is the hash table element.

Returns a **HASHELEM** pointer to the previous hash table element within the
bucket.
*/
HASHELEM* hashelem_prev( HASHELEM* hashelem )
{
	if( !( hashelem ) )
		return (HASHELEM*)NULL;

	return hashelem->prev;
}
