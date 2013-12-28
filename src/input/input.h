/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	input.h
Usage:	Phorward Input Adapter
----------------------------------------------------------------------------- */

typedef struct _pia				pia;

typedef enum
{
	PIATYPE_NONE,				/* Nothing */
	PIATYPE_STREAM,				/* File by stream (FILE*) */
	PIATYPE_STR,				/* String */
	PIATYPE_FUNC				/* Function: Default: getchar() */
} piatype;

struct _pia
{
	piatype				type;	/* Input source type */

	unsigned int		eof;	/* End of input symbol */
	unsigned int		ch;		/* End of input symbol */

	unsigned int*		bufbeg;	/* Begin of buffer */
	unsigned int*		bufend;	/* End of buffer */
	size_t				bufsiz;	/* Current buffer size */
#define PIA_BUFSIZ		1024

	int					flags;	/* Flags */

#define PIA_MOD_NONE	0		/* For sake of completeness. */
#define PIA_MOD_UTF8	1		/* Input is UTF-8 */
#define PIA_MOD_WCHAR	2		/* Wide-character input */
#define PIA_MOD_RELEASE	4		/* Close input / free pointer */
#define PIA_MOD_IS_EOF	8		/* Has EOF been entered? */

	union
	{
		char*			str;		/* String */
		FILE*			file;		/* File stream */
		unsigned int	(*func)();	/* Function */
	} src;						/* Source destination pointer*/

	char*				ptr;	/* Current pointer (string only) */
};
