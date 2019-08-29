/* -HEADER----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
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
	wchar_t			begin;		/* Character range begin */
	wchar_t			end;		/* Character range end */
} pcrange;

typedef struct
{
	wchar_t			min;		/* Character universe start */
	wchar_t			max;		/* Character universe end */

	parray			ranges;		/* Ranges */

	char*			str;		/* String representation */
} pccl;

