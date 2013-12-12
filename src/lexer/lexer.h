/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
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

	int			srctype;		/* Source type flag */
#define PGLEXER_SRCTYPE_FUNC	0	/* Function: Default: getchar() */
#define PGLEXER_SRCTYPE_STRING	1	/* String */
#define PGLEXER_SRCTYPE_FILE	2	/* File via file pointer */
#define PGLEXER_SRCTYPE_FD		3	/* File via file descriptor */

	union
	{
		int		(*func)();			/* Function */
		char*	str;				/* String */
		FILE*	file;				/* File stream */
		int		fd;					/* File descriptor */
	} src;						/* Source  destination pointer*/
};
