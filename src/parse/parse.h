/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.h
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

typedef struct _ppsym		ppsym;
typedef struct _ppprod		ppprod;
typedef struct _ppgram		ppgram;

/* Flags for grammars and their objects */
#define PPFLAG_CALLED		1
#define PPFLAG_DEFINED		2
#define PPFLAG_NULLABLE		4
#define PPFLAG_LEFTREC		8
#define PPFLAG_LEXEM		16
#define PPFLAG_WHITESPACE	32
#define PPFLAG_PREVENTLREC	64
#define PPFLAG_NAMELESS		128

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

/* Production */
struct _ppprod
{
	int						id;
	ppsym*					lhs;
	plist*					rhs;

	int						flags;
	int						emit;

	/* Debug */
	char*					strval;
};

/* Symbol types */
typedef enum
{
	PPSYMTYPE_NONTERM,
	PPSYMTYPE_CCL,
	PPSYMTYPE_STRING,
	PPSYMTYPE_REGEX,
	PPSYMTYPE_SPECIAL
} ppsymtype;

/* Symbol */
struct _ppsym
{
	int						id;
	ppsymtype				type;

	char*					name;
	int						flags;

	/* Nonterminal specific */
	plist*					first;
	plist*					prods;

	/* Terminal specific */
	pccl*					ccl;
	char*					str;
	pregex*					re;

	/* AST construction */
	int						emit;

	/* Debug */
	char*					strval;
};

/* Grammar */
struct _ppgram
{
	plist*					symbols;
	plist*					prods;

	plist*					ws;
	ppsym*					goal;
	ppsym*					eof;

	int						flags;
};

/* Match (AST entry) */
typedef struct
{
	#define PPMATCH_BEGIN	1
	#define PPMATCH_END		2
	int						type;

	int						emit;
	ppsym*					sym;
	ppprod*					prod;

	char*					start;
	char*					end;

	int						row;
	int						col;
} ppmatch;

/* Parser maintainance object */
typedef struct
{
	int						type;
	ppgram*					gram;

	/*
	plist*					states;
	*/
} pparse;
