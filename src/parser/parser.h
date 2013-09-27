/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parser.h
Usage:	
----------------------------------------------------------------------------- */

typedef struct _pgparser			pgparser;
typedef struct _pgtoken				pgtoken;
typedef struct _pgastnode			pgastnode;

/* Token */
struct _pgtoken
{
	pgsymbol*		symbol;			/* Symbol of token */
	char*			token;			/* Token text */
};

/* Parser */
struct _pgparser
{
	pggrammar*		grammar;		/* The grammar of the parser */
	pglexer*		lexer;			/* The lexer */
	pgparadigm		paradigm;		/* Parsing paradigm */

	LIST*			states;			/* The parser states */

	pboolean		optimize;		/* Enable state optimizeion */
	char*			source;			/* Source */
};

/* AST */
struct _pgastnode
{
	pgtoken*		token;			/* Token of node */

	pgastnode*		parent;			/* Parent node */
	pgastnode*		child;			/* First child node */
	pgastnode*		prev;			/* Previous node in current level */
	pgastnode*		next;			/* Next node in current level */
};

