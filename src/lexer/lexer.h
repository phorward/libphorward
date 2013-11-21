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
	pregex_ptn*	whitespace;		/* Whitespace */

	pregex_nfa*	nfa;			/* NFA state machine */
	pregex_dfa*	dfa;			/* DFA state machine */

	int			flags;			/* Lexer flags */
#define PGLEXER_MOD_NONE		0
#define PGLEXER_MOD_UTF8		1	/* UTF-8 character mode */
#define PGLEXER_MOD_WCHAR		2	/* Wide-character mode */

	plist*		tokens;			/* Tokens array */
	int			fetchlimit;		/* Token fetch limit */

	int			source;			/* Source flags */
#define PGLEXER_SRC_FUNCTION	0	/* Function mode (default=getchar()) */
#define PGLEXER_SRC_STRING		1	/* String */
#define PGLEXER_SRC_FILE		2	/* File via file pointer */
#define PGLEXER_SRC_FD			3	/* File via file descriptor */

	union
	{
		int		(*func)();		/* Function */
		char*	str;			/* String */
		FILE*	file;			/* File stream */
		int		fd;				/* File descriptor */
	} stream;					/* Input stream */
};
