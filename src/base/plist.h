/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See \LICENSE for more information.

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
typedef struct Pelem		plistelem;
typedef struct Plist		plist;

typedef	pboolean			(*plistelem_fn)( pbyte* e );
#define PELEM_FN_NULL		( (plistelem_fn)NULL )

/* Element */
struct Pelem
{
	uchar*					key;

	plist*					list;

	plistelem*				prev;
	plistelem*				next;

	plistelem*				hashnext;
	plistelem*				hashprev;
};

/* Container */
struct Plist
{
	int						flags;
	int						size;
	int						count;
	int						hashsize;
	
	plistelem_fn			free_fn;

	plistelem*				unused;

	plistelem*				first;
	plistelem*				last;
	plistelem**				hash;
};

/* Macros */
#define plist_size( l )		( ( l ) ? ( l )->size : (psize)0 )
#define plist_count( l )	( ( l ) ? ( l )->count : (psize)0 )
#define plist_first( l )	( ( l ) ? ( l )->first : (plistelem*)NULL )
#define plist_last( l )		( ( l ) ? ( l )->last : (plistelem*)NULL )
#define plist_access( e )	( (pbyte*)( ( e ) + 1 ) )
#define plist_next( e )		( ( e ) ? ( e )->next : (plistelem*)NULL )
#define plist_prev( e )		( ( e ) ? ( e )->prev : (plistelem*)NULL )

#endif /* PLIST_H */
