/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	local.h
Usage:	Local definitions
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Closure item */
typedef struct
{
	pgproduction*	prod;			/* Production */
	int				dot;			/* Dot offset */
	LIST*			lookahead;		/* Lookahead symbols */
} pglritem;

/* LR-State */
typedef struct
{
	LIST*			kernel;			/* Kernel items */

	LIST*			actions;		/* Action row entries */
	LIST*			gotos;			/* Goto row entries */

	pgproduction*	def_prod;		/* Default production */

	pboolean		done;			/* Done flag */
	pboolean		closed;			/* Closed flag */
} pglrstate;

/* LR-Transition */
typedef struct
{
	pgsymbol*		symbol;			/* Symbol */

	pglrstate*		shift;			/* Shift to state */
	pgproduction*	reduce;			/* Reduce by production */
} pglrcolumn;
