/* -MODULE----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	array.c
Author:	Jan Max Meyer
Usage:	Universal, dynamic array management functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

#ifndef PARRAY_DEFAULT_CHUNK
#define PARRAY_DEFAULT_CHUNK		128			/* Default chunk size */
#endif

/* Compare elements of a list */
static int parray_compare( parray* array, void* l, void* r )
{
	if( array->comparefn )
		return (*array->comparefn)( array, l, r );

	return -memcmp( l, r, array->size );
}

/** Performs an array initialization.

//array// is the pointer to the array to be initialized.

//size// defines the size of one array element, in bytes.
This should be evaluated using the sizeof()-macro.

//chunk// defines the chunk size, when an array-(re)allocation will be
performed. If, e.g. this is set to 128, then, if the 128th item is created
within the array, a realloction is done. Once allocated memory remains until
the array is freed again. The array's elements may change their heap address
when a chunk reallocation is required.
*/
void parray_init( parray* array, size_t size, size_t chunk )
{
	if( !chunk )
		chunk = PARRAY_DEFAULT_CHUNK;

	memset( array, 0, sizeof( parray ) );
	array->size = size;
	array->chunk = chunk;

	array->sortfn = parray_compare;
}

/** Create a new parray as an object with an element allocation size //size//,
a reallocation-chunk-size of //chunk//.

The returned memory must be released with parray_free().  */
parray* parray_create( size_t size, size_t chunk )
{
	parray*	array;

	array = (parray*)pmalloc( sizeof( parray ) );
	parray_init( array, size, chunk );

	return array;
}

/** Erase a dynamic array.

The array must not be reinitialized after destruction, using parray_init().

//array// is the pointer to the array to be erased. */
void parray_erase( parray* array )
{
	array->bottom = array->top = array->start = pfree( array->start );
	array->count = array->first = array->last = 0;
}

/** Releases all the memory //array// uses and destroys the array object.

The function always returns (parray*)NULL. */
parray* parray_free( parray* array )
{
	if( !array )
		return (parray*)NULL;

	parray_erase( array );
	pfree( array );

	return (parray*)NULL;
}

/** Appends an element to the end of the array.

The element's memory is copied during the push. The item must be of the same
memory size as used at array initialization.

//array// is the pointer to array where to push an item on.

//item// is the pointer to the memory of the item that should be pushed onto the
array. The caller should cast his type into void, or wrap the push-operation
with a macro. It can be left NULL, so no memory will be copied.

The function returns the address of the newly pushed item, and NULL if
the item could not be pushed.
*/
void* parray_push( parray* array, void* item )
{
	/* Is reallocation required? */
	if( !array->count || array->last == array->count )
	{
		array->count += array->chunk;

		if( !( array->start = (void*)prealloc(
				(void*)array->start, array->count * array->size ) ) )
			return NULL;
	}

	array->bottom = array->start + array->first * array->size;
	array->top = array->start + ++array->last * array->size;

	/* Copy item into last of array */
	if( item )
		memcpy( array->top - array->size, item, array->size );

	return array->top - array->size;
}

/** Reserves memory for //n// items in //array//.

This function is only used to assume that no memory reallocation is done when
the next //n// items are inserted/malloced. */
void* parray_reserve( parray* array, size_t n )
{
	if( array->last + n < array->count )
		return array->bottom + n;

	array->count += n + ( n % array->chunk );

	if( !( array->start = (void*)prealloc(
			(void*)array->start, array->count * array->size ) ) )
		return NULL;

	array->bottom = array->start + array->first * array->size;
	array->top = array->start + array->last * array->size;

	return array->bottom + n;
}

/** Pushes and "allocates" an empty element on the array.

This function is just a shortcut to ```parray_push( array, NULL )```,
and the memory of the pushed element is initialized to zero. */
void* parray_malloc( parray* array )
{
	void*	ptr;

	if( !( ptr = parray_push( array, NULL ) ) )
		return ptr;

	memset( ptr, 0, array->size );
	return ptr;
}

/** Unshifts and "allocates" an empty element on the array.

This function is just a shortcut to ```parray_unshift( array, NULL )```,
and the memory of the unshifted element is initialized to zero. */
void* parray_rmalloc( parray* array )
{
	void*	ptr;

	if( !( ptr = parray_unshift( array, NULL ) ) )
		return ptr;

	memset( ptr, 0, array->size );
	return ptr;
}

/** Insert item //item// at //offset// into array //array//.
Items right to //offset// will move up.

Gap space between the offset is filled with zero elements;
Handle with care! */
void* parray_insert( parray* array, size_t offset, void* item )
{
	void*	slot;

	/* Within current bounds? */
	if( array->first + offset < array->last )
	{
		/* Allocate one item for moving up */
		parray_malloc( array );

		slot = array->start + ( array->first + offset ) * array->size;

		/* Move up existing items right of offset */
		memmove( slot + array->size, slot,
					( array->last - 1 -
						( array->first + offset ) ) * array->size );

		/* Put new element */
		if( item )
			memcpy( slot, item, array->size );
		else
			memset( slot, 0, array->size );

		return slot;
	}

	while( array->first + offset >= array->last )
		if( !( slot = parray_malloc( array ) ) )
			return slot;

	if( item )
		memcpy( slot, item, array->size );

	return slot;
}

/** Remove item on //offset// from array //array//.

The removed item will be copied into //item//, if //item// is not NULL.
The function returns the memory of the removed item (it will contain the
moved up data part or invalid memory, if on the end). */
void* parray_remove( parray* array, size_t offset, void** item )
{
	void*	slot;

	/* Within current bounds? */
	if( array->first + offset >= array->last )
		return NULL;

	slot = array->start + ( array->first + offset ) * array->size;

	if( item )
		memcpy( *item, slot, ( array->first + offset ) * array->size );

	/* Move existing items to the left */
	if( array->first + offset + 1 < array->last )
		memmove( slot, slot + array->size,
				( array->last - ( array->first + offset + 1 ) )
					* array->size );

	array->last--;
	array->top -= array->size;

	return slot;
}

/** Removes an element from the end of an array.

The function returns the pointer of the popped item. Because dynamic arrays only
grow and no memory is freed, the returned data pointer is still valid, and will
only be overridden with the next push operation.

//array// is the pointer to array where to pop an item off.

The function returns the address of the popped item, and NULL if the
item could not be popped (e.g. array is empty).
*/
void* parray_pop( parray* array )
{
	if( array->last == array->first )
		/* last is zero, no items on the array */
		return NULL;

	array->top -= array->size;

	return array->start + ( ( --array->last ) * array->size );
}

/** Appends an element to the begin of the array.

The elements memory is copied during the unshift. The item must be of the same
memory size as used at array initialization.

//array// is the pointer to array where to push an item to the beginning.

//item// is the pointer to the memory of the item that should be pushed onto the
array. The caller should cast his type into void, or wrap the push-operation
with a macro. It can be left NULL, so no memory will be copied.

The function returns the address of the newly unshifted item, and NULL
if the item could not be unshifted.
*/
void* parray_unshift( parray* array, void* item )
{
	/* Is reallocation required? */
	if( !array->count || array->first == 0 )
	{
		array->count += array->chunk;

		if( !( array->start = (void*)prealloc(
				(void*)array->start, array->count * array->size ) ) )
			return NULL;

		array->first = array->chunk;

		if( array->last > 0 )
			memmove( array->start + array->first * array->size,
						array->start, array->last * array->size );

		array->last += array->first;

		array->bottom = array->start + array->first * array->size;
		array->top = array->start + array->last * array->size;
	}

	array->bottom -= array->size;
	array->first--;

	/* Copy item into last of array */
	if( item )
		memcpy( array->bottom, item, array->size );

	return array->bottom;
}

/** Removes an element from the begin of an array.

The function returns the pointer of the shifted item.
Because dynamic arrays only grow and no memory is freed, the returned data
pointer is still valid, and will only be overridden with the next unshift
operation.

//array// is the pointer to array where to pop an item off.

The function returns the address of the shifted item, and NULL if the
item could not be popped (e.g. array is empty).
*/
void* parray_shift( parray* array )
{
	if( array->last == array->first )
		/* last is zero, no items on the array. */
		return NULL;

	array->bottom += array->size;
	return array->start + array->first++ * array->size;
}

/** Access an element from the array by its offset position from the left.

//array// is the pointer to array where to access the element from.
//offset// is the offset of the element to be accessed from the array's
base address.

Returns the address of the accessed item, and NULL if the item could not
be accessed (e.g. if the array is empty or offset is beyond the top of array).

Use parray_rget() for access items from the end.
*/
void* parray_get( parray* array, size_t offset )
{
	if( array->last == array->first
			|| offset >= ( array->last - array->first ) )
		/* offset defines an item that is out of bounds of the array */
		return NULL;

	return array->start + ( array->first + offset ) * array->size;
}

/** Put an element //item// at position //offset// of array //array//.

//array// is the pointer to array where to put the element to.
//offset// is the offset of the element to be set.
//item// is a pointer to the memory that will be copied into the
position at //offset//. If this is NULL, the position at //offset// will be
set to zero.

Returns the address of the item in the array, or NULL if the desired offset
is out of the array bounds.
*/
void* parray_put( parray* array, size_t offset, void* item )
{
	void*	slot;

	if( array->last == array->first
			|| offset >= ( array->last - array->first ) )
		/* offset defines an item that is out of bounds of the array */
		return NULL;

	slot = array->start + ( array->first + offset ) * array->size;

	if( item )
		memcpy( slot, item, array->size );
	else
		memset( slot, 0, array->size );

	return slot;
}

/** Access an element from the array by its offset position from the right.

//array// is the pointer to array where to access the element from.
//offset// is the offset of the element to be accessed from the array's
base address.

Returns the address of the accessed item, and NULL if the item could not
be accessed (e.g. if the array is empty or offset is beyond the bottom of
the array).

Use parray_get() to access items from the begin.
*/
void* parray_rget( parray* array, size_t offset )
{
	return parray_get( array, array->last - offset );
}

/** Put an element //item// at position //offset// from the right of
array //array//.

//array// is the pointer to array where to put the element to.
//offset// is the offset of the element to be set.
//item// is a pointer to the memory that will be copied into the
position at //offset//. If this is NULL, the position at //offset// will be
set to zero.

Returns the address of the item in the array, or NULL if the desired offset
is out of the array bounds.
*/
void* parray_rput( parray* array, size_t offset, void* item )
{
	return parray_put( array, array->last - offset, item );
}

/** Iterates over //array//.

Iterates over all items of //array// and calls the function //callback// on
every item. */
void parray_iter( parray* array, parrayfn callback )
{
	void*	ptr;

	parray_for( array, ptr )
		(*callback)( array, ptr );
}

/** Iterates backwards over //array//.

Backwardly iterates over all items of //array// and calls the function
//callback// on every item. */
void parray_riter( parray* array, parrayfn callback )
{
	void*	ptr;

	for( ptr = parray_last( array ); ptr; ptr = parray_prev( array, ptr ) )
		(*callback)( array, ptr );
}

/** Access first element of the array.

Returns the address of the accessed item, and NULL if nothing is in
the array.
*/
void* parray_first( parray* array )
{
	if( array->first == array->last )
		return NULL;

	return array->bottom;
}

/** Access last element of the array.

Returns the address of the accessed item, and NULL if nothing is in
the array.
*/
void* parray_last( parray* array )
{
	if( array->first == array->last )
		return NULL;

	return array->top - array->size;
}

/** Access next element from //ptr// in //array//.

Returns the address of the next element, and NULL if the access gets
out of bounds.
*/
void* parray_next( parray* array, void* ptr )
{
	ptr += array->size;
	if( ptr > parray_last( array ) || ptr < parray_first( array ) )
		return NULL;

	return ptr;
}

/** Access previous element from //ptr// in //array//.

Returns the address of the previous element, and NULL if the access gets
out of bounds.
*/
void* parray_prev( parray* array, void* ptr )
{
	ptr -= array->size;
	if( ptr < parray_first( array ) || ptr > parray_last( array ) )
		return NULL;

	return ptr;
}

/** Swap two elements of an array. */
void parray_swap( parray* array, size_t pos1, size_t pos2 )
{
	void*	ptr1;
	void*	ptr2;

	if( pos1 == pos2 || pos1 >= array->count || pos2 >= array->count )
		return;

	/* Reserve one entry first before accessing and moving elements. */
	parray_reserve( array, 1 );

	ptr1 = parray_get( array, pos1 );
	ptr2 = parray_get( array, pos2 );

	parray_push( array, ptr1 );
	parray_put( array, pos1, ptr2 );
	parray_put( array, pos2, parray_pop( array ) );
}

/** Returns the number of elements in a array. */
size_t parray_count( parray* array )
{
	if( !array )
		return 0;

	return array->last - array->first;
}

/** Returns TRUE, if //ptr// is an element of array //array//. */
void* parray_partof( parray* array, void* ptr )
{
	if( ptr >= (void*)array->bottom && ptr <= (void*)array->top )
		return ptr;

	return NULL;
}

/** Return offset of element //ptr// in array //array//.

Returns the offset of //ptr// in //array//.
The function returns the size of the array (which is an invalid offset)
if //ptr// is not part of //array//.

To check if a pointer belongs to an array, call parray_partof(). */
size_t parray_offset( parray* array, void* ptr )
{
	if( !parray_partof( array, ptr ) )
		return parray_count( array );

	return ( (char*)ptr - ( array->start + ( array->first * array->size ) ) )
				/ array->size;
}

/*TESTCASE:parray_(create|free|push|pop|shift|insert|unshift|remove)
#include <phorward.h>

typedef struct
{
	char	firstname	[ 30 + 1 ];
	char	lastname	[ 30 + 1 ];
} person;

static void dump( parray* a )
{
	int		i;
	person*	p;

	printf( "first = %ld last = %ld count = %ld\n",
				a->first, a->last, a->count );

	printf( "-- %d Elements --\n", parray_count( a ) );

	for( i = 0; ( p = (person*)parray_get( a, i ) ); i++ )
		printf( "%02d) %s %s\n", i, p->firstname, p->lastname );

	printf( "-- %d Elements --\n", parray_count( a ) );
}

void testcase( void )
{
	person 	x;
	person*	p;
	parray*	a;

	a = parray_create( sizeof( person ), 0 );

	strcpy( x.lastname, "Zufall" );
	strcpy( x.firstname, "Reiner" );
	parray_push( a, (void*)&x );

	dump( a );

	strcpy( x.lastname, "Pfahl" );
	strcpy( x.firstname, "Martha" );
	p = (person*)parray_insert( a, 5, (void*)&x );

	dump( a );
	printf( "%ld\n", parray_offset( a, p - 10 ) );

	strcpy( x.lastname, "Racho" );
	strcpy( x.firstname, "Volker" );
	parray_unshift( a, (void*)&x );

	dump( a );

	strcpy( x.lastname, "Pete" );
	strcpy( x.firstname, "Dieter" );
	parray_unshift( a, (void*)&x );

	dump( a );

	parray_remove( a, 1, (void**)NULL );

	dump( a );

	a = parray_free( a );
}

--------------------------------------------------------------------------------
first = 0 last = 1 count = 128
-- 1 Elements --
00) Reiner Zufall
-- 1 Elements --
first = 0 last = 6 count = 128
-- 6 Elements --
00) Reiner Zufall
01)
02)
03)
04)
05) Martha Pfahl
-- 6 Elements --
6
first = 127 last = 134 count = 256
-- 7 Elements --
00) Volker Racho
01) Reiner Zufall
02)
03)
04)
05)
06) Martha Pfahl
-- 7 Elements --
first = 126 last = 134 count = 256
-- 8 Elements --
00) Dieter Pete
01) Volker Racho
02) Reiner Zufall
03)
04)
05)
06)
07) Martha Pfahl
-- 8 Elements --
first = 126 last = 133 count = 256
-- 7 Elements --
00) Dieter Pete
01) Reiner Zufall
02)
03)
04)
05)
06) Martha Pfahl
-- 7 Elements --
TESTCASE*/

/** Concats the elements of array //src// to the elements of array //dest//.

The function will not run if both arrays have different element size settings.

The function returns the number of elements added to //dest//. */
size_t parray_concat( parray* dest, parray* src )
{
	size_t		count;
	char*       p;

	if( dest->size != src->size )
		return 0;

	count = dest->count;

	/* fixme: This can be done much faster! */
	for( p = src->bottom; p && p < src->top; p += src->size )
		if( !parray_push( dest, p ) )
			break;

	return dest->count - count;
}

/** Unions elements from array //from// into array //all//.

An element is only added to //all//, if there exists no equal element with the
same size and content.

The function will not run if both arrays have different element size settings.

The function returns the number of elements added to //from//. */
size_t parray_union( parray* all, parray* from )
{
	size_t		last;
	char*		top;
	char*       p;
	char*       q;

	if( !( all->size == from->size && all->comparefn == from->comparefn ) )
		return 0;

	if( !( last = all->last ) )
		return parray_concat( all, from );

	for( p = from->bottom; p < from->top; p += from->size )
	{
		top = all->bottom + last * all->size;

		for( q = all->bottom; q < top; q += all->size )
			if( parray_compare( all, p, q ) == 0 )
				break;

		if( q == top )
			if( !parray_push( all, p ) )
				break;
	}

	return parray_count( all ) - last;
}

/*TESTCASE:parray_union
#include <phorward.h>

void dump( parray* a, void* p )
{
	printf( "%c%s", *((char*)p), p == parray_last( a ) ? "\n" : "" );
}

void testcase()
{
	parray  a;
	parray  b;

	parray_init( &a, sizeof( char ), 0 );
	parray_init( &b, sizeof( char ), 0 );

	parray_push( &a, "a" );
	parray_push( &a, "b" );
	parray_push( &a, "c" );

	parray_push( &b, "a" );
	parray_push( &b, "d" );

	parray_iter( &a, dump );
	parray_iter( &b, dump );

	printf( "%ld\n", parray_union( &b, &a ) );

	parray_iter( &a, dump );
	parray_iter( &b, dump );
}
---
abc
ad
2
abc
adbc
*/

/** Tests the contents (data parts) of the array //left// and the array //right//
for equal elements.

The function returns a value < 0 if //left// is lower //right//, a value > 0
if //left// is greater //right// and a value == 0 if //left// is equal to
//right//. */
int parray_diff( parray* left, parray* right )
{
	int		diff;
	char*   p;
	char*   q;


	if( !( left->size == right->size && left->comparefn == right->comparefn ) )
		return -1;

	/* Checking for same element count */
	if( parray_count( right ) < parray_count( left ) )
		return 1;
	else if( parray_count( right ) > parray_count( left ) )
		return -1;

	/* OK, requiring deep check */
	for( p = left->bottom, q = right->bottom;
			p < left->top && q < right->top;
				p += left->size, q += right->size )
	{
		if( ( diff = parray_compare( left, p, q ) ) )
			break;
	}

	return diff;
}

/*TESTCASE:parray_diff
#include <phorward.h>

void dump( parray* a, void* p )
{
	printf( "%c%s", *((char*)p), p == parray_last( a ) ? "\n" : "" );
}

void testcase()
{
	parray  a;
	parray  b;

	parray_init( &a, sizeof( char ), 0 );
	parray_init( &b, sizeof( char ), 0 );

	parray_push( &a, "a" );
	parray_push( &a, "b" );

	parray_push( &b, "a" );
	parray_push( &b, "b" );

	parray_iter( &a, dump );
	parray_iter( &b, dump );

	printf( "%d\n", parray_diff( &a, &b ) );

	parray_push( &b, "c" );

	printf( "%d\n", parray_diff( &a, &b ) );

	parray_push( &a, "c" );

	printf( "%d\n", parray_diff( &a, &b ) );

	parray_shift( &b );

	parray_iter( &a, dump );
	parray_iter( &b, dump );

	printf( "%d\n", parray_diff( &a, &b ) );

	parray_shift( &a );
	parray_pop( &b );

	parray_iter( &a, dump );
	parray_iter( &b, dump );

	printf( "%d\n", parray_diff( &a, &b ) );
	printf( "%d\n", parray_diff( &b, &a ) );
}
---
ab
ab
0
-1
0
abc
bc
1
bc
b
1
-1
*/

/** Sorts //array// between the elements //from// and //to// according to the
sort-function that was set for the array.

To sort the entire array, use parray_sort().

The sort-function can be modified by using parray_set_sortfn().

The default sort function sorts the list by content using the memcmp()
standard function. */
void parray_subsort( parray* array, size_t from, size_t to )
{
	size_t	a	    = from;
	size_t	b	    = to;
	size_t  ref;

	if( from == to )
		return;

	if( from > to )
	{
		ref = from;
		from = to;
		to = ref;
	}

	a = ref = from;

	do
	{
		while( ( *array->sortfn )(
			array, parray_get( array, a ), parray_get( array, ref ) )
				> 0 )
			a++;

		while( ( *array->sortfn )(
			array, parray_get( array, ref ), parray_get( array, b ) )
				> 0 )
			b--;

		if( a <= b )
		{
			parray_swap( array, a, b );
			a++;

			if( b )
				b--;
		}
	}
	while( a <= b );

	if( ( b != from ) && ( b != from - 1 ) )
		parray_subsort( array, from, b );

	if( ( a != to ) && ( a != to + 1 ) )
		parray_subsort( array, a, to );
}

/** Sorts //list// according to the sort-function that was set for the list.

To sort only parts of a list, use plist_subsort().

The sort-function can be modified by using plist_set_sortfn().

The default sort function sorts the list by content using the memcmp()
standard function. */
void parray_sort( parray* array )
{
	if( !parray_first( array ) )
		return;

	parray_subsort( array, array->first, array->last - 1 );
}

/*TESTCASE:parray_sort
#include <phorward.h>

void dump( parray* a, void* p )
{
	printf( "%c%s", *((char*)p), p == parray_last( a ) ? "\n" : "" );
}

int sort( parray* a, void* p, void* q )
{
	int ret = toupper( *((char*)q) ) - toupper( *((char*)p) );

	if( ret == 0 )
		return *((char*)q) - *((char*)p);

	return ret;
}

void testcase()
{
	parray  a;
	parray  b;

	parray_init( &a, sizeof( char ), 0 );
	parray_init( &b, sizeof( char ), 0 );

	parray_push( &a, "c" );
	parray_push( &a, "d" );
	parray_push( &a, "a" );
	parray_push( &a, "b" );
	parray_push( &a, "b" );
	parray_push( &a, "B" );
	parray_push( &a, "k" );
	parray_push( &a, "e" );
	parray_push( &a, "A" );
	parray_push( &a, "x" );

	parray_concat( &b, &a );

	parray_iter( &a, dump );
	parray_sort( &a );
	parray_iter( &a, dump );

	parray_set_sortfn( &b, sort );

	parray_iter( &b, dump );
	parray_sort( &b );
	parray_iter( &b, dump );
}
---
cdabbBkeAx
ABabbcdekx
cdabbBkeAx
AaBbbcdekx
*/

/** Sets array compare function.

If no compare function is set or NULL is provided, memcmp() will be used
as default fallback. */
void parray_set_comparefn( parray* array,
			int (*comparefn)( parray*, void*, void* ) )
{
	if( !( array->comparefn = comparefn ) )
		array->comparefn = parray_compare;
}

/** Sets array sort function.

If no sort function is given, the compare function set by parray_set_comparefn()
is used. If even unset, memcmp() will be used. */
void parray_set_sortfn( parray* array,
			int (*sortfn)( parray*, void*, void* ) )
{
	if( !( array->sortfn = sortfn ) )
		array->sortfn = parray_compare;
}
