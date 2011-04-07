/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: Hash Table Library
Copyright (C) 2007 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>.com

File:	hashtab.h (created on 01.07.2007)
Author:	Jan Max Meyer
Usage:	Hash Table Library / Structures and Definitions
----------------------------------------------------------------------------- */

#ifndef __HASHTAB_H
#define __HASHTAB_H

typedef struct _hashtab 	HASHTAB;
typedef struct _bucket		BUCKET;
typedef struct _hashelem	HASHELEM;

/*
 * Structure definitions
 */
struct _hashtab
{
	int			size;					/* Size of the table
										   	(number of buckets) */
	int			external_keys;			/* Flag, if element keys
										   	are handled internal
											or external */
	BUCKET*		buckets;				/* Array of buckets */
	HASHELEM*	discarded;				/* Discarded elements */
};

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

#endif

