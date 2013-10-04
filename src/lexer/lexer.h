/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.h
Usage:
----------------------------------------------------------------------------- */

typedef struct _pglexer				pglexer;

/* Lexer */
struct _pglexer
{
	plist		symbols;		/* Pattern definitions of the symbols */
	pregex_dfa	dfa;			/* DFA state machine */

	plist		tokens;			/* Tokens array */
};

