/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: String Object Library
Copyright (C) 2010, 2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

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
	pchar*		str;		/* String data content */
} _pstring;

typedef _pstring*	pstring;

/* Prototypes */
#include "pstring.proto.h"

#endif

