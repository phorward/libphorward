/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	grammar.h
Usage:	Definitions for the grammar parsing module
----------------------------------------------------------------------------- */

typedef struct _pggrammar			pggrammar;
typedef struct _pgproduction		pgprod;
typedef struct _pgsymbol			pgsymbol;
typedef struct _pgtoken				pgtoken;

typedef struct _pgastnode			pgastnode;	/* AST node */
typedef struct _pgast				pgast;

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

typedef enum
{
	PGPARADIGM_UNDEFINED,			/* Undefined */
	/* ~~~ */
	PGPARADIGM_LR0,					/* LR(0) */
	PGPARADIGM_SLR1,				/* SLR(1) */
	PGPARADIGM_LR1,					/* LR(1) */
	PGPARADIGM_LALR1,				/* LALR(1) */

	PGPARADIGM_LL1,					/* LL(1) */
	/* ~~~ */
	PGPARADIGM_EOP					/* End Of Paradigms */
} pgparadigm;

/* Symbol (Terminal, Nonterminal) */
struct _pgsymbol
{
	pggrammar*		grammar;		/* Grammar reference */

	int				id;				/* Symbol ID */
	pgsymtype		type;			/* Symbol type */
	char*			name;			/* Symbol name */

	pboolean		nullable;		/* Nullable-flag (FIRST-set computation) */

	plist*			first;			/* FIRST-set */
	plist*			follow;			/* FOLLOW-set */

	/* Terminal-specific */
	pregex_ptn*		ptn;			/* Regular expression pattern */
	pregex_accept	accept;			/* Regular expression config */

	int				prec;			/* Precedence level */
	pgassoc			assoc;			/* Associativity */

	/* Nonterminal-specific */
	plist*			productions;	/* Productions that belong to a
										nonterminal symbol */

	pboolean		emit;			/* Emit an AST node for this nonterminal */
};

typedef pgsymbol	pgterm;		/* Terminal data type */
typedef pgsymbol	pgnonterm;	/* Non-terminal data type */

/* Production */
struct _pgproduction
{
	pggrammar*		grammar;		/* Grammar reference */

	int				id;				/* Production ID */

	pgsymbol*		lhs;			/* Left-hand side */
	plist*			rhs;			/* Right-hand side */

	int				prec;			/* Precedence level */
	pgassoc			assoc;			/* Associativity */

	plist*			select;			/* SELECT-set */

	char*			strval;			/* DEBUG: String representation
										of the production */
};

/* Grammar */
struct _pggrammar
{
	plist*			symbols;		/* Symbol table */
	plist*			productions;	/* Production table */

	pgnonterm*		goal;			/* Goal non-terminal symbol */
	pgterm*			eoi;			/* End of input terminal symbol */
	pgterm*			error;			/* Error token terminal symbol */

	pregex_ptn*		whitespace;		/* Whitespace pattern */
};

/* Token */
struct _pgtoken
{
	pgsymbol*		symbol;			/* Token ID */
	int				flags;			/* Flag configuration */

	pgvalue*		lexem;			/* Token lexem */

	int				row;			/* Line */
	int				col;			/* Column */
};

/* AST node (also used for syntax tree) */
struct _pgastnode
{
	pgsymbol*		symbol;		/* Symbol */
	pgtoken*		token;		/* Token */

	pgastnode*		parent;		/* Parent node */
	pgastnode*		child;		/* First child node */
	pgastnode*		prev;		/* Previous node in current level */
	pgastnode*		next;		/* Next node in current level */
};

/* AST */
struct _pgast
{
	pggrammar*		grammar;	/* Grammar where AST belongs to */
	pgastnode*		root;		/* AST root node */
};
