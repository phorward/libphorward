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
#define PLIST_MOD_NONE		0
#define PLIST_MOD_RECYCLE	1
#define PLIST_MOD_EXTKEYS	2
#define PLIST_MOD_UNIQUE	4
#define PLIST_MOD_WCHAR		8

/* Typedefs */
typedef struct Pelem		punit;
typedef struct Plist		plist;

typedef	pboolean			(*punit_fn)( pbyte* e );
#define PELEM_FN_NULL		( (punit_fn)NULL )

/* Element */
struct Pelem
{
	char*					key;

	plist*					list;

	punit*					prev;
	punit*					next;

	punit*					hashnext;
	punit*					hashprev;
};

/* Container */
struct Plist
{
	int						flags;
	int						size;
	int						count;
	int						hashsize;

	punit_fn				destruct_fn;

	punit*					unused;

	punit*					first;
	punit*					last;
	punit**					hash;
};

#endif /* PLIST_H */
