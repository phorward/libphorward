/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.h
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

typedef enum
{
	PPSYMTYPE_NONTERM,
	PPSYMTYPE_CCL,
	PPSYMTYPE_STRING,
	PPSYMTYPE_SPECIAL
} ppsymtype;

typedef struct _ppsym	ppsym;
typedef struct _ppprod	ppprod;
typedef struct _ppgram	ppgram;

#define PPFLAG_CALLED	1
#define PPFLAG_DEFINED	2
#define PPFLAG_NULLABLE	4
#define PPFLAG_LEFTREC	8
#define PPFLAG_ASTNODE	16

struct _ppprod
{
	int				id;
	ppsym*			lhs;
	plist*			rhs;

	int				flags;

	char*			strval;
};

struct _ppsym
{
	int				id;
	ppsymtype		type;

	char*			name;
	pboolean		emit;

	int				flags;

	plist*			first;

	pccl*			ccl;
	plist*			productions;
};

struct _ppgram
{
	plist*			symbols;
	plist*			productions;

	ppsym*			ws;
	ppsym*			goal;
	ppsym*			eof;
};

typedef struct
{
	enum
	{
		PPMATCH_BEGIN,
		PPMATCH_END
	} 				type;

	ppsym*			sym;
	ppprod*			prod;

	char*			start;
	char*			end;

	int				line;
	int				col;
} ppmatch;

