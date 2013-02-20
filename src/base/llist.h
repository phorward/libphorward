/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	llist.h
Usage:	Universal management functions for dynamic linked pointer list
----------------------------------------------------------------------------- */

#ifndef LLIST_H
#define LLIST_H

/* Structures */
typedef struct llist
{
	void*			pptr;
	struct llist*	next;
} LIST;

/* Macros */
#define list_access( ll )		( (ll) ? (ll)->pptr : (void*)NULL )
#define list_next( ll )			( (ll) ? (ll)->next : (LIST*)NULL )
#define list_replace( ll, ptr )	( (ll) ? (ll)->pptr = (ptr) : 0 )

#define LISTFOR( ll, cc )		for( (cc) = (ll); (cc); (cc) = list_next(cc) )

#endif

