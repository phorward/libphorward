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
	PIATYPE_FILE,				/* File via file pointer */
	PIATYPE_STR,				/* String */
	PIATYPE_FUNC				/* Function: Default: getchar() */
} piatype;

struct _pia
{
	piatype				type;	/* Input source type */

	unsigned int		eof;	/* End of input symbol */
	unsigned int		ch;		/* Current input character */
	char*				ptr;	/* Current pointer (string only) */

	int					flags;	/* Flags */

#define PIA_MOD_NONE	0		/* For sake of completeness. */
#define PIA_MOD_UTF8	1		/* Input is UTF-8 */
#define PIA_MOD_WCHAR	2		/* Wide-character input */
#define PIA_MOD_RELEASE	4		/* Close input / Free pointer */
#define PIA_MOD_IS_EOF	8		/* Has EOF been entered? */

	union
	{
		char*			str;		/* String */
		FILE*			file;		/* File stream */
		int				(*func)();	/* Function */
	} src;						/* Source destination pointer*/
};
