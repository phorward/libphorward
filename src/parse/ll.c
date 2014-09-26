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

/* LL parser with left-recursion support */

static pboolean is_direct_lrec( ppprod* p )
{
	int		i;
	ppsym*	s;

	for( i = 0; ( s = pp_prod_getfromrhs( p, i ) ); i++ )
	{
		if( s == p->lhs )
			return TRUE;

		if( !( s->flags & PPFLAG_NULLABLE ) )
			break;
	}

	return FALSE;
}

static pboolean pp_ll_PARSE( parray* ast, ppgram* grm,
								char* start, char** end, ppsym* sym )
{
	pboolean	myast	= FALSE;
	ppsym*		rsym;
	ppprod*		p;
	char*		ptr;
	int			loop	= 0;
	int			i;
	int			j;
	pboolean	pnext;
	ppmatch*	match;
	ppmatch*	smatch;
	ppmatch*	ematch;
	long		smatch_off;
	long		off;

	if( !( grm && start && end && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !ast )
	{
		ast = parray_create( sizeof( ppmatch ), 0 );
		myast = TRUE;
	}

	do
	{
		printf( "loop = %d\n", loop );

		smatch = (ppmatch*)parray_malloc( ast );
		smatch->type = PPMATCH_BEGIN;
		smatch_off = parray_count( ast ) - 1;

		for( i = 0, pnext = TRUE;
				pnext && ( p = (ppprod*)plist_access(
									plist_get( sym->prods, i ) ) ); i++ )
		{

			printf( "Trying %s\n", pp_prod_to_str( p ) );

			if( !( ( loop == 0 && !is_direct_lrec( p ) )
						|| ( loop > 0 && is_direct_lrec( p ) ) ) )
				continue;

			for( j = 0, ptr = start;
					( rsym = pp_prod_getfromrhs( p, j ) ); j++ )
			{
				printf( "Testing >%s<\n", ptr );

				if( rsym->type == PPSYMTYPE_NONTERM )
				{
					for( off = smatch_off - 1;
							( match = (ppmatch*)parray_get( ast, off ) );
								off-- )
					{
						if( match->start < ptr )
						{
							off = -1L;
							break;
						}

						if( match->type == PPMATCH_END
								&& match->sym == rsym )
						{
							ptr = match->end;
							break;
						}
					}

					if( off < 0L && !pp_ll_PARSE( ast, grm, ptr, &ptr, rsym ) )
						break;
				}
				else if( !pp_sym_in_input( rsym, ptr, &ptr ) )
					break;
			}

			if( !rsym )
				break;

			while( smatch_off + 1 < parray_count( ast ) )
				parray_pop( ast );
		}

		if( !( pnext && p ) )
		{
			parray_remove( ast, smatch_off, (void**)NULL );
			break;
		}

		ematch = (ppmatch*)parray_malloc( ast );
		ematch->type = PPMATCH_END;
		smatch->start = ematch->start = start;
		smatch->end = ematch->end = ptr;
		smatch->sym = ematch->sym = sym;
		smatch->prod = ematch->prod = p;

		*end = ptr;
		loop++;
	}
	while( sym->flags & PPFLAG_LEFTREC );

	if( myast )
		parray_free( ast );

	return loop ? TRUE : FALSE;
}

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

	ret = pp_ll_PARSE( ast, grm, start, end, grm->goal );

	if( ast )
		pp_ll_normalize_leftrec( ast );

	if( myast )
	{
		pp_ast_print( ast );
		parray_free( ast );
	}

	return ret;
}

