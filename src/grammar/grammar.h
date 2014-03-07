/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	grammar.h
Usage:	Definitions for the grammar parsing module
----------------------------------------------------------------------------- */

typedef struct _pggrammar			pggrammar;
typedef struct _pgproduction		pgproduction;
typedef struct _pgsymbol			pgsymbol;
typedef struct _pgtoken				pgtoken;

typedef struct _pgasttype			pgasttype;	/* AST node type */
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

	pgasttype*		asttype;		/* Associated AST type */

	/* Nonterminal-specific */
	plist*			productions;	/* Productions that belong to a
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
	plist*			rhs;			/* Right-hand side */

	int				prec;			/* Precedence level */
	pgassoc			assoc;			/* Associativity */

	plist*			select;			/* SELECT-set */

	char*			strval;			/* String representation */

	pgasttype*		asttype;		/* Associated AST type */
};

/* Grammar */
struct _pggrammar
{
	plist*			symbols;		/* Symbol table */
	plist*			productions;	/* Production table */

	pgnonterminal*	goal;			/* Goal non-terminal symbol */
	pgterminal*		eoi;			/* End of input terminal symbol */
	pgterminal*		error;			/* Error token terminal symbol */

	pregex_ptn*		whitespace;		/* Whitespace pattern */

	plist*			asttypes;		/* AST node types */
};

/* Token */
struct _pgtoken
{
	pgsymbol*		symbol;			/* Token ID */
	int				flags;			/* Flag configuration */
#define PG_TOKFLAG_NONE		0			/* No current config */
#define PG_TOKFLAG_ALLOC	1			/* Allocated content in lexem */
#define PG_TOKFLAG_ZEROTERM	2			/* Zero terminated content in lexem */
#define PG_TOKFLAG_WCHAR	4			/* Wide-character in lexem */

	char*			lexem;			/* Token lexem */

	int				row;			/* Line */
	int				col;			/* Column */
};

/* AST traversal function */
typedef void						(*pgastfn)( pgastnode* astnode );
#define PG_ASTFUNC( name )			void name( pgastnode* astnode )

/* AST node type */
struct _pgasttype
{
	char*			name;		/* Type name */
	size_t			size;		/* User type size */

	pgastfn			topdown;	/* Top-down callback */
	pgastfn			passover;	/* Passover callback */
	pgastfn			bottomup;	/* Bottom-up callback */
};

/* AST node (also used for syntax tree) */
struct _pgastnode
{
	pgasttype*		type;		/* Node type */

	pgsymbol*		symbol;		/* Symbol of node */
	pgtoken*		token;		/* Token of node */

	plist*			attributes;	/* Attributes */

	pgastnode*		parent;		/* Parent node */
	pgastnode*		child;		/* First child node */
	pgastnode*		prev;		/* Previous node in current level */
	pgastnode*		next;		/* Next node in current level */
};

/* AST creation mode */
typedef enum
{
	PGASTMODE_NONE,				/* For sake of completeness */
	PGASTMODE_SYNTAX,			/* Syntax tree */
	PGASTMODE_AST				/* Abstract syntax tree */
} pgastmode;

/* AST */
struct _pgast
{
	pggrammar*		grammar;	/* Grammar where AST belongs to */
	pgastmode		mode;		/* AST creation mode */
	pgastnode*		root;		/* AST root node */
};
