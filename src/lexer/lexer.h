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
	int				flags;			/* Lexer flags */

#define PLEX_MOD_NONE			0		/* No flags */
#define PLEX_MOD_SKIP_UNKNOWN	1		/* Skip unknown characters */
#define PLEX_MOD_UTF8			2		/* UTF-8 character processing */
#define PLEX_MOD_WCHAR			4		/* Wide-character processing */

	int				states_cnt;		/* DFA state count */
	pchar**			states;			/* DFA states */

	/* Input processing */
	int				source;
#define PLEX_SRCTYPE_FUNC		0		/* Function: Default: getchar() */
#define PLEX_SRCTYPE_STRING		1		/* String */
#define	PLEX_SRCTYPE_STREAM		2		/* File by stream (FILE*) */

	union
	{
		char*			str;			/* String */
		FILE*			stream;			/* File stream */
		unsigned int	(*func)();		/* Function */
	} src;							/* Source destination pointer*/

	pchar			eof;			/* End of file symbol */
	pboolean		is_eof;			/* Has EOF been reached? */

	/* RUNTIME INFORMATION & BUFFERING */
	int				chsize;

	pchar*			bufbeg;			/* Begin of buffer */
	pchar*			bufend;			/* End of buffer */
	size_t			bufsiz;			/* Actual buffer size */

	char*			lexem;			/* Lexem value */
	size_t			lexem_siz;		/* Actual lexem buffer size */

#define PLEX_BUFSTEP	1024

	int				line;			/* Current line */
	int				column;			/* Current column */
};
