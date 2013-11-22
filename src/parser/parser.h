/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parser.h
Usage:
----------------------------------------------------------------------------- */

typedef struct _pgparser			pgparser;
typedef struct _pgastnode			pgastnode;

/* Parser */
struct _pgparser
{
	pgparadigm		paradigm;	/* Parser paradigm config */

	pggrammar*		grammar;	/* The grammar */
	pglexer*		lexer;		/* The lexer */

	plist*			states;		/* The parser states */

	pboolean		optimize;	/* Enable state optimizeion */
	char*			source;		/* Source */
};

/* Token */
struct _pgtoken
{
	int			id;			/* Symbol match ID */
	pgsymbol*	symbol;		/* Symbol terminal ID */
	char*		token;		/* Token pointer */
	int			len;		/* Token length */

	int			row;		/* Line */
	int			col;		/* Column */
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

