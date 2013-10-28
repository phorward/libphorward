/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	plist.c
Usage:	An improved, double linked, optionally hashed list collection object.
----------------------------------------------------------------------------- */

#include "phorward.h"

/*
The plist-object implements

- a double linked-list
- hash-table
- dynamic stack functionalities

into one tool. It is the replacement for LIST, HASHTAB and STACK.
*/

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
static pboolean plist_hash_insert( plist* list, plistel* e )
{
	int			idx;
	plistel*	he;
	plistel**	bucket;

	if( !( list && e ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !e->key )
		return FALSE;

	if( !list->hash && !plist_hash_rebuild( list ) )
		return FALSE;

	e->hashnext = (plistel*)NULL;
	e->hashprev = (plistel*)NULL;

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
	plistel*	e;

	if( !list )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( list->hash )
		list->hash = pfree( list->hash );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		e->hashnext = (plistel*)NULL;

	list->hash = (plistel**)pmalloc( list->hashsize * sizeof( plistel* ) );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		plist_hash_insert( list, e );

	return TRUE;
}

/* Drop list element */
static pboolean plistel_drop( plistel* e )
{
	PROC( "plistel_drop" );

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

/** Creates an independent copy of //list// and returns it.
All elements of //list// are duplicated and stand-alone. */
plist* plist_dup( plist* list )
{
	plist*		dup;
	plistel*	e;

	if( !list )
	{
		WRONGPARAM;
		return (plist*)NULL;
	}

	dup = plist_create( list->size, list->flags );

	for( e = plist_first( list ); e; e = plist_next( e ) )
		plist_insert( dup, (plistel*)NULL, e->key, plist_access( e ) );

	return dup;
}

/** Erase all allocated content of the list //list//.

The object //list// will be still alive, but must be re-configured
using plist_init(). */
pboolean plist_erase( plist* list )
{
	plistel*	e;
	plistel*	next;

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

		plistel_drop( e );
		pfree( e );
	}

	MSG( "Freeing list of unused nodes" );
	for( e = list->unused; e; e = next )
	{
		next = e->next;
		pfree( e );
	}

	MSG( "Resetting list-object pointers" );
	list->first = (plistel*)NULL;
	list->last = (plistel*)NULL;
	list->hash = (plistel**)NULL;
	list->unused = (plistel*)NULL;
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
plistel* plist_insert( plist* list, plistel* pos, char* key, void* src )
{
	plistel*	e;
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
		RETURN( (plistel*)NULL );
	}

	/* Recycle existing elements? */
	if( list->unused )
	{
		MSG( "Recycle list contains element, recycling" );
		e = list->unused;
		list->unused = e->next;
		memset( e, 0, sizeof( plistel ) + list->size );
	}
	else
	{
		MSG( "Allocating new element" );
		VARS( "size", "%d", sizeof( plistel ) + list->size );

		e = (plistel*)pmalloc( sizeof( plistel ) + list->size );
	}

	e->list = list;

	if( src )
	{
		MSG( "data is provided, will copy memory" );
		VARS( "sizeof( plistel )", "%d", sizeof( plistel ) );
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
plistel* plist_push( plist* list, void* src )
{
	if( !( list ) )
	{
		WRONGPARAM;
		return (plistel*)NULL;
	}

	return plist_insert( list, (plistel*)NULL, (char*)NULL, src );
}

/** Removes the element //e// from the the //list// and free it or puts
 it into the unused element chain if PLIST_MOD_RECYCLE is flagged.

Will always return (plistel*)NULL. */
plistel* plist_remove( plist* list, plistel* e )
{
	PROC( "plist_remove" );

	if( !( list && e && e->list == list ) )
	{
		WRONGPARAM;
		RETURN( (plistel*)NULL );
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
	else if( list->hash )
		list->hash[ plist_hash_index( list, e->key ) ] = e->hashnext;

	/* Drop element contents */
	plistel_drop( e );

	/* Put unused node into unused list or free? */
	if( list->flags & PLIST_MOD_RECYCLE )
	{
		MSG( "Will recycle current element" );
		memset( e, 0, sizeof( plistel ) + list->size );

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
	RETURN( (plistel*)NULL );
}

/** Pop last element to //dest// off the list //list//.

Like //list// would be a stack, the last element of the list is poppend and
its content is written to //dest//, if provided at the end of the list.

//dest// can be omitted and given as (void*)NULL, so the last element will
be popped off the list and discards. */
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
plistel* plist_get( plist* list, int n )
{
	plistel*	e;

	if( !( list && n >= 0 ) )
	{
		WRONGPARAM;
		return (plistel*)NULL;
	}

	for( e = plist_first( list ); e && n > 0;
			e = plist_next( e ), n-- )
		;

	return e;
}

/** Retrieve list element by hash-table key.

This function tries to fetch a list entry plistel from list //list//
with the key //key//.
*/
plistel* plist_get_by_key( plist* list, char* key )
{
	int			idx;
	plistel*	e;

	if( !( list && key ) )
	{
		WRONGPARAM;
		return (plistel*)NULL;
	}

	if( !list->hash )
		return (plistel*)NULL;

	idx = plist_hash_index( list, key );

	for( e = list->hash[ idx ]; e; e = e->hashnext )
	{
		if( plist_hash_compare( list, e->key, key ) == 0 )
			return e;
	}

	return e;
}

/** Sort a list. */
pboolean plist_subsort( plistel* from, plistel* to,
							pboolean (*less)( void*, void * ) )
{
	plistel*	a	= from;
	plistel*	b	= to;
	plistel*	e;
	void*		ref;

	int			i	= 0;
	int			j	= 0;

	if( !( from && to && less ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( from == to )
		return TRUE;

	while( a != b )
	{
		j++;

		if( !( a = a->next ) )
		{
			WRONGPARAM;
			return FALSE;
		}
	}

	a = from;
	ref = plist_access( from );

	do
	{
		while( (*less)( plist_access( a ), ref ) )
		{
			i++;
			a = a->next;
		}

		while( (*less)( ref, plist_access( b ) ) )
		{
			j--;
			b = b->prev;
		}

		if( i <= j )
		{
			if( from == a )
				from = b;
			else if( from == b )
				from = a;

			if( to == a )
				to = b;
			else if( to == b )
				to = a;

			plist_swap( a, b );

			e = a;
			a = b->next;
			b = e->prev;

			i++;
			j--;
		}
	}
	while( i <= j );

	if( ( b != from ) && ( b != from->prev ) )
		plist_subsort( from, b, less );

	if( ( a != to ) && ( a != to->next ) )
		plist_subsort( a, to, less );

	return TRUE;
}

pboolean plist_sort( plist* list, pboolean (*less)( void*, void * ) )
{
	if( !( list && less ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !plist_first( list ) )
		return TRUE;

	return plist_subsort( plist_first( list ), plist_last( list ), less );
}

/** Retrieve list element by pointer.

This function returns the list element of the unit within the list //list//
that is the pointer //ptr//.
*/
plistel* plist_get_by_ptr( plist* list, void* ptr )
{
	plistel*	e;

	if( !( list && ptr ) )
	{
		WRONGPARAM;
		return (plistel*)NULL;
	}

	for( e = plist_first( list ); e; e = plist_next( e ) )
		if( plist_access( e ) == ptr )
			return e;

	return (plistel*)NULL;
}

/** Access data-content of the current element //e//. */
void* plist_access( plistel* e )
{
	if( !( e ) )
		return (void*)NULL;

	return (void*)( e + 1 );
}

/** Access next element of current unit //u//. */
plistel* plist_next( plistel* u )
{
	if( !( u ) )
		return (plistel*)NULL;

	return u->next;
}

/** Access previous element of a current unit //u//. */
plistel* plist_prev( plistel* u )
{
	if( !( u ) )
		return (plistel*)NULL;

	return u->prev;
}

/** Return the offset of the unit //u// within the list it belongs to. */
int plist_offset( plistel* u )
{
	int		off		= 0;

	if( !( u && u->list ) )
		return -1;

	while( ( u = plist_prev( u ) ) )
		off++;

	return off;
}

/** Swaps the positions of the list elemements //a// and //b// with each
other. The elements must be in the same plist object, else the function
returns with FALSE. */
pboolean plist_swap( plistel* a, plistel* b )
{
	plist*		l;
	plistel*	aprev;
	plistel*	anext;
	plistel*	bprev;
	plistel*	bnext;

	if( !( a && b ) || b->list != a->list )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( a == b )
		return TRUE;

	/* Retrieve pointers */
	l = a->list;
	aprev = a->prev;
	anext = a->next;
	bprev = b->prev;
	bnext = b->next;

	/* a next */
	if( anext == b )
	{
		a->prev = b;
		b->next = a;
	}
	else if( ( b->next = anext ) )
		anext->prev = b;

	/* a prev */
	if( aprev == b )
	{
		a->next = b;
		b->prev = a;
	}
	else if( ( b->prev = aprev ) )
		aprev->next = b;

	/* b next */
	if( bnext == a )
	{
		b->prev = a;
		a->next = b;
	}
	else if( ( a->next = bnext ) )
		bnext->prev = a;

	/* b prev */
	if( bprev == a )
	{
		b->next = a;
		a->prev = b;
	}
	else if( ( a->prev = bprev ) )
		bprev->next = a;

	/* first */
	if( a == l->first )
		l->first = b;
	else if( b == l->first )
		l->first = a;

	/* last */
	if( a == l->last )
		l->last = b;
	else if( b == l->last )
		l->last = a;

	return TRUE;
}

/** Return first element of list //l//. */
plistel* plist_first( plist* l )
{
	if( !( l ) )
		return (plistel*)NULL;

	return l->first;
}

/** Return last element of list //l//. */
plistel* plist_last( plist* l )
{
	if( !( l ) )
		return (plistel*)NULL;

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
