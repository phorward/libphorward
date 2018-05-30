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
#define PPFLAG_NONE			0x00
#define PPFLAG_CALLED		0x01
#define PPFLAG_DEFINED		0x02
#define PPFLAG_NULLABLE		0x04
#define PPFLAG_LEFTREC		0x08
#define PPFLAG_LEXEM		0x10
#define PPFLAG_WHITESPACE	0x20
#define PPFLAG_PREVENTLREC	0x40
#define PPFLAG_NAMELESS		0x80
#define PPFLAG_GENERATED	0x100
#define PPFLAG_FREENAME		0x200
#define PPFLAG_FREEEMIT		0x400
#define PPFLAG_SPECIAL		0x800
#define PPFLAG_FINALIZED	0x1000
#define PPFLAG_FROZEN		0x2000

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

#define PPLR_SHIFT			1
#define PPLR_REDUCE			2

/* Associativity */
typedef enum
{
	PPASSOC_NONE,
	PPASSOC_NOT,
	PPASSOC_LEFT,
	PPASSOC_RIGHT
} ppassoc;

/* Production */
struct _ppprod
{
	ppgram*					grm;		/* Grammar */

	unsigned int			idx;		/* Production index */
	ppsym*					lhs;		/* Left-hand side */
	plist*					rhs;		/* Left-hand side items */
	unsigned int			flags;		/* Configuration flags */

	ppassoc					assoc;		/* LR associativity */
	unsigned int			prec;		/* LR precedence level */

	char*					emit;		/* AST emitting node */

	char*					strval;		/* String represenation */
};

/* Symbol (both terminal and nonterminal for easier access in productions) */
struct _ppsym
{
	ppgram*					grm;		/* Grammar */

	unsigned int			idx;		/* Symbol index */
	char*					name;		/* Unique name */
#ifndef PPSYM_T_EOF
#define PPSYM_T_EOF			"&eof"
#endif

	unsigned int			flags;		/* Configuration flags */

	ppassoc					assoc;		/* LR associativity */
	unsigned int			prec;		/* LR precedence level */

	plist*					first;		/* Set of FIRST() symbols */

	char*					emit;		/* AST emitting node */
	pregex_ptn*				ptn;		/* Pattern definition (terminals!) */

	char*					strval;		/* String representation */
};

#ifndef PPSYM_IS_TERMINAL
#define PPSYM_IS_TERMINAL( sym )	( !sym->name || !( *sym->name ) \
										|| !islower( *( sym )->name ) )
#endif

/* Grammar */
struct _ppgram
{
	plist*					symbols;	/* Symbols (both nonterminals
														and terminals) */
	plist*					prods;		/* Productions */

	ppsym*					goal;		/* The start/goal symbol */
	ppsym*					eof;		/* End-of-input symbol */

	unsigned int			flags;		/* Configuration flags */

	char*					strval;		/* String representation */
};

/* AST */
struct _ppast
{
	char*					emit;		/* AST node name */

	ppsym*					sym;		/* Emitting symbol */
	ppprod*					prod;		/* Emitting production */

	/* Semantics */
	pany*					val;		/* Value */

	/* Match */
	char*					start;		/* Begin of fragment */
	char*					end;		/* End of fragment */
	size_t					len;		/* Fragment length */

	/* Source */
	unsigned long			row;		/* Appearance in row */
	unsigned long			col;		/* Appearance in column */

	/* AST */
	ppast*					child;		/* First child element */
	ppast*					prev;		/* Previous element in current scope */
	ppast*					next;		/* Next element in current scope */
};

/* AST traversal */
typedef enum
{
	PPAST_EVAL_TOPDOWN,
	PPAST_EVAL_PASSOVER,
	PPAST_EVAL_BOTTOMUP
} ppasteval;

typedef void (*pastevalfn)( ppasteval type, ppast* node );

/* Parser states */
typedef enum
{
	PPPAR_STATE_INITIAL,
	PPPAR_STATE_DONE,
	PPPAR_STATE_NEXT,
	PPPAR_STATE_ERROR
} ppparstate;

/* Parser */
typedef struct
{
	/* Grammar */
	ppgram*					gram;		/* Grammar */

	/* Parser */
	unsigned int			states;		/* States count */
	unsigned int**			dfa;		/* Parse table */

} pppar;

/* Parser context */
typedef struct
{
	pppar*					par;		/* Parser */

	ppparstate				state;		/* State */
	int						reduce;		/* Reduce */
	parray					stack;		/* Stack */
	ppast*					ast;		/* AST */
} ppparctx;

/* Macro: PP_GRAM_DUMP */
#ifdef DEBUG
	#define PP_GRAM_DUMP( g ) \
		_dbg_gram_dump( __FILE__, __LINE__, _dbg_proc_name, #g, g )
#else
	#define PP_GRAM_DUMP( g )
#endif

/* Macro: PP_AST_DUMP */
#ifdef DEBUG
	#define PP_AST_DUMP( ast ) \
		_dbg_ast_dump( __FILE__, __LINE__, _dbg_proc_name, #ast, ast )
#else
	#define PP_AST_DUMP( ast )
#endif
