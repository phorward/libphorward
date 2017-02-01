/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	prod.c
Usage:	Defining and handling grammar productions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Creates a new production on left-hand-side //lhs//
	within the grammar //g//. */
ppprod* pp_prod_create( ppgram* g, ppsym* lhs, ... )
{
	ppprod*	prod;
	ppsym*	sym;
	va_list	varg;

	if( !( g && lhs && lhs->type == PPSYMTYPE_NONTERM ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	prod = (ppprod*)plist_malloc( g->prods );

	prod->id = -1;
	prod->grm = g;

	prod->lhs = lhs;
	prod->rhs = plist_create( 0, PLIST_MOD_PTR );

	plist_push( lhs->prods, prod );

	va_start( varg, lhs );

	while( ( sym = va_arg( varg, ppsym* ) ) )
		pp_prod_append( prod, sym );

	va_end( varg );

	return prod;
}

/** Frees the production object //p// and releases any used memory. */
ppprod* pp_prod_drop( ppprod* p )
{
	if( !p )
		return (ppprod*)NULL;

	if( p->flags & PPFLAG_FREEEMIT )
		pfree( p->emit );

	pfree( p->strval );

	plist_free( p->rhs );

	plist_remove( p->lhs->prods, plist_get_by_ptr( p->lhs->prods, p ) );
	plist_remove( p->grm->prods, plist_get_by_ptr( p->grm->prods, p ) );

	return (ppprod*)NULL;
}

/** Get the //n//th production from grammar //g//.
Returns (ppprod*)NULL if no symbol was found. */
ppprod* pp_prod_get( ppgram* g, int n )
{
	if( !( g && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	return (ppprod*)plist_access( plist_get( g->prods, n ) );
}

/** Appends the symbol //sym// to the right-hand-side of production //p//. */
pboolean pp_prod_append( ppprod* p, ppsym* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	plist_push( p->rhs, sym );
	pfree( p->strval );

	return TRUE;
}

/** Removes all occurences of symbol //sym// from the right-hand-side of
	production //p//. */
int pp_prod_remove( ppprod* p, ppsym* sym )
{
	int			cnt 	= 0;
	plistel*	e;

	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	while( ( e = plist_get_by_ptr( p->rhs, sym ) ) )
	{
		plist_remove( p->rhs, e );
		cnt++;
	}

	return cnt;
}

/** Returns the //off//s element from the right-hand-side of
	production //p//. Returns (ppsym*)NULL if the requested element does
	not exist. */
ppsym* pp_prod_getfromrhs( ppprod* p, int off )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get( p->rhs, off ) );
}

/** Returns the string representation of production //p//.

	The returned pointer is part of //p// and can be referenced multiple times.
	It may not be freed by the caller. */
char* pp_prod_to_str( ppprod* p )
{
	plistel*	e;
	ppsym*		sym;

	if( !p )
	{
		WRONGPARAM;
		return "";
	}

	if( !p->strval )
	{
		if( p->lhs )
			p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );

		p->strval = pstrcatstr( p->strval, " : ", FALSE );
		plist_for( p->rhs, e )
		{
			sym = (ppsym*)plist_access( e );

			if( e != plist_first( p->rhs ) )
				p->strval = pstrcatstr( p->strval, " ", FALSE );

			if( sym->type != PPSYMTYPE_NONTERM )
				p->strval = pstrcatstr( p->strval, "@", FALSE );

			p->strval = pstrcatstr( p->strval, sym->name, FALSE );
		}
	}

	return p->strval;
}
