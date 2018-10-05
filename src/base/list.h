/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	list.h
Usage:	An improved, double linked, optionally hashed list collection object.
----------------------------------------------------------------------------- */

#ifndef PLIST_H
#define PLIST_H

/* Typedefs */
typedef struct Plist		plist;			/* List */
typedef struct Plistel		plistel;		/* List element */
typedef void (*plistelfn)	( plistel* );	/* List element callback */
typedef void (*plistfn)		( void* );		/* List element access callback */

/* Element */
struct Plistel
{
	char*					key;
	plist*					list;

	plistel*				prev;
	plistel*				next;

	plistel*				hashnext;
	plistel*				hashprev;
};

/* Container */
struct Plist
{
	int						flags;
#define PLIST_MOD_NONE		0	/* No modification (for sake of completeness) */
#define PLIST_MOD_PTR		1	/* Pointer mode: Store only pointers
									into list elements */
#define PLIST_MOD_RECYCLE	2	/* Recycle removed elements for later
									usage */
#define PLIST_MOD_AUTOSORT	4	/* Automatically sort elements */
#define PLIST_MOD_EXTKEYS	8	/* Store keys as pointer, no copying */
#define PLIST_MOD_PTRKEYS	16	/* Keys are directly used and compared
									(pointer value) */
#define PLIST_MOD_UNIQUE	32	/* Unique keys, no collisions */
#define PLIST_MOD_WCHAR		64	/* Use wide-character strings for keys */

	size_t					size;
	long					count;
	int						hashsize;
#define PLIST_DFT_HASHSIZE	61

	int						size_index;

	int						(*comparefn)( plist*, plistel*, plistel* );
	int						(*sortfn)( plist*, plistel*, plistel* );
	void					(*printfn)( plist* );

	plistel*				unused;

	plistel*				first;
	plistel*				last;
	plistel**				hash;

	/* statistics */
	int						load_factor;
	int						free_hash_entries;
	int						hash_collisions;
	long					recycled;
};

/* Macros */

/** Macro that expands into a for-loop iterating a plist-object //l// using
the element variable //e// as walker.

This macro expands into a for-loop of the format

``` for( e = plist_first( l ); e; e = plist_next( e ) )

where //e// is a walker variable for each element.

**Example:**

```
plist*		my_list;
plistel*	e;

plist_for( my_list, e )
{
	\* Access current element data part *\
	ptr = (cast*)plist_access( e );
	...
}
```
*/
/*MACRO:plist_for( plist* l, plistel* e )*/
#define plist_for( l, e )	\
	for( (e) = plist_first( l ); (e); (e) = plist_next( e ) )

#endif /* PLIST_H */
