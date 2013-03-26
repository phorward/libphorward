/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.c
Usage:	LR/LALR/SLR parse table computation
----------------------------------------------------------------------------- */

#include "local.h"

static pglrstate* pg_lrstate_create( pgparser* parser )
{
	pglrstate*	state;

	if( !( parser ) )
	{
		WRONGPARAM;
		return (pglrstate*)NULL;
	}

	state = pmalloc( sizeof( pglrstate ) );
	parser->states = list_push( parser->states, state );

	return state;
}

static BOOLEAN pg_parser_lr_same_kernel( LIST* kernel1, LIST* kernel2 )
{
	LIST*		l;
	LIST*		m;
	pglritem*	it1;
	pglritem*	it2;
	int			same	= 0;

	if( list_count( kernel1 ) == list_count( kernel2 ) )
	{
		LISTFOR( kernel1, l )
		{
			it1 = (pglritem*)list_access( l );

			LISTFOR( kernel2, m )
			{
				it2 = (pglritem*)list_access( m );

				if( it1->prod == it2->prod
					&& it1->dot == it2->dot )
				{
					same++;
				}
			}
		}

		if( list_count( kernel1 ) == same )
			return TRUE;
	}

	return FALSE;
}

static void pg_lritem_print( pglritem* it, FILE* f )
{
	int			i;
	pgsymbol*	sym;

	if( ( !it ) )
	{
		WRONGPARAM;
		return;
	}

	if( !f )
		f = stderr;

	pg_symbol_print( it->prod->lhs, f );
	fprintf( f, " : " );

	for( i = 0; ( sym = pg_production_get_rhs( it->prod, i ) ); i++ )
	{
		if( i > 0 )
			fprintf( f, " " );

		if( i == it->dot )
			fprintf( f, ". " );

		pg_symbol_print( sym, f );
	}

	if( i == it->dot )
		fprintf( f, " ." );

	fprintf( f, "\n" );
}

static void pg_lritems_print( LIST* items, FILE* f, uchar* what )
{
	LIST*	l;

	if( ( !items ) )
		return;

	if( !f )
		f = stderr;

	if( what && *what )
		fprintf( f, "%s:\n", what );

	LISTFOR( items, l )
	{
		if( what && *what )
			fprintf( f, "\t" );

		pg_lritem_print( (pglritem*)list_access( l ), f );
	}
}

static pglritem* pg_lritem_create( LIST** list, pgproduction* prod, int dot )
{
	pglritem*	item;

	if( !( prod ) )
	{
		WRONGPARAM;
		return (pglritem*)NULL;
	}

	if( dot < 0 )
		dot = 0;
	else if( dot > pg_production_get_rhs_length( prod ) )
		dot = pg_production_get_rhs_length( prod );

	item = pmalloc( sizeof( pglritem ) );
	item->prod = prod;
	item->dot = dot;

	if( list )
		*list = list_push( *list, item );

	return item;
}

static pglritem* pg_lritem_free( pglritem* it )
{
	if( !( it ) )
		return (pglritem*)NULL;

	pfree( it );

	return (pglritem*)NULL;
}

static pglrstate* pg_parser_lr_get_undone( pgparser* parser )
{
	pglrstate*	st;
	LIST*		l;

	LISTFOR( parser->states, l )
	{
		st = (pglrstate*)list_access( l );
		if( !st->done )
			return st;
	}

	return (pglrstate*)NULL;
}

BOOLEAN pg_parser_lr_closure( pgparser* parser )
{
	pglrstate*		st;
	pglritem*		it;
	pglritem*		kit;
	pgsymbol*		sym;
	pgproduction*	prod;
	LIST*			closure;
	LIST*			start;
	LIST*			l;
	LIST*			m;
	int				i;
	int				j;
	int				cnt;
	int				prev_cnt;

	if( !( parser ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !( parser->paradigm == PGPARADIGM_LALR1
			|| parser->paradigm == PGPARADIGM_LR0
				|| parser->paradigm == PGPARADIGM_LR1
					|| parser->paradigm == PGPARADIGM_SLR1 ) )
	{
		PGERR( "Wrong paradigm to function" );
		return FALSE;
	}

	st = pg_lrstate_create( parser );
	it = pg_lritem_create( &st->kernel,
				pg_production_get_by_lhs(
					pg_grammar_get_goal( parser->grammar ), 0 ), 0 );

	while( ( st = pg_parser_lr_get_undone( parser ) ) )
	{
		fprintf( stderr, "---\nClosing state %d\n",
					list_find( parser->states, st ) );
		st->done = TRUE;

		/* Close all items of the current state */
		cnt = 0;
		closure = (LIST*)NULL;

		pg_lritems_print( st->kernel, stderr, "Kernel" );

		LISTFOR( st->kernel, l )
		{
			kit = (pglritem*)list_access( l );
			it = pg_lritem_create( &closure, kit->prod, kit->dot );
		}

		do
		{
			prev_cnt = cnt;
			cnt = 0;

			/* Loop trought all items of the current state */
			LISTFOR( closure, l )
			{
				it = (pglritem*)list_access( l );

				/* Check if symbol right to the dot is a nonterminal */
				if( !( sym = pg_production_get_rhs( it->prod, it->dot ) )
						|| !pg_symbol_is_nonterminal( sym ) )
					continue;

				/* Add all productions of the nonterminal to the closure,
					if not already in */
				for( i = 0; ( prod = pg_production_get_by_lhs( sym, i ) ); i++ )
				{
					LISTFOR( closure, m )
					{
						it = (pglritem*)list_access( m );
						if( it->prod == prod )
							break;
					}

					if( !m )
					{
						it = pg_lritem_create( &closure, prod, 0 );
					}

					/* TODO: LALR lookahead */
				}
			}
			cnt = list_count( closure );
		}
		while( prev_cnt != cnt );

		/* Add current kernel to closure */
		pg_lritems_print( closure, stderr, "Closure" );

		/* Create new states from the items in the closure having a nonterminal
			right to their dot */
		do
		{
			sym = (pgsymbol*)NULL;
			start = (LIST*)NULL;

			LISTFOR( closure, l )
			{
				it = (pglritem*)list_access( l );

				/* Check if symbol right to the dot is a nonterminal */
				if( !sym && !( sym = pg_production_get_rhs(
								it->prod, it->dot ) ) )
					continue;

				/* Add item to new state kernel */
				if( pg_production_get_rhs( it->prod, it->dot ) == sym )
				{
					it->dot++;
					start = list_push( start, it );
				}
			}

			/* Check for existing state */
			if( start )
			{
				/* Remove new kernel from closure */
				LISTFOR( start, l )
					closure = list_remove( closure, list_access( l ) );

				/* Check if state already exists */
				LISTFOR( parser->states, l )
				{
					st = (pglrstate*)list_access( l );

					if( pg_parser_lr_same_kernel( st->kernel, start ) )
						break;
				}

				if( !l )
				{
					fprintf( stderr, "Creating new state %d\n",
										list_count( parser->states ) );
					pg_lritems_print( start, stderr, "Kernel" );

					st = pg_lrstate_create( parser );
					st->kernel = start;
				}
				else
				{
					fprintf( stderr, "Using existing state %d\n",
								list_find( parser->states, st ) );
					pg_lritems_print( start, stderr, "Kernel" );

					LISTFOR( start, l )
						pg_lritem_free( (pglritem*)list_access( l ) );

					list_free( start );
				}
			}
		}
		while( sym );

		/*
		getchar();
		*/
	}

	fprintf( stderr, "\n*** FINAL STATES***\n\n" );

	LISTFOR( parser->states, l )
	{
		st = (pglrstate*)list_access( l );
		fprintf( stderr, "-- State %d --\n",
			list_find( parser->states, st ) );

		pg_lritems_print( st->kernel, stderr, "Kernel" );
	}

	return TRUE;
}
