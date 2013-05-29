/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	local.h
Usage:	Local definitions
----------------------------------------------------------------------------- */

#include <phorward.h>

/* Closure item */
typedef struct
{
	pgproduction*	prod;			/* Production */
	int				dot;			/* Dot offset */
	LIST*			lookahead;		/* Lookahead symbols */
} pglritem;

/* LALR(1) State */
typedef struct
{
	LIST*			kernel;			/* Kernel items */
	LIST*			epsilon;		/* Epsilon items */

	LIST*			actions;		/* Action table entries */
	LIST*			gotos;			/* Goto table entries */

	pgproduction*	def_prod;		/* Default production */

	pboolean		done;			/* Done flag */
	pboolean		closed;			/* Closed flag */
} pglrstate;

/* Parser actions */
#define ERROR					0	/* Force parse error */
#define REDUCE					1 	/* Reduce 			0 1 */
#define SHIFT					2 	/* Shift 			1 0 */
#define SHIFT_REDUCE			3 	/* Shift & Reduce 	1 1 */
