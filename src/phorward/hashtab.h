/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: Base Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	hashtab.h
Author:	Jan Max Meyer
Usage:	Hash Table Library / Structures and Definitions
----------------------------------------------------------------------------- */

#ifndef __HASHTAB_H
#define __HASHTAB_H

/*
 * Hash table modifiers
 */
#define HASHTAB_MOD_NONE		0	/* No modifiers */
#define HASHTAB_MOD_EXTKEYS		1	/* External keys */
#define HASHTAB_MOD_WCHAR		2	/* Wide-character keys */
#define HASHTAB_MOD_LIST		4	/* All table elements are additionally
 										hold in a linked list, to keep the
										insertation order of the hash table
										elements */
#define HASHTAB_MOD_NO_COLL		8	/* Disallow collisions of hash table
										elements; When inserted, (HASHELEM*)
										NULL will be returned! */

#define HASHTAB_CALLBACK		void(*) ( void* )
#define HASHTAB_NO_CALLBACK		( (HASHTAB_CALLBACK) NULL )

/*
 * Type definitions
 */
typedef struct _hashtab 	HASHTAB;
typedef struct _bucket		BUCKET;
typedef struct _hashelem	HASHELEM;

/*
 * Structure definitions
 */
struct _hashtab
{
	int			flags;					/* Flags, that are associated
										   	with the individual hash-table */
	int			size;					/* Size of the table
										   	(number of buckets) */
	BUCKET*		buckets;				/* Array of buckets */
	HASHELEM*	discarded;				/* Discarded elements */
	LIST*		list;					/* Hash element adjustment
											list (HASHTAB_MOD_LIST only) */
};

#define hashtab_list( ht )				(ht)->list

struct _bucket
{
	HASHELEM*	start;					/* Pointer to start item */
	HASHELEM*	end;					/* Pointer to end item */
};

struct _hashelem
{
	uchar*		key;					/* Identifying key of the entry */
	void*		data;					/* Pointer to additional data/object */
	HASHELEM*	prev;					/* Pointer to previous item in
										   	the bucket */
	HASHELEM*	next;					/* Pointer to next item in the bucket */
};

#define hashelem_access( hl )			(hl)->data
#define hashelem_key( hl )				(hl)->key
#define hashelem_prev( hl )				(hl)->prev
#define hashelem_next( hl )				(hl)->next

#endif

