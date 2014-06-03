/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.h
Usage:
----------------------------------------------------------------------------- */

typedef struct _pglexer			pglexer;

/* Lexer */
struct _pglexer
{
	pggrammar*		grammar;		/* Grammar (optional) */
	int				flags;			/* Lexer flags */

#define PG_LEXMOD_NONE			0		/* No flags */
#define PG_LEXMOD_SKIP_UNKNOWN	1		/* Skip unknown characters */
#define PG_LEXMOD_UTF8			2		/* UTF-8 character processing */
#define PG_LEXMOD_WCHAR			4		/* Wide-character processing */

	int				states_cnt;		/* DFA state count */
	wchar_t**			states;			/* DFA states */

	/* Input processing */
	int				source;
#define PG_LEX_SRCTYPE_FUNC		0		/* Function: Default: getchar() */
#define PG_LEX_SRCTYPE_STRING		1		/* String */
#define	PG_LEX_SRCTYPE_STREAM		2		/* File by stream (FILE*) */

	union
	{
		char*			str;			/* String */
		FILE*			stream;			/* File stream */
		unsigned int	(*func)();		/* Function */
	} src;							/* Source destination pointer*/

	wchar_t			eof;			/* End of file symbol */
	pboolean		is_eof;			/* Has EOF been reached? */

	/* RUNTIME INFORMATION & BUFFERING */
	int				chsize;

	wchar_t*		bufbeg;			/* Begin of buffer */
	wchar_t*		bufend;			/* End of buffer */
	size_t			bufsiz;			/* Actual buffer size */

	char*			lexem;			/* Lexem value */
	size_t			len;			/* Current lexem length */

#define PLEX_BUFSTEP	1024

	int				line;			/* Current line */
	int				column;			/* Current column */

	pgtoken*		token;			/* Current token */
};
