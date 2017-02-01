/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ccl.h
Author:	Jan Max Meyer
Usage:	Implements functions for Unicode-enabled character-class handling.
----------------------------------------------------------------------------- */

/* Character class */
#define PCCL_MIN			0x0
#if UNICODE
#define PCCL_MAX			0xFFFF
#else
#define PCCL_MAX			0xFF
#endif

typedef struct
{
	wchar_t			begin;
	wchar_t			end;
} pcrange;

typedef struct
{
	int				min;
	int				max;

	plist*			ranges;

	char*			str;
} pccl;

