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
static int plist_hash_compare( plist* list, char* l, char* r )
{
	int		res;

	if( !( list && l && r ) )
	{
		WRONGPARAM;
		return -1;
	}

	if( list->flags & PLIST_MOD_WCHAR )
		res = wcscmp( (pchar*)l, (pchar*)r );
	else
		res = strcmp( l, r );

	return res;
}

/* Get hash table index */
static int plist_hash_index( plist* list, char* key )
{
	psize hashval	= 0L;
	psize len;

	if( !( list ) )
	{
		WRONGPARAM;
		return 0;
	}

	if( list->flags & PLIST_MOD_WCHAR )
		for( len = (pchar)pwcslen( (pchar*)key ); len > 0; len-- )
			hashval += (int)( (pchar*)key )[ len - 1 ];
	else
		for( len = (pchar)pstrlen( key ); len > 0; len-- )
			hashval += (int)key[ len - 1 ];

	return (int)( hashval % list->hashsize );
}

/* Insert a plist entry node into the hash-table via its key. */
static pboolean plist_hash_insert( plist* list, punit* e )
{
	int		idx;
	punit*	he;
	punit** bucket;

	if( !( list && e ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !e->key )
		return FALSE;

	if( !list->hash && !plist_hash_rebuild( list ) )
		return FALSE;

	e->hashnext = (punit*)NULL;
	e->hashprev = (punit*)NULL;

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
	punit*	e;

	if( !list )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( list->hash )
		list->hash = pfree( list->hash );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		e->hashnext = (punit*)NULL;

	list->hash = (punit**)pmalloc( list->hashsize * sizeof( punit* ) );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		plist_hash_insert( list, e );

	return TRUE;
}

/* Drop list element */
static pboolean punit_drop( punit* e )
{
	PROC( "punit_drop" );

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

/** Create a new plist as an object with an element allocation size //size//.
//flags// defines an optional flag configuration that modifies the behavior
of the linked list and hash table usage.

Use plist_free() to erase and release the returned list object. */
plist* plist_create( psize size, pbyte flags )
{
	plist*	list;

	if( !( size >= 0 ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	list = (plist*)pmalloc( sizeof( plist ) );
	plist_init( list, size, flags );

	return list;
}

/** Erase all allocated content of the list //list//. */
pboolean plist_erase( plist* list )
{
	punit*	e;
	punit*	next;

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

		punit_drop( e );
		pfree( e );
	}

	MSG( "Freeing list of unused nodes" );
	for( e = list->unused; e; e = next )
	{
		next = e->next;
		pfree( e );
	}

	MSG( "Resetting list-object pointers" );
	list->first = (punit*)NULL;
	list->last = (punit*)NULL;
	list->hash = (punit**)NULL;
	list->unused = (punit*)NULL;
	list->count = 0;

	RETURN( TRUE );
}

/** Releases all the memory //list// uses and destroys the list object.

The function always returns (plist*)NULL. */
plist* plist_free( plist* list )
{
	if( !( list ) )
		return (plist*)NULL;

	plist_erase( list );
	pfree( list );

	return (plist*)NULL;
}

/** Insert //src// as element to the list //list// at positon //pos//.

If //pos// is NULL, the new element will be attached to the end of the
list. If //key// is not NULL, the element will be additionally engaged
into the lists hash table. */
punit* plist_insert( plist* list, punit* pos, char* key, void* src )
{
	punit*	e;
	int		size;
	pbyte*	dst;

	PROC( "plist_insert" );
	PARMS( "list", "%p", list );
	PARMS( "pos", "%p", pos );
	PARMS( "key", "%s", key );
	PARMS( "src", "%p", src );

	if( !( list ) )
	{
		WRONGPARAM;
		RETURN( (punit*)NULL );
	}

	/* Recycle existing elements? */
	if( list->unused )
	{
		MSG( "Recycle list contains element, recycling" );
		e = list->unused;
		list->unused = e->next;
		memset( e, 0, sizeof( punit ) + list->size );
	}
	else
	{
		MSG( "Allocating new element" );
		VARS( "size", "%d", sizeof( punit ) + list->size );

		e = (punit*)pmalloc( sizeof( punit ) + list->size );
	}

	e->list = list;

	if( src )
	{
		MSG( "data is provided, will copy memory" );
		VARS( "sizeof( punit )", "%d", sizeof( punit ) );
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
			e->key = (char*)pwcsdup( (pchar*)key );
		else
			e->key = pstrdup( key );

		plist_hash_insert( list, e );
	}

	list->count++;

	VARS( "list->count", "%d", list->count );
	RETURN( e );
}

/** Push //src// to //list//.

Like //list// would be a stack, //src// is pushed at the end of the list.
This function can only be used for simple linked lists without the hash-table
feature in use. */
punit* plist_push( plist* list, void* src )
{
	if( !( list ) )
	{
		WRONGPARAM;
		return (punit*)NULL;
	}

	return plist_insert( list, (punit*)NULL, (char*)NULL, src );
}

/** Removes the element //e// from the the //list// and free it or puts
 it into the unused element chain if PLIST_MOD_RECYCLE is flagged.

Will always return (punit*)NULL. */
punit* plist_remove( plist* list, punit* e )
{
	PROC( "plist_remove" );

	if( !( list && e && e->list == list ) )
	{
		WRONGPARAM;
		RETURN( (punit*)NULL );
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
	punit_drop( e );

	/* Put unused node into unused list or free? */
	if( list->flags & PLIST_MOD_RECYCLE )
	{
		MSG( "Will recycle current element" );
		memset( e, 0, sizeof( punit ) + list->size );

		e->next = list->unused;
		list->unused = e;

		MSG( "Element is now discarded, for later usage" );
	}
	else
	{
		MSG( "Freeing current element" );
		pfree( e );
		MSG( "Element gone" );
	}

	list->count--;
	RETURN( (punit*)NULL );
}

/** Pop last element to //dest// off the list //list//.

Like //list// would be a stack, the last element of the list is poppend and
its content is written to //dest//, if provided at the end of the list. */
pboolean plist_pop( plist* list, void* dest )
{
	if( !( list ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !list->last )
		return FALSE;

	if( dest )
		memcpy( dest, plist_access( list->last ), list->size );

	plist_remove( list, list->last );
	return TRUE;
}

/** Retrieve list element by its index from the begin.

The function returns the //n//th element of the list //list//. */
punit* plist_get( plist* list, int n )
{
	punit*	e;

	if( !( list && n >= 0 ) )
	{
		WRONGPARAM;
		return (punit*)NULL;
	}

	for( e = plist_first( list ); e && n > 0;
			e = plist_next( e ), n-- )
		;

	return e;
}

/** Retrieve list element by hash-table key.

This function tries to fetch a list entry punit from list //list//
with the key //key//.
*/
punit* plist_get_by_key( plist* list, char* key )
{
	int		idx;
	punit*	e;

	if( !( list && key ) )
	{
		WRONGPARAM;
		return (punit*)NULL;
	}

	if( !list->hash )
		return (punit*)NULL;

	idx = plist_hash_index( list, key );

	for( e = list->hash[ idx ]; e; e = e->hashnext )
	{
		if( plist_hash_compare( list, e->key, key ) == 0 )
			return e;
	}

	return e;
}

/** Retrieve list element by pointer.

This function returns the list element of the unit within the list //list/
that is the pointer //ptr//.
*/
punit* plist_get_by_ptr( plist* list, void* ptr )
{
	punit*	e;

	if( !( list && ptr ) )
	{
		WRONGPARAM;
		return (punit*)NULL;
	}

	for( e = plist_first( list ); e; e = plist_next( e ) )
		if( plist_access( e ) == ptr )
			return e;

	return (punit*)NULL;
}

/** Access data-content of the current element //e//. */
void* plist_access( punit* e )
{
	if( !( e ) )
		return (void*)NULL;

	return (void*)( e + 1 );
}

/** Access next element of current unit //u//. */
punit* plist_next( punit* u )
{
	if( !( u ) )
		return (punit*)NULL;

	return u->next;
}

/** Access previous element of a current unit //u//. */
punit* plist_prev( punit* u )
{
	if( !( u ) )
		return (punit*)NULL;

	return u->prev;
}

/** Return the offset of the unit //u// within the list it belongs to. */
int plist_offset( punit* u )
{
	int		off		= 0;

	if( !( u && u->list ) )
		return -1;

	while( ( u = plist_prev( u ) ) )
		off++;

	return off;
}

/** Return first element of list //l//. */
punit* plist_first( plist* l )
{
	if( !( l ) )
		return (punit*)NULL;

	return l->first;
}

/** Return last element of list //l//. */
punit* plist_last( plist* l )
{
	if( !( l ) )
		return (punit*)NULL;

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
