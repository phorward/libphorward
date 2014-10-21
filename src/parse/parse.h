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
	PPSYMTYPE_REGEX,
	PPSYMTYPE_SPECIAL
} ppsymtype;

typedef struct _ppsym		ppsym;
typedef struct _ppprod		ppprod;
typedef struct _ppgram		ppgram;

#define PPFLAG_CALLED		1
#define PPFLAG_DEFINED		2
#define PPFLAG_NULLABLE		4
#define PPFLAG_LEFTREC		8
#define PPFLAG_EMIT			16
#define PPFLAG_LEXEM		32
#define PPFLAG_WHITESPACE	64
#define PPFLAG_PREVENTLREC	128

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

struct _ppprod
{
	int						id;
	ppsym*					lhs;
	plist*					rhs;

	int						flags;

	/* Debug */
	char*					strval;
};

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
	pregex_ptn*				ptn;
	pregex_nfa*				nfa;

	/* AST construction */
	int						emit_id;

	/* Debug */
	char*					strval;
};

struct _ppgram
{
	plist*					symbols;
	plist*					prods;

	plist*					ws;
	ppsym*					goal;
	ppsym*					eof;

	int						flags;
};

typedef struct
{
	#define PPMATCH_BEGIN	1
	#define PPMATCH_END		2
	int						type;

	int						emit_id;
	ppsym*					sym;
	ppprod*					prod;

	char*					start;
	char*					end;

	int						line;
	int						col;
} ppmatch;

