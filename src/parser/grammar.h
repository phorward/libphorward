/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	grammar.h
Usage:	Definitions for the grammar parsing module
----------------------------------------------------------------------------- */

typedef struct _pggrammar			pggrammar;
typedef struct _pgproduction		pgproduction;
typedef struct _pgsymbol			pgsymbol;

typedef enum
{
	PGSYMTYPE_UNDEFINED,			/* Undefined symbol */
	PGSYMTYPE_TERMINAL,				/* Terminal symbol */
	PGSYMTYPE_NONTERMINAL			/* Nonterminal symbol */
} pgsymtype;

typedef enum
{
	PGASSOC_NONE,					/* No/Undefined association */
	PGASSOC_LEFT,					/* Left associative */
	PGASSOC_RIGHT,					/* Right associative */
	PGASSOC_NOASSOC					/* Not associative */
} pgassoc;

/* Symbol (Terminal, Nonterminal) */
struct _pgsymbol
{
	pggrammar*		grammar;		/* Grammar reference */

	int				id;				/* Symbol ID */
	pgsymtype		type;			/* Symbol type */
	uchar*			name;			/* Symbol name */

	LIST*			first;			/* FIRST sets */

	/* Terminal-specific */
	pregex_ptn*		ptn;			/* Regular expression pattern */
	int				prec;			/* Precedence level */
	pgassoc			assoc;			/* Associativity */

	/* Nonterminal-specific */
	LIST*			productions;	/* Productions that belong to a
										nonterminal symbol */
};

typedef pgsymbol	pgterminal;		/* Terminal data type */
typedef pgsymbol	pgnonterminal;	/* Non-terminal data type */

/* Production */
struct _pgproduction
{
	pggrammar*		grammar;		/* Grammar reference */

	int				id;				/* Production ID */

	pgsymbol*		lhs;			/* Left-hand side */
	LIST*			rhs;			/* Right-hand side */

	int				prec;			/* Precedence level */
	pgassoc			assoc;			/* Associativity */

	uchar*			strval;			/* String representation */
};

/* Grammar */
struct _pggrammar
{
	plist*			symbols;		/* Symbol table */
	LIST*			productions;	/* Production table */

	pregex			lexer;			/* Lexical analyzer */

	pgnonterminal*	goal;			/* Goal non-terminal symbol */
	pgterminal*		end_of_input;	/* End of input terminal symbol */
	pgterminal*		error;			/* Error token terminal symbol */
};

