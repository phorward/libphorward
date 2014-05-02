/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.h
Usage:	LR parsing definitions
----------------------------------------------------------------------------- */

/* Closure item */
typedef struct
{
	pgprod*	prod;			/* Production */
	int				dot;			/* Dot offset */
	plist*			lookahead;		/* Lookahead symbols */
} pglritem;

/* LR-State */
typedef struct
{
	plist*			kernel;			/* Kernel items */

	plist*			actions;		/* Action row entries */
	plist*			gotos;			/* Goto row entries */

	pgprod*	def_prod;		/* Default production */

	pboolean		done;			/* Done flag */
	pboolean		closed;			/* Closed flag */
} pglrstate;

/* LR-Transition */
typedef struct
{
	pgsymbol*		symbol;			/* Symbol */

	pglrstate*		shift;			/* Shift to state */
	pgprod*	reduce;			/* Reduce by production */
} pglrcolumn;
