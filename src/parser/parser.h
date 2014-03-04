/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parser.h
Usage:
----------------------------------------------------------------------------- */

typedef struct _pgparser		pgparser;

typedef enum
{
	PGTREEMODE_NONE,
	PGTREEMODE_SYNTAX,
	PGTREEMODE_AST
} pgtreemode;

/* Parser */
struct _pgparser
{
	pgparadigm		paradigm;	/* Parser paradigm config */

	pggrammar*		grammar;	/* The grammar */
	pglexer*		lexer;		/* The lexer */

	plist*			states;		/* The parser states */

	pgtreemode		treemode;	/* Tree creation mode */

	pboolean		optimize;	/* Enable state optimizeion */
	char*			source;		/* Source */
};

