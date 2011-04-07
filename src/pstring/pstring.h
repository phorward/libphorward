/* -HEADER----------------------------------------------------------------------
Phorward String Object Library
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:	pstring.h
Author:	Jan Max Meyer
Usage:	Header for Phorward string object functions & features
----------------------------------------------------------------------------- */

#ifndef PSTRING_H
#define PSTRING_H

/* pstring - Phorward string object */
typedef struct
{
	psize		len;		/* Length of the string object's data content */
	pchar*		str;		/* String data content (pchar) */
	pbyte*		tmp;		/* Pointer to store uchar*-conversion results,
								for garbage collection */
} pstring;

typedef pstring*	Pstring;

/* Prototypes */
#ifndef MAKE_PROTOTYPES
#include "pstring.proto.h"
#endif

/* General get/set macros */
#define pstring_set			pstring_set_pchar
#define pstring_get			pstring_get_pchar

#endif

