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

	PROC( "plist_hash_insert" );
	PARMS( "list", "%p", list );
	PARMS( "e", "%p", e );

	if( !( list && e ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !e->key )
		RETURN( FALSE );

	e->hashnext = (plistel*)NULL;
	e->hashprev = (plistel*)NULL;

	bucket = &( list->hash[ plist_hash_index( list, e->key ) ] );

	if( ! *bucket )
	{
		MSG( "Bucket is empty, chaining start position" );
		*bucket = e;
	}
	else
	{
		MSG( "Chaining into hash" );

		for( he = *bucket; he; he = he->hashnext )
		{
			VARS( "he->key", "%s", he->key );
			VARS( "e->key", "%s", e->key );

			if( plist_hash_compare( list, he->key, e->key ) == 0 )
			{
				if( list->flags & PLIST_MOD_UNIQUE )
					RETURN( FALSE );

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

	RETURN( TRUE );
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

/* Compare elements of a list */
static int plist_compare( plist* list, plistel* l, plistel* r )
{
	if( !( list && l && r ) )
	{
		WRONGPARAM;
		return -1;
	}

	if( list->comparefn )
		return (*list->comparefn)( list, l, r );

	return memcmp( l + 1, r + 1, list->size );
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

	/* A size of zero causes a pointer list allocation,
		so the flag must also be set. */
	if( size == 0 )
		flags |= PLIST_MOD_PTR;

	if( flags & PLIST_MOD_PTR && size < sizeof( void* ) )
		size = sizeof( void* );

	memset( list, 0, sizeof( plist ) );
	list->flags = flags;
	list->size = size;
	list->hashsize = 64;

	list->sortfn = plist_compare;

	return TRUE;
}

/** Create a new plist as an object with an element allocation size //size//.
Providing a //size// of 0 causes automatic configuration of PLIST_MOD_PTR.

//flags// defines an optional flag configuration that modifies the behavior
of the linked list and hash table usage. The flags can be merged together using
bitwise or (|).

Possible flags are:
- **PLIST_MOD_NONE** for no special flagging.
- **PLIST_MOD_PTR** to use the plist-object in pointer-mode: Each plistel-element cointains only a pointer to an object in the memory and returns this, instead of copying from or into pointers.
- **PLIST_MOD_RECYCLE** to configure that elements that are removed during list usage will be reused later.
- **PLIST_MOD_EXTKEYS** to configure that string pointers to hash-table key values are stored elsewhere, so the plist-module only uses the original pointers instead of copying them.
- **PLIST_MOD_UNIQUE** to disallow hash-table-key collisions, so elements with a key that already exist in the object will be rejected.
- **PLIST_MOD_WCHAR** to let all key values handle as wide-character strings.
-

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

/** Clear content of the list //list//.

The function has nearly the same purpose as plist_erase(), except that
the entire list is only cleared, but if the list was initialized with
PLIST_MOD_RECYCLE, existing pointers are hold for later usage. */
pboolean plist_clear( plist* list )
{
	plistel*	e;

	PROC( "plist_clear" );

	if( !( list ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	while( list->first )
		plist_remove( list, list->first );

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

	/* Rebuild hash-table if necessary */
	if( key && !list->hash && !plist_hash_rebuild( list ) )
		RETURN( (plistel*)NULL );

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

		if( list->flags & PLIST_MOD_PTR )
		{
			MSG( "Pointer mode, just store the pointer" );
			*( (void**)( e + 1 ) ) = src;
		}
		else
		{
			MSG( "Copy memory of size-bytes" );
			memcpy( e + 1, src, list->size );
		}
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

		if( !plist_hash_insert( list, e ) )
		{
			MSG( "This should not happen..." );
		}
	}

	if( list->flags & PLIST_MOD_AUTOSORT )
		plist_sort( list );

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

/** Allocates memory for a new element in list //list//, push it to the end and
return the pointer to this.

The function works as a shortcut for plist_access() in combination with
plist_push().
*/
void* plist_malloc( plist* list )
{
	if( !( list ) )
	{
		WRONGPARAM;
		return (void*)NULL;
	}

	return plist_access( plist_push( list, (void*)NULL ) );
}

/** Removes the element //e// from the the //list// and frees it or puts
 it into the unused element chain if PLIST_MOD_RECYCLE is flagged. */
pboolean plist_remove( plist* list, plistel* e )
{
	PROC( "plist_remove" );

	if( !( list && e && e->list == list ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
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
	RETURN( TRUE );
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
	{
		if( list->flags & PLIST_MOD_PTR )
			*( (void**)dest ) = plist_access( list->last );
		else
			memcpy( dest, plist_access( list->last ), list->size );
	}

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
	int			i = 0;
	int			idx;
	plistel*	e;

	PROC( "plist_get_by_key" );
	PARMS( "list", "%p", list );
	PARMS( "key", "%s", key );

	if( !( list && key ) )
	{
		WRONGPARAM;
		RETURN( (plistel*)NULL );
	}

	if( !list->hash )
		RETURN( (plistel*)NULL );

	idx = plist_hash_index( list, key );
	VARS( "idx", "%d", idx );

	for( e = list->hash[ idx ]; e; e = e->hashnext )
	{
		VARS( "e", "%p", e );
		VARS( "e->key", "%s", e->key );

		if( plist_hash_compare( list, e->key, key ) == 0 )
		{
			MSG( "Key matches" );
			RETURN( e );
		}
	}

	RETURN( e );
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

/** Unions elements from list //from// into list //all//.
An element is only added to //all//, if there exists no other
element with the same size and content.

The function will not run if both lists have different element size settings.

The function returns the number of elements added to //from//. */
int plist_union( plist* all, plist* from )
{
	int			added	= 0;
	plistel*	ea;
	plistel*	ef;

	PROC( "plist_union" );
	PARMS( "all", "%p", all );
	PARMS( "from", "%p", from );

	if( !( all && from && all->size == from->size ) )
	{
		WRONGPARAM;
		RETURN( 0 );
	}

	plist_for( from, ef )
	{
		VARS( "ef", "%p", ef );
		plist_for( all, ea )
		{
			VARS( "ea", "%p", ea );

			if( plist_compare( all, ea, ef ) == 0 )
			{
				MSG( "Elements match" );
				break;
			}
		}

		if( !ea && plist_insert( all, (plistel*)NULL,
						(char*)NULL, plist_access( ef ) ) )
		{
			MSG( "Added element" );
			added++;
		}
	}

	VARS( "added", "%d", added );
	RETURN( added );
}

/** Checks the contents of list //left// and list //right//for equal elements. */
int plist_diff( plist* left, plist* right )
{
	plistel*	el;
	plistel*	er;
	int			diff;

	PROC( "plist_diff" );
	PARMS( "left", "%p", left );
	PARMS( "right", "%p", right );

	if( !( left && right && left->size == right->size ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	MSG( "Checking for same list sizes" );
	if( !( diff = right->count - left->count ) )
	{
		MSG( "OK, requiring deep check" );

		for( el = plist_first( left ), er = plist_first( right );
					el && er; el = plist_next( el ), er = plist_next( er ) )
		{
			if( ( diff = plist_compare( left, el, er ) ) )
			{
				MSG( "Elements are not equal" );
				VARS( "diff", "%d", diff );
				RETURN( diff );
			}
		}
	}

	VARS( "diff", "%d", diff );
	RETURN( diff );
}

/** Moves all elements from list //src// into list //dst//.

Both lists must have the same element size, else the move operation cannot be
performed.
*/
/*
int plist_move( plist* dst, plist* src )
{
	int			count	= 0;
	plistel*	e;

	PROC( "plist_move" );
	PARMS( "dst", "%p", dst );
	PARMS( "src", "%p", src );

	if( !( dst && src && dst->size == src->size ) )
	{
		WRONGPARAM;
		RETURN( 0 );
	}

	MSG( "Looping trough elements" );
	plist_for( src, e )
	{

	}

	RETURN( count );
}
*/

/** Sort a list. */
pboolean plist_subsort( plist* list, plistel* from, plistel* to )
{
	plistel*	a	= from;
	plistel*	b	= to;
	plistel*	e;
	void*		ref;

	int			i	= 0;
	int			j	= 0;

	if( !( list && from && to && from->list == list && to->list == list ) )
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
		while( ( *list->sortfn )( from->list, a, from ) < 0 )
		{
			i++;
			a = a->next;
		}

		while( ( *list->sortfn )( from->list, from, b ) < 0 )
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
		plist_subsort( list, from, b );

	if( ( a != to ) && ( a != to->next ) )
		plist_subsort( list, a, to );

	return TRUE;
}

pboolean plist_sort( plist* list )
{
	if( !( list ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !plist_first( list ) )
		return TRUE;

	return plist_subsort( list, plist_first( list ), plist_last( list ) );
}

/** Set compare function */
pboolean plist_set_comparefn( plist* list,
			int (*comparefn)( plist*, plistel*, plistel* ) )
{
	PROC( "plist_set_comparefn" );
	PARMS( "list", "%p", list );
	PARMS( "compare_fn", "%p", comparefn );

	if( !( list ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !( list->comparefn = comparefn ) )
		list->comparefn = plist_compare;

	RETURN( TRUE );
}

/** Set sort function */
pboolean plist_set_sortfn( plist* list,
			int (*sortfn)( plist*, plistel*, plistel* ) )
{
	PROC( "plist_set_sortfn" );
	PARMS( "list", "%p", list );
	PARMS( "sortfn", "%p", sortfn );

	if( !( list ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !( list->sortfn = sortfn ) )
		list->sortfn = plist_compare;

	RETURN( TRUE );
}

/** Access data-content of the current element //e//. */
void* plist_access( plistel* e )
{
	if( !( e ) )
		return (void*)NULL;

	/* Dereference pointer list differently */
	if( e->list->flags & PLIST_MOD_PTR )
		return *((void**)( e + 1 ));

	return (void*)( e + 1 );
}

/** Access key-content of the current element //e//. */
char* plist_key( plistel* e )
{
	if( !( e ) )
		return (char*)NULL;

	return e->key;
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
