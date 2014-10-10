/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ll.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Simple recursive-descent LL parser */

static pboolean pp_ll_PARSE( ppsym* sym, parray* ast, ppgram* grm,
								char* start, char** end )
{
	plistel*	ep;
	plistel*	es;
	ppprod*		p;
	ppsym*		s;
	size_t		istart;
	ppmatch*	mstart;
	ppmatch*	mend;
	char*		ptr;

	if( !( sym && ast && grm && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Skip over whitespace */
	if( pp_white_in_input( grm, start, end ) )
		start = *end;

	/* Terminal input? */
	if( sym->type != PPSYMTYPE_NONTERM )
	{
		if( !pp_sym_in_input( sym, start, end ) )
			return FALSE;

		if( sym->flags & PPFLAG_EMIT )
		{
			mend = (ppmatch*)parray_malloc( ast );

			mend->type = PPMATCH_BEGIN | PPMATCH_END;
			mend->sym = sym;
			mend->emit_id = sym->emit_id;

			mend->start = start;
			mend->end = *end;
		}

		return TRUE;
	}

	istart = parray_count( ast );

	/* Loop trough the productions */
	for( ep = plist_first( sym->prods ); ep; ep = plist_next( ep ) )
	{
		p = (ppprod*)plist_access( ep );
		ptr = start;

		if( p->flags & PPFLAG_LEFTREC )
		{
			TODO;
			fprintf( stderr,
				"Left-recursion is not implemented for LL, skipping\n" );
			continue;
		}

		/* Run the sequence */
		for( es = plist_first( p->rhs ); es; es = plist_next( es ) )
		{
			s = (ppsym*)plist_access( es );

			if( !pp_ll_PARSE( s, ast, grm, ptr, &ptr ) )
				break;
		}

		if( !es )
			break;

		/* Clear partly parsed AST */
		while( parray_count( ast ) > istart )
			parray_pop( ast );
	}

	if( !ep )
		return FALSE;

	*end = ptr;

	if( sym->flags & PPFLAG_EMIT )
	{
		if( istart != parray_count( ast ) )
		{
			mstart = (ppmatch*)parray_insert( ast, istart, (void*)NULL );
			memset( mstart, 0, sizeof( ppmatch ) );
			mstart->type = PPMATCH_BEGIN;

			mend = (ppmatch*)parray_malloc( ast );
			mend->type = PPMATCH_END;
		}
		else
		{
			mend = (ppmatch*)parray_malloc( ast );
			mend->type = PPMATCH_BEGIN | PPMATCH_END;
			mstart = mend;
		}

		mstart->sym = mend->sym = sym;
		mstart->prod = mend->prod = p;
		mstart->emit_id = mend->emit_id = sym->emit_id;

		mstart->start = mend->start = start;
		mstart->end = mend->end = *end;
	}

	return TRUE;
}

/*
static void pp_ll_normalize_leftrec( parray* ast )
{
	size_t		i;
	size_t		j;
	int			cnt;
	ppmatch*	match;
	ppmatch*	pmatch;

	for( i = 0; ( match = (ppmatch*)parray_get( ast, i ) ); i++ )
	{
		if( match->sym->flags & PPFLAG_LEFTREC
				&& match->type == PPMATCH_BEGIN )
		{
			pmatch = (ppmatch*)parray_get( ast, i - 1 );

			if( !( pmatch && pmatch->type == PPMATCH_END
					&& pmatch->start == match->start
						&& pmatch->end < match->end ) )
				continue;

			for( j = i - 1, cnt = 0; j >= 0; j-- )
			{
				pmatch = (ppmatch*)parray_get( ast, j );

				if( !pmatch )
					continue;

				if( pmatch->type == PPMATCH_BEGIN )
					cnt++;

				if( pmatch->type == PPMATCH_END )
					cnt--;

				if( !cnt )
					break;
			}

			if( !pmatch )
				continue;

			parray_insert( ast, j, (void*)NULL );
			parray_put( ast, j, parray_get( ast, i + 1 ) );
			parray_remove( ast, i + 1, (void**)NULL );
		}
	}
}
*/

pboolean pp_ll_parse( parray* ast, ppgram* grm, char* start, char** end )
{
	pboolean	myast	= FALSE;
	pboolean	ret;

	if( !( grm && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !ast )
	{
		ast = parray_create( sizeof( ppmatch ), 0 );
		myast = TRUE;
	}

	ret = pp_ll_PARSE( grm->goal, ast, grm, start, end );

	/*
	if( ast )
		pp_ll_normalize_leftrec( ast );
	*/

	if( myast )
	{
		pp_ast_print( ast );
		parray_free( ast );
	}

	return ret;
}

