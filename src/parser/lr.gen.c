/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.gen.c
Usage:	LR/LALR/SLR parse table computation
----------------------------------------------------------------------------- */

#include "phorward.h"
#include "lr.h"

static void pg_lritem_print( pglritem* it, FILE* f )
{
	int			i;
	pgsymbol*	sym;
	plistel*	e;

	if( ( !it ) )
	{
		WRONGPARAM;
		return;
	}

	if( !f )
		f = stderr;

	pg_symbol_print( it->prod->lhs, f );
	fprintf( f, " : " );

	for( i = 0; ( sym = pg_prod_get_rhs( it->prod, i ) ); i++ )
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

			plist_for( it->lookahead, e )
			{
				if( e != plist_first( it->lookahead ) )
					fprintf( f, " " );

				pg_symbol_print( (pgsymbol*)plist_access( e ), stderr );
			}

			fprintf( f, " ]" );
		}
	}

	fprintf( f, "\n" );
}

static void pg_lritems_print( plist* items, FILE* f, char* what )
{
	plistel*	e;

	if( ( !items ) )
		return;

	if( !f )
		f = stderr;

	if( what && *what )
		fprintf( f, "%s:\n", what );

	plist_for( items, e )
	{
		if( what && *what )
			fprintf( f, "\t" );

		pg_lritem_print( (pglritem*)plist_access( e ), f );
	}
}

static pglritem* pg_lritem_create( plist* list, pgprod* prod, int dot )
{
	pglritem*	item;

	if( !( list && prod ) )
	{
		WRONGPARAM;
		return (pglritem*)NULL;
	}

	if( dot < 0 )
		dot = 0;
	else if( dot > pg_prod_get_rhs_length( prod ) )
		dot = pg_prod_get_rhs_length( prod );

	item = (pglritem*)plist_malloc( list );
	item->prod = prod;
	item->dot = dot;
	item->lookahead = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_AUTOSORT );

	return item;
}

static pglritem* pg_lritem_free( pglritem* it )
{
	if( !( it ) )
		return (pglritem*)NULL;

	it->lookahead = plist_free( it->lookahead );

	return (pglritem*)NULL;
}

static pglrcolumn* pg_lrcolumn_create(
	pglrstate* st, pgsymbol* sym, pglrstate* shift, pgprod* reduce )
{
	pglrcolumn*		col;

	if( pg_symbol_is_term( sym ) )
		col = (pglrcolumn*)plist_malloc( st->actions );
	else
		col = (pglrcolumn*)plist_malloc( st->gotos );

	col->symbol = sym;
	col->shift = shift;
	col->reduce = reduce;

	return col;
}

static pglrstate* pg_lrstate_create( pgparser* parser, plist* kernel )
{
	plistel*	e;
	pglrstate*	state;

	if( !( parser ) )
	{
		WRONGPARAM;
		return (pglrstate*)NULL;
	}

	state = plist_malloc( parser->states );

	state->kernel = plist_create( sizeof( pglritem ), PLIST_MOD_NONE );
	state->actions = plist_create( sizeof( pglrcolumn ), PLIST_MOD_NONE );
	state->gotos = plist_create( sizeof( pglrcolumn ), PLIST_MOD_NONE );

	plist_for( kernel, e )
		plist_push( state->kernel, plist_access( e ) );

	return state;
}

static pglrstate* pg_lrstate_free( pglrstate* state )
{
	if( !state )
		return (pglrstate*)NULL;

	plist_free( state->actions );
	plist_free( state->gotos );
	plist_free( state->kernel );

	pfree( state );

	return (pglrstate*)NULL;
}

static BOOLEAN pg_parser_lr_compare( plist* set1, plist* set2, pgparadigm para )
{
	plistel*	e;
	plistel*	f;
	pglritem*	it1;
	pglritem*	it2;
	int			same	= 0;

	if( plist_count( set1 ) == plist_count( set2 ) )
	{
		plist_for( set1, e )
		{
			it1 = (pglritem*)plist_access( e );

			plist_for( set2, f )
			{
				it2 = (pglritem*)plist_access( f );

				if( it1->prod == it2->prod
					&& it1->dot == it2->dot )
				{
					if( para != PGPARADIGM_LR1
							|| !plist_diff( it1->lookahead, it2->lookahead ) )
						same++;
				}
			}
		}

		if( plist_count( set1 ) == same )
			return TRUE;
	}

	return FALSE;
}

static pglrstate* pg_parser_lr_get_undone( pgparser* parser )
{
	pglrstate*	st;
	plistel*	e;

	plist_for( parser->states, e )
	{
		st = (pglrstate*)plist_access( e );

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
	pgprod*	prod;
	pglrcolumn*		col;
	plist*			closure;
	plist*			part;
	plistel*		e;
	plistel*		f;
	plistel*		g;
	plistel*		h;
	int				i;
	int				j;
	int				cnt;
	int				prev_cnt;

	PROC( "pg_parser_lr_closure" );
	PARMS( "parser", "%p", parser );

	if( !( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !pg_parser_is_lr( parser ) )
	{
		PGERR( pg_parser_get_grammar( parser ),
				__FILE__, __LINE__, "Wrong paradigm to function" );
		RETURN( FALSE );
	}

	MSG( "Resetting LR parse tables" );
	pg_parser_lr_reset( parser );

	MSG( "Performing FIRST set computation" );
	pg_grammar_compute_first( pg_parser_get_grammar( parser ) );
	pg_grammar_print( pg_parser_get_grammar( parser ) );

	MSG( "Creating a closure seed" );
	nst = pg_lrstate_create( parser, (plist*)NULL );
	it = pg_lritem_create( nst->kernel,
				pg_prod_get_by_lhs(
					pg_grammar_get_goal( parser->grammar ), 0 ), 0 );

	plist_push( it->lookahead, pg_grammar_get_eoi( parser->grammar ) );

	MSG( "Initializing part and closure lists" );
	part = plist_create( sizeof( pglritem ), PLIST_MOD_RECYCLE );
	closure = plist_create( sizeof( pglritem ), PLIST_MOD_RECYCLE );

	MSG( "Run the closure loop" );
	while( ( st = pg_parser_lr_get_undone( parser ) ) )
	{
		fprintf( stderr, "---\nClosing state %d\n",
					plist_offset( plist_get_by_ptr( parser->states, st ) ) );
		st->done = TRUE;

		MSG( "Closing state" );
		VARS( "State", "%d", plist_offset(
								plist_get_by_ptr( parser->states, st ) ) );

		/* Close all items of the current state */
		cnt = 0;
		plist_clear( closure );

		pg_lritems_print( st->kernel, stderr, "Kernel" );

		/* Duplicate state kernel to closure */
		MSG( "Duplicate current kernel to closure" );
		plist_for( st->kernel, e )
		{
			kit = (pglritem*)plist_access( e );
			it = pg_lritem_create( closure, kit->prod, kit->dot );
			it->lookahead = plist_dup( kit->lookahead );
		}

		/* Close the closure! */
		MSG( "Performing closure" );

		do
		{
			prev_cnt = cnt;
			cnt = 0;

			/* Loop trought all items of the current state */
			plist_for( closure, e )
			{
				it = (pglritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !( lhs = pg_prod_get_rhs( it->prod, it->dot ) )
						|| !pg_symbol_is_nonterm( lhs ) )
					continue;

				/* Add all productions of the nonterminal to the closure,
					if not already in */
				for( i = 0; ( prod = pg_prod_get_by_lhs( lhs, i ) ); i++ )
				{
					plist_for( closure, f )
					{
						cit = (pglritem*)plist_access( f );
						if( cit->prod == prod )
							break;
					}

					if( !f )
						cit = pg_lritem_create( closure, prod, 0 );

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
								( sym = pg_prod_get_rhs( it->prod, j ) );
									j++ )
						{
							plist_union( cit->lookahead, sym->first );

							if( !sym->nullable )
								break;
						}

						/*
							If all symbols right to the dot are nullable
							(or there are simply none), then add the current
							items lookahead to the closed items lookahead.
						*/
						if( !sym )
							plist_union( cit->lookahead, it->lookahead );
					}
				}
			}

			cnt = plist_count( closure );
		}
		while( prev_cnt != cnt );

		MSG( "Closure finished" );

		/* Add current kernel to closure */
		pg_lritems_print( closure, stderr, "Closure" );

		/* Create new states from the items in the closure having a symbol
			right to their dot */
		do
		{
			sym = (pgsymbol*)NULL;
			plist_clear( part );

			for( e = plist_first( closure ); e; )
			{
				it = (pglritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !sym && !( sym = pg_prod_get_rhs(
											it->prod, it->dot ) ) )
				{
					e = plist_next( e );
					continue;
				}

				/* Add item to new state kernel */
				if( pg_prod_get_rhs( it->prod, it->dot ) == sym )
				{
					it->dot++;
					plist_push( part, it );

					f = plist_prev( e );
					plist_remove( closure, e );

					if( !( e = f ) )
						e = plist_first( closure );
				}
				else
					e = plist_next( e );
			}

			/* Stop if no more partitions found (there is no first item) */
			if( !( it = (pglritem*)plist_access( plist_first( part ) ) ) )
				break;

			/*
				Can we do a shift and reduce in one transition?

				Watch for partitions that are

					x -> y .z

				where x is nonterminal, y is a possible sequence of
				terminals and/or nonterminals or even epsilon, and z is a
				terminal or nonterminal.
			*/
			if( pg_parser_get_optimize( parser )
					&& ( plist_count( part ) == 1
							&& !pg_prod_get_rhs( it->prod, it->dot ) ) )
			{
				MSG( "State optimization" );

				if( !st->closed )
					pg_lrcolumn_create( st, sym,
							(pglrstate*)it->prod, it->prod );

				/* Forget current partition
					- its not needed anymore... */
				plist_for( part, e )
					pg_lritem_free( (pglritem*)plist_access( e ) );

				continue;
			}

			MSG( "Check in state pool for same kernel configuration" );
			plist_for( parser->states, e )
			{
				nst = (pglrstate*)plist_access( e );

				if( pg_parser_lr_compare(
						part, nst->kernel, parser->paradigm ) )
					break;
			}

			/* State does not already exists?
				Create it as new! */
			if( !e )
			{
				MSG( "No such state, creating new state from current config" );
				pg_lritems_print( part, stderr, "Kernel" );
				nst = pg_lrstate_create( parser, part );
			}
			else
			/* State already exists?
				Merge lookaheads (if needed). */
			{
				MSG( "There is a state with such configuration" );
				if( parser->paradigm != PGPARADIGM_LR0 )
				{
					MSG( "Merging lookahead" );

					cnt = 0;
					prev_cnt = 0;

					for( e = plist_first( nst->kernel ),
							f = plist_first( part ); e;
								e = plist_next( e ), f = plist_next( f ) )
					{
						it = (pglritem*)plist_access( e );
						cit = (pglritem*)plist_access( f );

						prev_cnt += plist_count( it->lookahead );
						plist_union( it->lookahead, cit->lookahead );
						cnt += plist_count( it->lookahead );
					}

					if( cnt != prev_cnt )
						nst->done = FALSE;
				}

				pg_lritems_print( st->kernel, stderr, "Kernel" );
			}

			if( sym && !st->closed )
				pg_lrcolumn_create( st, sym, nst, (pgprod*)NULL );
		}
		while( TRUE );

		/* getchar(); */

		st->closed = TRUE;
		MSG( "State closed" );
	}

	plist_free( closure );
	plist_free( part );

	MSG( "Performing reductions" );
	plist_for( parser->states, e )
	{
		st = (pglrstate*)plist_access( e );

		plist_for( st->kernel, f )
		{
			it = (pglritem*)plist_access( f );

			if( pg_prod_get_rhs( it->prod, it->dot ) )
				continue;

			plist_for( it->lookahead, g )
			{
				sym = (pgsymbol*)plist_access( g );

				plist_for( st->actions, h )
				{
					col = (pglrcolumn*)plist_access( h );
					if( col->symbol == sym )
						break;
				}

				if( h )
				{
					/* TODO Conflict resolution */
					fprintf( stderr,
						"Conflict %p/%p on %s, now %s\n",
							col->shift, col->reduce,
								pg_symbol_get_name( col->symbol ),
									pg_symbol_get_name( sym ) );
				}

				pg_lrcolumn_create( st, sym, (pglrstate*)NULL, it->prod );
			}
		}
	}

	fprintf( stderr, "\n*** FINAL STATES***\n\n" );

	plist_for( parser->states, e )
	{
		st = (pglrstate*)plist_access( e );
		fprintf( stderr, "-- State %d %p --\n",
			plist_offset( e ), plist_access( e ) );

		pg_lritems_print( st->kernel, stderr, "Kernel" );

		plist_for( st->actions, f )
		{
			col = (pglrcolumn*)plist_access( f );

			if( col->shift && col->reduce )
				fprintf( stderr, "\t<- Shift/Reduce on '%s' by "
									"production '%s'\n",
							pg_symbol_get_name( col->symbol ),
								pg_prod_to_string( col->reduce ) );
			else if( col->shift )
				fprintf( stderr, "\t-> Shift on '%s' to state %d\n",
							pg_symbol_get_name( col->symbol ),
								plist_offset( plist_get_by_ptr(
									parser->states, col->shift ) ) );
			else if( col->reduce )
				fprintf( stderr, "\t<- Reduce on '%s' by production '%s'\n",
							pg_symbol_get_name( col->symbol ),
								pg_prod_to_string( col->reduce ) );
			else
				fprintf( stderr, "\tXX Error on '%s'\n",
					pg_symbol_get_name( col->symbol ) );
		}

		plist_for( st->gotos, f )
		{
			col = (pglrcolumn*)plist_access( f );

			if( col->shift && col->reduce )
				fprintf( stderr, "\t<- Goto/Reduce by production '%s' in '%s'\n",
							pg_prod_to_string( col->reduce ),
								pg_symbol_get_name( col->symbol ) );
			else if( col->shift )
				fprintf( stderr, "\t-> Goto state %d on '%s'\n",
							plist_offset( plist_get_by_ptr(
									parser->states, col->shift ) ),
								pg_symbol_get_name( col->symbol ) );
			else
				MISSINGCASE;
		}
	}

	MSG( "Finished" );
	RETURN( TRUE );
}

BOOLEAN pg_parser_lr_reset( pgparser* parser )
{
	plistel*	e;
	pglrstate*	st;

	if( !( parser ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !pg_parser_is_lr( parser ) )
	{
		PGERR( pg_parser_get_grammar( parser ),
				__FILE__, __LINE__, "Wrong paradigm to function" );
		return FALSE;
	}

	if( parser->states )
	{
		plist_for( parser->states, e )
		{
			st = (pglrstate*)plist_access( e );
			pg_lrstate_free( st );
		}

		plist_clear( parser->states );
	}
	else
		parser->states = plist_create( sizeof( pglrstate ), PLIST_MOD_RECYCLE );

	return TRUE;
}
