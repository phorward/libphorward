/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.gen.c
Usage:	LR/LALR/SLR parse table computation
----------------------------------------------------------------------------- */

#include "local.h"

static void pg_lritem_print( pglritem* it, FILE* f )
{
	int			i;
	pgsymbol*	sym;
	LIST*		l;

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
	{
		fprintf( f, " ." );

		if( it->lookahead )
		{
			fprintf( f, "   [ " );
			LISTFOR( it->lookahead, l )
			{
				if( l != it->lookahead )
					fprintf( f, " " );

				pg_symbol_print( (pgsymbol*)list_access( l ), stderr );
			}

			fprintf( f, " ]" );
		}
	}

	fprintf( f, "\n" );
}

static void pg_lritems_print( LIST* items, FILE* f, char* what )
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

	list_free( it->lookahead );
	pfree( it );

	return (pglritem*)NULL;
}

static pglrcolumn* pg_lrcolumn_create( pglrstate* st, short action,
										pgsymbol* sym, void* ptr )
{
	pglrcolumn*		col;

	col = (pglrcolumn*)pmalloc( sizeof( pglrcolumn ) );

	col->action = action;
	col->symbol = sym;

	if( action & REDUCE )
		col->target.production = (pgproduction*)ptr;
	else
		col->target.state = (pglrstate*)ptr;

	if( pg_symbol_is_terminal( sym ) )
		st->actions = list_push( st->actions, col );
	else
		st->gotos = list_push( st->gotos, col );

	return col;
}


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

static pglrstate* pg_lrstate_free( pglrstate* state )
{
	LIST*		l;
	pglrcolumn*	col;
	pglritem*	it;

	if( !state )
		return (pglrstate*)NULL;

	LISTFOR( state->actions, l )
	{
		col = (pglrcolumn*)list_access( l );
		pfree( col );
	}

	LISTFOR( state->gotos, l )
	{
		col = (pglrcolumn*)list_access( l );
		pfree( col );
	}

	LISTFOR( state->kernel, l )
	{
		it = (pglritem*)list_access( l );
		pfree( col );
	}

	state->actions = list_free( state->actions );
	state->gotos = list_free( state->gotos );
	state->kernel = list_free( state->kernel );

	pfree( state );

	return (pglrstate*)NULL;
}

static BOOLEAN pg_parser_lr_compare(
		LIST* kernel1, LIST* kernel2, pgparadigm para )
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
					&& it1->dot == it2->dot
					&& ( para != PGPARADIGM_LR1
							|| list_diff( it1->lookahead, it2->lookahead ) )
					)
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
	pglrstate*		nst;
	pglritem*		it;
	pglritem*		kit;
	pglritem*		cit;
	pgsymbol*		sym;
	pgsymbol*		lhs;
	pgproduction*	prod;
	pglrcolumn*		col;
	LIST*			closure;
	LIST*			start;
	LIST*			l;
	LIST*			m;
	LIST*			n;
	LIST*			o;
	int				i;
	int				j;
	int				cnt;
	int				prev_cnt;

	if( !( parser ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !pg_parser_is_lr( parser ) )
	{
		PGERR( "Wrong paradigm to function" );
		return FALSE;
	}

	/* Reset LR parse tables */
	pg_parser_lr_reset( parser );

	/* Perform FIRST set computation */
	pg_grammar_compute_first( pg_parser_get_grammar( parser ) );

	/* Create a starting seed */
	nst = pg_lrstate_create( parser );
	it = pg_lritem_create( &nst->kernel,
				pg_production_get_by_lhs(
					pg_grammar_get_goal( parser->grammar ), 0 ), 0 );

	it->lookahead = list_push( it->lookahead,
						pg_grammar_get_eoi( parser->grammar ) );

	while( ( st = pg_parser_lr_get_undone( parser ) ) )
	{
		fprintf( stderr, "---\nClosing state %d\n",
					list_find( parser->states, st ) );
		st->done = TRUE;

		/* Close all items of the current state */
		cnt = 0;
		closure = (LIST*)NULL;

		pg_lritems_print( st->kernel, stderr, "Kernel" );

		/* Duplicate state kernel to closure */
		LISTFOR( st->kernel, l )
		{
			kit = (pglritem*)list_access( l );
			it = pg_lritem_create( &closure, kit->prod, kit->dot );
			it->lookahead = list_dup( kit->lookahead );
		}

		/* Close the closure! */
		do
		{
			prev_cnt = cnt;
			cnt = 0;

			/* Loop trought all items of the current state */
			LISTFOR( closure, l )
			{
				it = (pglritem*)list_access( l );

				/* Check if symbol right to the dot is a nonterminal */
				if( !( lhs = pg_production_get_rhs( it->prod, it->dot ) )
						|| !pg_symbol_is_nonterminal( lhs ) )
					continue;

				/* Add all productions of the nonterminal to the closure,
					if not already in */
				for( i = 0; ( prod = pg_production_get_by_lhs( lhs, i ) ); i++ )
				{
					LISTFOR( closure, m )
					{
						cit = (pglritem*)list_access( m );
						if( cit->prod == prod )
							break;
					}

					if( !m )
						cit = pg_lritem_create( &closure, prod, 0 );

					/* Merge lookahead */
					if( parser->paradigm != PGPARADIGM_LR0 )
					{
						/*
							Find out all lookahead symbols by merging the
							FIRST-sets of all nullable and the first
							non-nullable items on the productions right-hand
							side.
						*/
						for( j = it->dot + 1;
								( sym = pg_production_get_rhs( it->prod, j ) );
									j++ )
						{
							cit->lookahead = list_union( cit->lookahead,
															sym->first );

							if( !sym->nullable )
								break;
						}

						/*
							If all symbols right to the dot are nullable
							(or there are simply none), then add the current
							items lookahead to the closed items lookahead.
						*/
						if( !sym )
							cit->lookahead = list_union( cit->lookahead,
															it->lookahead );
					}
				}
			}

			cnt = list_count( closure );
		}
		while( prev_cnt != cnt );

		/* Add current kernel to closure */
		pg_lritems_print( closure, stderr, "Closure" );

		/* Create new states from the items in the closure having a symbol
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

			/* Stop if no more partitions found! */
			if( !start )
				break;

			/* Remove this partition from closure */
			LISTFOR( start, l )
				closure = list_remove( closure, list_access( l ) );

			/*
				Can we do a shift and reduce in one transition?

				Watch for partitions that are

					x -> y .z

				where x is nonterminal, y is a possible sequence of
				terminals and/or nonterminals or even epsilon, and z is a
				terminal or nonterminal.
			*/
			if( pg_parser_get_optimize( parser ) &&
					( list_count( start ) == 1 &&
						!pg_production_get_rhs( it->prod, it->dot ) ) )
			{
				if( !st->closed )
					pg_lrcolumn_create( st, SHIFT_REDUCE, sym, it->prod );

				/* Forget current partition
					- its not needed anymore... */
				LISTFOR( start, l )
					pg_lritem_free( (pglritem*)list_access( l ) );

				list_free( start );
				continue;
			}

			/* Check if state already exists */
			LISTFOR( parser->states, l )
			{
				nst = (pglrstate*)list_access( l );

				if( pg_parser_lr_compare(
						nst->kernel, start, parser->paradigm ) )
					break;
			}

			/* State does not already exists?
				Create it as new! */
			if( !l )
			{
				fprintf( stderr, "Creating new state %d\n",
									list_count( parser->states ) );
				pg_lritems_print( start, stderr, "Kernel" );

				nst = pg_lrstate_create( parser );
				nst->kernel = start;
			}
			else
			/* State already exists?
				Merge lookaheads (if needed). */
			{
				fprintf( stderr, "Using existing state %d\n",
							list_find( parser->states, nst ) );

				if( parser->paradigm != PGPARADIGM_LR0 )
				{
					cnt = 0;
					prev_cnt = 0;

					for( l = nst->kernel, m = start; l;
							l = list_next( l ), m = list_next( m ) )
					{
						it = (pglritem*)list_access( l );
						cit = (pglritem*)list_access( l );

						prev_cnt += list_count( it->lookahead );
						it->lookahead = list_union( it->lookahead,
														cit->lookahead );
						cnt += list_count( it->lookahead );
					}

					if( cnt != prev_cnt )
						nst->done = FALSE;
				}

				pg_lritems_print( st->kernel, stderr, "Kernel" );

				LISTFOR( start, l )
					pg_lritem_free( (pglritem*)list_access( l ) );

				list_free( start );
			}

			if( sym && !st->closed )
				pg_lrcolumn_create( st, SHIFT, sym, nst );
		}
		while( TRUE );

		/*
		getchar();
		*/
		st->closed = TRUE;
	}

	/* Perform reductions */
	LISTFOR( parser->states, l )
	{
		st = (pglrstate*)list_access( l );

		LISTFOR( st->kernel, m )
		{
			it = (pglritem*)list_access( m );

			if( pg_production_get_rhs( it->prod, it->dot ) )
				continue;

			LISTFOR( it->lookahead, n )
			{
				sym = (pgsymbol*)list_access( n );

				LISTFOR( st->actions, o )
				{
					col = (pglrcolumn*)list_access( o );
					if( col->symbol == sym )
						break;
				}

				if( !o )
					pg_lrcolumn_create( st, REDUCE, sym, it->prod );
				else
					fprintf( stderr,
						"Conflict %d on %s, now %s\n",
							col->action, pg_symbol_get_name( col->symbol ),
								pg_symbol_get_name( sym ) );
			}
		}
	}

	fprintf( stderr, "\n*** FINAL STATES***\n\n" );

	LISTFOR( parser->states, l )
	{
		st = (pglrstate*)list_access( l );
		fprintf( stderr, "-- State %d --\n",
			list_find( parser->states, st ) );

		pg_lritems_print( st->kernel, stderr, "Kernel" );

		LISTFOR( st->actions, m )
		{
			col = (pglrcolumn*)list_access( m );

			if( col->action == SHIFT_REDUCE )
				fprintf( stderr, "\t<- Shift/Reduce on '%s' by "
									"production '%s'\n",
							pg_symbol_get_name( col->symbol ),
								pg_production_to_string(
									col->target.production ) );
			else if( col->action == SHIFT )
				fprintf( stderr, "\t-> Shift on '%s' to state %d\n",
							pg_symbol_get_name( col->symbol ),
								list_find( parser->states,
									col->target.state ) );
			else
				fprintf( stderr, "\t<- Reduce on '%s' by production '%s'\n",
							pg_symbol_get_name( col->symbol ),
								pg_production_to_string(
									col->target.production ) );
		}

		LISTFOR( st->gotos, m )
		{
			col = (pglrcolumn*)list_access( m );

			fprintf( stderr, "\t-> Goto state state %d on symbol '%s'\n",
							list_find( parser->states, col->target.state ),
									pg_symbol_get_name( col->symbol ) );
		}
	}

	return TRUE;
}

BOOLEAN pg_parser_lr_reset( pgparser* parser )
{
	LIST*		l;
	pglrstate*	st;

	if( !( parser ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !pg_parser_is_lr( parser ) )
	{
		PGERR( "Wrong paradigm to function" );
		return FALSE;
	}

	LISTFOR( parser->states, l )
	{
		st = (pglrstate*)list_access( l );
		pg_lrstate_free( st );
	}

	return TRUE;
}
