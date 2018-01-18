/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
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
#define PPFLAG_SPECIAL		1024

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

/* Symbol (both terminal and nonterminal for easier access in productions) */
struct _ppsym
{
	/* Primaries */
	int						id;
	char*					name;
#define PPSYM_T_EOF			"&eof"

	int						flags;

	ppgram*					grm;

	plist*					first;		/* FIRST sets */

	/* AST construction */
	char*					emit;

	/* Debug */
	char*					strval;
};

#define PPSYM_IS_TERMINAL( sym )	( !islower( *( sym )->name ) )
#define PPSYM_IS_NONTERMINAL( sym )	!PPSYM_IS_TERMINAL( sym )

/* Grammar */
struct _ppgram
{
	int						sym_id;		/* Symbol ID */
	plist*					symbols;	/* Nonterminals and Terminals */

	int						prod_id;	/* Production ID */
	plist*					prods;		/* Productions */

	ppsym*					goal;		/* The start/goal symbol */
	ppsym*					eof;		/* End-of-input symbol */

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
	ppgram*					gram;
	plex*					lex;

} pparse;

