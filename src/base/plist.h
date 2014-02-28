/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	plist.h
Usage:	An improved, double linked, optionally hashed list collection object.
----------------------------------------------------------------------------- */

#ifndef PLIST_H
#define PLIST_H

/* Defines */
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



/* Typedefs */
typedef struct Plistel		plistel;
typedef struct Plist		plist;

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
	int						size;
	int						count;
	int						hashsize;

	int						(*comparefn)( plist*, plistel*, plistel* );
	int						(*sortfn)( plist*, plistel*, plistel* );

	plistel*				unused;

	plistel*				first;
	plistel*				last;
	plistel**				hash;
};

/* Macros */

/** Macro that expands into a for-loop trough a plist-object //l// using
element walker variable //e//.

This macro expands into a for-loop of the format

``` for( e = plist_first( l ); e; e = plist_next( e ) )

where //e// is a walker variable for each element.

**Example:**

```
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
