/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.h
Usage:
----------------------------------------------------------------------------- */

typedef struct _pgtoken			pgtoken;
typedef struct _pglexer			pglexer;

/* Token */
struct _pgtoken
{
	int				id;			/* Symbol match ID */
	pgsymbol*		symbol;		/* Symbol terminal ID */
	char*			token;		/* Token pointer */
	int				len;		/* Token length */

	int				row;		/* Line */
	int				col;		/* Column */
};

/* Lexer */
struct _pglexer
{
	pggrammar*		grammar;		/* Grammar (optional) */

	int				states_cnt;		/* DFA state count */
	int**			states;			/* DFA states */

	unsigned int	eof;			/* End of file symbol */
	pboolean		ignore_unknown;	/* Ignore any unknown characters */
};
