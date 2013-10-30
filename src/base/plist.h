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
#define PLIST_MOD_RECYCLE	1	/* Recycle removed elements for later
									usage */
#define PLIST_MOD_EXTKEYS	2	/* Store keys as pointer, no copying */
#define PLIST_MOD_UNIQUE	4	/* Unique keys, no collisions */
#define PLIST_MOD_WCHAR		8	/* Wide-character keys */
#define PLIST_MOD_PTR		16	/* Pointer mode: Store only pointers */

/* Typedefs */
typedef struct Plistel		plistel;
typedef struct Plist		plist;

typedef	pboolean			(*plistel_fn)( pbyte* e );
#define PELEM_FN_NULL		( (plistel_fn)NULL )

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

	plistel_fn				destruct_fn;

	plistel*				unused;

	plistel*				first;
	plistel*				last;
	plistel**				hash;
};

/* Macros */
#define plist_for( l, e )	\
	for( (e) = plist_first( l ); (e); (e) = plist_next( e ) )

#endif /* PLIST_H */
