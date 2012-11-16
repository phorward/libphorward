/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See \LICENSE for more information.

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

/** Insert a plist entry node into the hash-table via its key. */
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

/** Initialize the list //list// with an element allocation size //size//.
//flags// defines an optional flag configuration that modifies the behavior
of the linked list and hash table usage. */
pboolean plist_init( plist* list, psize size, pbyte flags )
{
	if( !( list && size <= 0 ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	memset( list, 0, sizeof( plist ) );
	list->flags = PLIST_MOD_NONE;
	list->size = size;
	list->hashsize = 64;

	return TRUE;
}

/** Insert //data// as element to the list //list// at positon //pos//.

If //pos// is NULL, the new element will be attached to the end of the list.
If //key// is not NULL, the element will be additionally engaged into the lists hash table. */
plistelem* plist_insert( plist* list, plistelem* pos, uchar* key, pbyte* data )
{
	plistelem*	e;

	if( !( list ) )
	{
		WRONGPARAM;
		return (plistelem*)NULL;
	}

	/* Recycle existing elements? */
	if( list->unused )
	{
		e = list->unused;
		list->unused = e->next;
		memset( e, 0, sizeof( plistelem ) + list->size );
	}
	else
		e = (plistelem*)pmalloc( sizeof( plistelem ) + list->size );

	e->list = list;
	memcpy( e + sizeof( plistelem ), data, list->size );

	if( !pos )
		pos = plist_last( list );

	e->prev = pos;
	e->next = pos->next;
	pos->next = e;
	e->next->prev = e;
	
	if( list->first == pos )
		list->first = e;
		
	if( list->last == pos )
		list->last = e;
		
	if( key )
	{
		if( list->flags & PLIST_MOD_EXTKEYS )
			e->key = key;
		else if( list->flags & PLIST_MOD_WCHAR )
			e->key = (uchar*)Pstrdup( (pchar*)key );
		else
			e->key = pstrdup( key );

		plist_hash_insert( list, e );
	}

	list->count++;
	return e;
}

/** Removes the element //e// from the the //list// and free it or puts it into the unused element chain if PLIST_MOD_RECYCLE is flagged. */
plistelem* plist_remove( plist* list, plistelem* e )
{
	if( !( list && e && e->list == list ) )
	{
		WRONGPARAM;
		return (plistelem*)NULL;
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

	/* Put unused node into unused list or free? */
	if( list->flags & PLIST_MOD_RECYCLE )
	{
		memset( e, 0, sizeof( plistelem ) + list->size );

		e->next = list->unused;
		list->unused = e;
	}
	else
		pfree( e );

	list->count--;
	return (plistelem*)NULL;
}

plistelem* plist_get( plist* list, int idx )
{
	plistelem*	e;

	if( !( list && idx >= 0 ) )
	{
		WRONGPARAM;
		return (plistelem*)NULL;
	}

	for( e = plist_first( list ); e && idx > 0; e = plist_next( e ), idx-- )
		;

	return e;
}

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

