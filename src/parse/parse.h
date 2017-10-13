/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.h
Usage:	Phorward parsing library
----------------------------------------------------------------------------- */

typedef struct _ppsym		ppsym;
typedef struct _ppprod		ppprod;
typedef struct _ppgram		ppgram;
typedef struct _ppast		ppast;

/* Flags for grammars and their objects */
#define PPFLAG_NONE			0
#define PPFLAG_CALLED		1
#define PPFLAG_DEFINED		2
#define PPFLAG_NULLABLE		4
#define PPFLAG_LEFTREC		8
#define PPFLAG_LEXEM		16
#define PPFLAG_WHITESPACE	32
#define PPFLAG_PREVENTLREC	64
#define PPFLAG_NAMELESS		128
#define PPFLAG_GENERATED	256
#define PPFLAG_FREEEMIT		512

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

/* Production */
struct _ppprod
{
	/* Elementary */
	int						id;
	ppsym*					lhs;
	plist*					rhs;
	int						flags;
	ppgram*					grm;

	/* AST construction */
	char*					emit;

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
	PPSYMTYPE_FUNCTION,
	PPSYMTYPE_SPECIAL
} ppsymtype;

/* Symbol function */
typedef pboolean (*ppsymfunc)( char* start, char** end );

/* Symbol */
struct _ppsym
{
	/* Primaries */
	int						id;
	ppsymtype				type;
	char*					name;
	int						flags;
	ppgram*					grm;

	/* Nonterminal specific */
	plist*					first;
	plist*					prods;

	/* Terminal specific */
	char*					def;		/* Definition */
	pregex_ptn*				ptn;		/* Pattern */

	pccl*					ccl;		/* Character class */
	char*					str;		/* String */
	pregex*					re;			/* Regular expression */
	ppsymfunc				sf;			/* Symbol function */

	/* AST construction */
	char*					emit;

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

	plex*					lex;
	ppsymfunc 				(*getsymfunc)( char* name );

	int						flags;
};

/* AST */
struct _ppast
{
	/* Emit */
	char*					emit;

	/* Grammar */
	ppsym*					sym;
	ppprod*					prod;

	/* Match */
	char*					start;
	char*					end;
	size_t					length;

	/* Source */
	int						row;
	int						col;

	/* AST */
	ppast*					child;
	ppast*					prev;
	ppast*					next;
};

/* Parser runtime object */
typedef struct
{
	int						type;
	ppgram*					gram;
} pparse;

