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
	pgproduction*	prod;			/* The associated production
										to this item */
	int				dot_offset;		/* The dot's offset from the left
										of the right hand side */
	pgsymbol*		next_symbol;	/* Symbol following the dot */
	LIST*			lookahead;		/* Set of lookahead-symbols */
} pgitem;

/* LALR(1) State */
typedef struct
{
	int				state_id;		/* State ID */
	LIST*			kernel;			/* Kernel item set */
	LIST*			epsilon;		/* Epsilon item set */

	LIST*			actions;		/* Action table entries */
	LIST*			gotos;			/* Goto table entries */

	pgproduction*	def_prod;		/* Default production */

	pboolean		done;			/* Done flag */
	pboolean		closed;			/* Closed flag */
} pgstate;

/* Parser actions */
#define ERROR					0	/* Force parse error */
#define REDUCE					1 	/* Reduce 			0 1 */
#define SHIFT					2 	/* Shift 			1 0 */
#define SHIFT_REDUCE			3 	/* Shift-Reduce 	1 1 */
