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
#define PPFLAG_FREENAME		512
#define PPFLAG_FREEEMIT		1024
#define PPFLAG_SPECIAL		2048
#define PPFLAG_FINALIZED	4096

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

#define PPLR_SHIFT			1
#define PPLR_REDUCE			2

/* Production */
struct _ppprod
{
	ppgram*					grm;		/* Grammar */

	unsigned int			idx;		/* Production index */
	ppsym*					lhs;		/* Left-hand side */
	plist*					rhs;		/* Left-hand side items */
	unsigned int			flags;		/* Configuration flags */

	char*					emit;		/* AST emitting node */

	char*					strval;		/* String represenation */
};

/* Symbol (both terminal and nonterminal for easier access in productions) */
struct _ppsym
{
	ppgram*					grm;		/* Grammar */

	unsigned int			idx;		/* Symbol index */
	char*					name;		/* Unique name */
#define PPSYM_T_EOF			"&eof"

	unsigned int			flags;		/* Configuration flags */

	plist*					first;		/* Set of FIRST() symbols */

	char*					emit;		/* AST emitting node */

	char*					strval;		/* String represenation */
};

#define PPSYM_IS_TERMINAL( sym )	( !islower( *( sym )->name ) )
#define PPSYM_IS_NONTERMINAL( sym )	!PPSYM_IS_TERMINAL( sym )

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

/* Parser */
typedef struct
{
	/* Grammar */
	ppgram*					gram;		/* Grammar */

	/* Parser */
	int						states;		/* States count */
	unsigned int**			dfa;		/* Parse table */

	/* Lexical analyzer */
	plex*					lex;		/* plex object */
	unsigned int			(*lexfn)( char** start, char** end ); /* callback */

} pparser;
