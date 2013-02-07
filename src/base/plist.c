/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	plist.c
Usage:	An improved, double linked, optionally hashed list collection object.
----------------------------------------------------------------------------- */

#include <phorward.h>

/* Local prototypes */
static pboolean plist_hash_rebuild( plist* list );

/* Compare hash-table elements */
static int plist_hash_compare( plist* list, uchar* l, uchar* r )
{
	int		res;

	if( !( list && l && r ) )
	{
		WRONGPARAM;
		return -1;
	}

	if( list->flags & PLIST_MOD_WCHAR )
		res = Pstrcmp( (pchar*)l, (pchar*)r );
	else
		res = pstrcmp( l, r );

	return res;
}

/* Get hash table index */
static int plist_hash_index( plist* list, uchar* key )
{
	psize hashval	= 0L;
	psize len;

	if( !( list ) )
	{
		WRONGPARAM;
		return 0;
	}

	if( list->flags & PLIST_MOD_WCHAR )
		for( len = (pchar)Pstrlen( (pchar*)key ); len > 0; len-- )
			hashval += (int)( (pchar*)key )[ len - 1 ];
	else
		for( len = (pchar)pstrlen( key ); len > 0; len-- )
			hashval += (int)key[ len - 1 ];

	return (int)( hashval % list->hashsize );
}

/* Insert a plist entry node into the hash-table via its key. */
static pboolean plist_hash_insert( plist* list, plistelem* e )
{
	int			idx;
	plistelem*	he;
	plistelem** bucket;

	if( !( list && e ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !e->key )
		return FALSE;

	if( !list->hash && !plist_hash_rebuild( list ) )
		return FALSE;
	
	e->hashnext = (plistelem*)NULL;
	e->hashprev = (plistelem*)NULL;

	bucket = &( list->hash[ plist_hash_index( list, e->key ) ] );

	if( ! *bucket )
		*bucket = e;
	else
	{
		for( he = *bucket; he; he = he->hashnext )
		{
			if( plist_hash_compare( list, he->key, e->key ) == 0 )
			{
				if( list->flags & PLIST_MOD_UNIQUE )
					return FALSE;

				if( he == *bucket )
					*bucket = e;

				/* he will become hidden with e */
				e->hashnext = he;
				e->hashprev = he->prev;
				he->hashprev = e;
				break;
			}
			else if( !he->hashnext )
			{
				he->hashnext = e;
				e->hashprev = he;
				break;
			}
		}
	}

	return TRUE;
}

/* Rebuild hash-table */
static pboolean plist_hash_rebuild( plist* list )
{
	plistelem*	e;

	if( !list )
	{
		WRONGPARAM;
		return FALSE;
	}
	
	if( list->hash )
		list->hash = pfree( list->hash );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		e->hashnext = (plistelem*)NULL;

	list->hash = (plistelem**)pmalloc( list->hashsize * sizeof( plistelem* ) );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		plist_hash_insert( list, e );

	return TRUE;
}

/* Drop list element */
static pboolean plistelem_drop( plistelem* e )
{
	PROC( "plistelem_drop" );
	
	if( !( e ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}
	
	/* TODO: Call element destructor? */
	if( !( e->list->flags & PLIST_MOD_EXTKEYS ) )
		e->key = pfree( e->key );
		
	RETURN( TRUE );
}

/** Initialize the list //list// with an element allocation size //size//.
//flags// defines an optional flag configuration that modifies the behavior
of the linked list and hash table usage. */
pboolean plist_init( plist* list, psize size, pbyte flags )
{
	if( !( list && size >= 0 ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	memset( list, 0, sizeof( plist ) );
	list->flags = flags;
	list->size = size;
	list->hashsize = 64;

	return TRUE;
}

/** Erase all allocated content of the list //list//. */
pboolean plist_erase( plist* list )
{
	plistelem*	e;
	plistelem*	next;

	PROC( "plist_erase" );
	
	if( !( list ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}
	
	MSG( "Freeing current list contents" );
	for( e = list->first; e; e = next )
	{
		next = e->next;

		plistelem_drop( e );
		pfree( e );		
	}
	
	MSG( "Freeing list of unused nodes" );
	for( e = list->unused; e; e = next )
	{
		next = e->next;
		pfree( e );
	}
	
	MSG( "Resetting list-object pointers" );
	list->first = (plistelem*)NULL;
	list->last = (plistelem*)NULL;
	list->hash = (plistelem**)NULL;
	list->unused = (plistelem*)NULL;
	list->count = 0;
	
	RETURN( TRUE );
}

/** Insert //data// as element to the list //list// at positon //pos//.

If //pos// is NULL, the new element will be attached to the end of the
list. If //key// is not NULL, the element will be additionally engaged
into the lists hash table. */
plistelem* plist_insert( plist* list, plistelem* pos, uchar* key, pbyte* src )
{
	plistelem*	e;
	int			size;
	pbyte*		dst;

	PROC( "plist_insert" );
	PARMS( "list", "%p", list );
	PARMS( "pos", "%p", pos );
	PARMS( "key", "%s", key );
	PARMS( "src", "%p", src );

	if( !( list ) )
	{
		WRONGPARAM;
		RETURN( (plistelem*)NULL );
	}

	/* Recycle existing elements? */
	if( list->unused )
	{
		MSG( "Recycle list contains element, recycling" );
		e = list->unused;
		list->unused = e->next;
		memset( e, 0, sizeof( plistelem ) + list->size );
	}
	else
	{
		MSG( "Allocating new element" );
		VARS( "size", "%d", sizeof( plistelem ) + list->size );

		e = (plistelem*)pmalloc( sizeof( plistelem ) + list->size );
	}

	e->list = list;

	if( src )
	{
		MSG( "data is provided, will copy memory" );
		VARS( "sizeof( plistelem )", "%d", sizeof( plistelem ) );
		VARS( "size", "%d", list->size );

		memcpy( e + 1, src, list->size );
	}

	if( !pos )
	{
		MSG( "pos unset, will get last element in list" );
		pos = plist_last( list );
	}

	VARS( "pos", "%p", pos );

	if( ( e->prev = pos ) )
	{
		if( ( e->next = pos->next ) )
			e->next->prev = e;

		pos->next = e;
	}
	else
		list->first = e;
		
	if( !e->next )
		list->last = e;
		
	if( key )
	{
		MSG( "Key provided, will insert into hash table" );
		VARS( "key", "%s", key );

		if( list->flags & PLIST_MOD_EXTKEYS )
			e->key = key;
		else if( list->flags & PLIST_MOD_WCHAR )
			e->key = (uchar*)Pstrdup( (pchar*)key );
		else
			e->key = pstrdup( key );

		plist_hash_insert( list, e );
	}

	list->count++;

	VARS( "list->count", "%d", list->count );
	RETURN( e );
}

/** Removes the element //e// from the the //list// and free it or puts
 it into the unused element chain if PLIST_MOD_RECYCLE is flagged. */
plistelem* plist_remove( plist* list, plistelem* e )
{
	PROC( "plist_remove" );

	if( !( list && e && e->list == list ) )
	{
		WRONGPARAM;
		RETURN( (plistelem*)NULL );
	}

	if( e->prev )
		e->prev->next = e->next;
	else
		list->first = e->next;

	if( e->next )
		e->next->prev = e->prev;
	else
		list->last = e->prev;

	if( e->hashprev )
		e->hashprev->hashnext = e->hashnext;
	else
		list->hash[ plist_hash_index( list, e->key ) ] = e->hashnext;
		
	/* Drop element contents */
	plistelem_drop( e );

	/* Put unused node into unused list or free? */
	if( list->flags & PLIST_MOD_RECYCLE )
	{
		MSG( "Will recycle current element" );
		memset( e, 0, sizeof( plistelem ) + list->size );

		e->next = list->unused;
		list->unused = e;
		
		MSG( "Element is now discarded, for later usage" );
	}
	else
	{
		MSG( "Freeing current element" );
		pfree( e );
		MSG( "Element has gone" );
	}

	list->count--;
	RETURN( (plistelem*)NULL );
}

/** Retrieve list element by its index from the begin.

The function returns the //n//th element of the list //list//. */
plistelem* plist_get( plist* list, int n )
{
	plistelem*	e;

	if( !( list && n >= 0 ) )
	{
		WRONGPARAM;
		return (plistelem*)NULL;
	}

	for( e = plist_first( list ); e && n > 0;
			e = plist_next( e ), n-- )
		;

	return e;
}

/** Retrieve list element by hash-table key.

This function tries to fetch a list entry plistelem from list //list//
with the key //key//.
*/
plistelem* plist_get_by_key( plist* list, uchar* key )
{
	int			idx;
	plistelem*	e;

	if( !( list && key ) )
	{
		WRONGPARAM;
		return (plistelem*)NULL;
	}

	idx = plist_hash_index( list, key );

	for( e = list->hash[ idx ]; e; e = e->hashnext )
		if( plist_hash_compare( list, e->key, key ) == 0 )
			return e;

	return e;
}

/** Access data-content of the current element //e//. */
pbyte* plist_access( plistelem* e )
{
	if( !( e ) )
		return (pbyte*)NULL;
	
	return (pbyte*)( e + 1 );
}

/** Access next element of current element //e//. */
plistelem* plist_next( plistelem* e )
{
	if( !( e ) )
		return (plistelem*)NULL;
		
	return e->next;
}

/** Access previous element of a current element //e//. */
plistelem* plist_prev( plistelem* e )
{
	if( !( e ) )
		return (plistelem*)NULL;
		
	return e->prev;
}

/** Return first element of list //l//. */
plistelem* plist_first( plist* l )
{
	if( !( l ) )
		return (plistelem*)NULL;
		
	return l->first;
}

/** Return last element of list //l//. */
plistelem* plist_last( plist* l )
{
	if( !( l ) )
		return (plistelem*)NULL;
		
	return l->last;
}

/** Return element size of list //l//. */
int plist_size( plist* l )
{
	if( !( l ) )
		return 0;
		
	return l->size;
}

/** Return element count of list //l//. */
int plist_count( plist* l )
{
	if( !( l ) )
		return 0;
		
	return l->count;
}
