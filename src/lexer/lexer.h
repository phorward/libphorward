/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.h
Usage:
----------------------------------------------------------------------------- */

typedef struct _pgtoken				pgtoken;
typedef struct _pglexer				pglexer;

/* Token */
struct _pgtoken
{
	int			id;			/* Symbol match ID */
	pgsymbol*	symbol;		/* Symbol terminal ID (optional) */
	char*		token;		/* Token pointer */
	int			len;		/* Token length */
};

/* Lexer */
struct _pglexer
{
	pggrammar*	grammar;		/* Grammar (optional) */

	pregex_nfa*	nfa;			/* NFA state machine */
	pregex_dfa*	dfa;			/* DFA state machine */

	plist*		tokens;			/* Tokens array */
};
