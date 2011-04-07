/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	utf8.h
Author:	Jan Max Meyer
Usage:	Macros and utilties for UTF-8 string handling
----------------------------------------------------------------------------- */

#ifndef UTF8_H
#define UTF8_H

/* Macros */
#define u8_isutf(c) (((c)&0xC0)!=0x80)

#endif
