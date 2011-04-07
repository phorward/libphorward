/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: Linked Pointer Lists
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

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

/* Defines for old-style function names */
#define listPush				list_push
#define listPop					list_pop
#define listRemove				list_remove
#define listFree				list_free
#define listPrint				list_print
#define listDup					list_dup
#define listFind				list_find
#define listGetPtrByPosition	list_getptr
#define listDiff				list_diff
#define listUnion				list_union
#define listCount				list_count
#define listSubset				list_subset

/* Macros */
#define list_access( ll )		( (ll) ? (ll)->pptr : (void*)NULL )
#define list_next( ll )			( (ll) ? (ll)->next : (LIST*)NULL )
#define list_replace( ll, ptr )	( (ll) ? (ll)->pptr = (ptr) : 0 )

#define LISTFOR( ll, cc )		for( (cc) = (ll); (cc); (cc) = list_next(cc) )

#endif

