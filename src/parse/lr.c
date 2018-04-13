/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.c
Usage:	LR/LALR/SLR parse table construction and execution.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Defines */

#define DEBUGLEVEL		0

/* Closure item */
typedef struct
{
	ppprod*			prod;			/* Production */
	int				dot;			/* Dot offset */
	plist*			lookahead;		/* Lookahead symbols */
} pplritem;

/* LR-State */
typedef struct
{
	int				idx;			/* State index */

	plist*			kernel;			/* Kernel items */
	plist*			epsilon;		/* Empty items */

	plist*			actions;		/* Action row entries */
	plist*			gotos;			/* Goto row entries */

	ppprod*			def_prod;		/* Default production */

	pboolean		done;			/* Done flag */
	pboolean		closed;			/* Closed flag */
} pplrstate;

/* LR-Transition */
typedef struct
{
	ppsym*			symbol;			/* Symbol */

	pplrstate*		shift;			/* Shift to state */
	ppprod*			reduce;			/* Reduce by production */
} pplrcolumn;

/* LR/LALR parser */

/* Debug for one lritem */
static void pp_lritem_print( pplritem* it )
{
	int			i;
	ppsym*		sym;
	plistel*	e;

	if( ( !it ) )
	{
		WRONGPARAM;
		return;
	}

	fprintf( stderr, "%s : ", pp_sym_to_str( it->prod->lhs ) );

	for( i = 0; i < plist_count( it->prod->rhs ); i++ )
	{
		sym = pp_prod_getfromrhs( it->prod, i );

		if( i > 0 )
			fprintf( stderr, " " );

		if( i == it->dot )
			fprintf( stderr, ". " );

		fprintf( stderr, "%s", pp_sym_to_str( sym ) );
	}

	if( i == it->dot )
	{
		fprintf( stderr, " ." );

		if( plist_count( it->lookahead ) )
		{
			fprintf( stderr, "   { " );

			plist_for( it->lookahead, e )
			{
				if( e != plist_first( it->lookahead ) )
					fprintf( stderr, " " );

				fprintf( stderr, ">%s<",
					pp_sym_to_str( ( (ppsym*)plist_access( e ) ) ) );
			}

			fprintf( stderr, " }" );
		}
	}

	fprintf( stderr, "\n" );
}

/* Debug for an item set consisting of lritems */
static void pp_lritems_print( plist* items, char* what )
{
	plistel*	e;

	if( ( !items ) )
		return;

	if( what && *what )
		fprintf( stderr, "%s (%d):\n", what, plist_count( items ) );

	if( !plist_count( items ) )
		fprintf( stderr, "\t(empty)\n" );

	plist_for( items, e )
	{
		if( what && *what )
			fprintf( stderr, "\t" );

		pp_lritem_print( (pplritem*)plist_access( e ) );
	}
}

/* Priority sort function for the lookahead-sets */
static int pp_lritem_lookahead_sort( plist* list, plistel* el, plistel* er )
{
	ppsym*	l	= (ppsym*)plist_access( el );
	ppsym*	r	= (ppsym*)plist_access( er );

	/* By idx order */
	if( l->idx < r->idx )
		return 1;

	return 0;
}

static pplritem* pp_lritem_create( plist* list, ppprod* prod, int dot )
{
	pplritem*	item;

	if( !( list && prod ) )
	{
		WRONGPARAM;
		return (pplritem*)NULL;
	}

	if( dot < 0 )
		dot = 0;
	else if( dot > plist_count( prod->rhs ) )
		dot = plist_count( prod->rhs );

	item = (pplritem*)plist_malloc( list );
	item->prod = prod;
	item->dot = dot;

	item->lookahead = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_AUTOSORT );
	plist_set_sortfn( item->lookahead, pp_lritem_lookahead_sort );

	return item;
}

static pplritem* pp_lritem_free( pplritem* it )
{
	if( !( it ) )
		return (pplritem*)NULL;

	plist_free( it->lookahead );

	return (pplritem*)NULL;
}

static pplrcolumn* pp_lrcolumn_create(
	pplrstate* st, ppsym* sym, pplrstate* shift, ppprod* reduce )
{
	pplrcolumn*		col;

	if( PPSYM_IS_TERMINAL( sym ) )
		col = (pplrcolumn*)plist_malloc( st->actions );
	else
		col = (pplrcolumn*)plist_malloc( st->gotos );

	col->symbol = sym;
	col->shift = shift;
	col->reduce = reduce;

	return col;
}

static pplrstate* pp_lrstate_create( plist* states, plist* kernel )
{
	plistel*	e;
	pplrstate*	state;

	if( !( states ) )
	{
		WRONGPARAM;
		return (pplrstate*)NULL;
	}

	state = plist_malloc( states );

	state->idx = plist_count( states ) - 1;
	state->kernel = plist_create( sizeof( pplritem ), PLIST_MOD_NONE );
	state->epsilon = plist_create( sizeof( pplritem ), PLIST_MOD_NONE );

	state->actions = plist_create( sizeof( pplrcolumn ), PLIST_MOD_NONE );
	state->gotos = plist_create( sizeof( pplrcolumn ), PLIST_MOD_NONE );

	plist_for( kernel, e )
		plist_push( state->kernel, plist_access( e ) );

	return state;
}

static plist* pp_lr_free( plist* states )
{
	pplrstate*	st;
	plistel*	e;
	plistel*	f;

	plist_for( states, e )
	{
		st = (pplrstate*)plist_access( e );

		/* Parse tables */
		plist_free( st->actions );
		plist_free( st->gotos );

		/* Kernel */
		plist_for( st->kernel, f )
			pp_lritem_free( (pplritem*)plist_access( f ) );

		/* Epsilons */
		plist_for( st->epsilon, f )
			pp_lritem_free( (pplritem*)plist_access( f ) );
	}

	return plist_free( states );
}

static void pp_lrstate_print( pplrstate* st )
{
	plistel*	e;
	pplrcolumn*	col;

	fprintf( stderr, "\n-- State %d --\n", st->idx );

	pp_lritems_print( st->kernel, "Kernel" );
	pp_lritems_print( st->epsilon, "Epsilon" );

	fprintf( stderr, "\n" );

	plist_for( st->actions, e )
	{
		col = (pplrcolumn*)plist_access( e );

		if( col->shift && col->reduce )
			fprintf( stderr, " <- Shift/Reduce on '%s' by "
								"production '%s'\n",
						col->symbol->name,
							pp_prod_to_str( col->reduce ) );
		else if( col->shift )
			fprintf( stderr, " -> Shift on '%s', goto state %d\n",
						col->symbol->name, col->shift->idx );
		else if( col->reduce )
			fprintf( stderr, " <- Reduce on '%s' by production '%s'\n",
						col->symbol->name,
							pp_prod_to_str( col->reduce ) );
		else
			fprintf( stderr, " XX Error on '%s'\n", col->symbol->name );
	}

	if( st->def_prod )
		fprintf( stderr, " <- Reduce (default) on '%s'\n",
			pp_prod_to_str( st->def_prod ) );

	plist_for( st->gotos, e )
	{
		col = (pplrcolumn*)plist_access( e );

		if( col->shift && col->reduce )
			fprintf( stderr, " <- Goto/Reduce by production "
									"'%s' in '%s'\n",
						pp_prod_to_str( col->reduce ),
							col->symbol->name );
		else if( col->shift )
			fprintf( stderr, " -> Goto state %d on '%s'\n",
						col->shift->idx,
							col->symbol->name );
		else
			MISSINGCASE;
	}
}

#if DEBUGLEVEL > 0
static void pp_lr_print( plist* states )
{
	plistel*	e;
	pplrstate*	st;

	plist_for( states, e )
	{
		st = (pplrstate*)plist_access( e );
		pp_lrstate_print( st );
	}
}
#endif

static pboolean lr_compare( plist* set1, plist* set2 )
{
	plistel*	e;
	plistel*	f;
	pplritem*	it1;
	pplritem*	it2;
	int			same	= 0;

	if( plist_count( set1 ) == plist_count( set2 ) )
	{
		plist_for( set1, e )
		{
			it1 = (pplritem*)plist_access( e );

			plist_for( set2, f )
			{
				it2 = (pplritem*)plist_access( f );

				if( it1->prod == it2->prod && it1->dot == it2->dot )
				{
					/* To become LR(1), uncomment this: */
					/* if( !plist_diff( it1->lookahead, it2->lookahead ) ) */
					same++;
				}
			}
		}

		if( plist_count( set1 ) == same )
			return TRUE;
	}

	return FALSE;
}

static pplrstate* lr_get_undone( plist* states )
{
	pplrstate*	st;
	plistel*	e;

	plist_for( states, e )
	{
		st = (pplrstate*)plist_access( e );

		if( !st->done )
			return st;
	}

	return (pplrstate*)NULL;
}

static plist* pp_lr_closure( ppgram* gram, pboolean optimize, pboolean resolve )
{
	plist*			states;
	pplrstate*		st;
	pplrstate*		nst;
	pplritem*		it;
	pplritem*		kit;
	pplritem*		cit;
	ppsym*			sym;
	ppsym*			lhs;
	ppprod*			prod;
	pplrcolumn*		col;
	pplrcolumn*		ccol;
	plist*			closure;
	plist*			part;
	plistel*		e;
	plistel*		f;
	plistel*		g;
	int				i;
	int				j;
	int				cnt;
	int				prev_cnt;
	int*			prodcnt;
	pboolean		printed;

	PROC( "pp_lr_closure" );
	PARMS( "gram", "%p", gram );
	PARMS( "optimize", "%s", BOOLEAN_STR( optimize ) );
	PARMS( "resolve", "%s", BOOLEAN_STR( resolve ) );

	if( !( gram ) )
	{
		WRONGPARAM;
		RETURN( (plist*)NULL );
	}

	MSG( "Initializing states list" );
	states = plist_create( sizeof( pplrstate ), PLIST_MOD_RECYCLE );

	MSG( "Creating closure seed" );
	nst = pp_lrstate_create( states, (plist*)NULL );
	it = pp_lritem_create( nst->kernel, pp_sym_getprod( gram->goal, 0 ), 0 );

	plist_push( it->lookahead, gram->eof );

	MSG( "Initializing part and closure lists" );
	part = plist_create( sizeof( pplritem ), PLIST_MOD_RECYCLE );
	closure = plist_create( sizeof( pplritem ), PLIST_MOD_RECYCLE );

	MSG( "Run the closure loop" );
	while( ( st = lr_get_undone( states ) ) )
	{
		st->done = TRUE;

		LOG( "--- Closing state %d",
				plist_offset( plist_get_by_ptr( states, st ) ) );

		MSG( "Closing state" );
		VARS( "State", "%d", plist_offset(
								plist_get_by_ptr( states, st ) ) );

		/* Close all items of the current state */
		cnt = 0;
		plist_clear( closure );

#if DEBUGLEVEL > 1
		pp_lritems_print( st->kernel, "Kernel" );
#endif

		/* Duplicate state kernel to closure */
		MSG( "Duplicate current kernel to closure" );
		plist_for( st->kernel, e )
		{
			kit = (pplritem*)plist_access( e );

			/* Add only items that have a symbol on the right of the dot */
			if( pp_prod_getfromrhs( kit->prod, kit->dot ) )
			{
				it = pp_lritem_create( closure, kit->prod, kit->dot );
				it->lookahead = plist_dup( kit->lookahead );
			}
		}

		/* Close the closure! */
		MSG( "Performing closure" );
		do
		{
			prev_cnt = cnt;
			cnt = 0;

			/* Loop throught all items of the current state */
			plist_for( closure, e )
			{
				it = (pplritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !( lhs = pp_prod_getfromrhs( it->prod, it->dot ) )
						|| PPSYM_IS_TERMINAL( lhs ) )
					continue;

				/* Add all prods of the nonterminal to the closure,
					if not already in */
				for( i = 0; ( prod = pp_sym_getprod( lhs, i ) ); i++ )
				{
					plist_for( closure, f )
					{
						cit = (pplritem*)plist_access( f );
						if( cit->prod == prod && cit->dot == 0 )
							break;
					}

					if( !f )
						cit = pp_lritem_create( closure, prod, 0 );

					/* Merge lookahead */

					/*
						Find out all lookahead symbols by merging the
						FIRST-sets of all nullable and the first
						non-nullable items on the prods right-hand
						side.
					*/
					for( j = it->dot + 1;
							( sym = pp_prod_getfromrhs( it->prod, j ) );
								j++ )
					{
						plist_union( cit->lookahead, sym->first );

						if( !( sym->flags & PPFLAG_NULLABLE ) )
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

			cnt = plist_count( closure );
		}
		while( prev_cnt != cnt );
		MSG( "Closure algorithm done" );

		/* Move all epsilon closures into state's epsilon list */
		for( e = plist_first( closure ); e; )
		{
			it = (pplritem*)plist_access( e );

			if( plist_count( it->prod->rhs ) > 0 )
				e = plist_next( e );
			else
			{
				plist_for( st->epsilon, f )
				{
					kit = (pplritem*)plist_access( f );
					if( kit->prod == it->prod )
						break;
				}

				if( !f )
					plist_push( st->epsilon, it );
				else
					plist_union( kit->lookahead, it->lookahead );

				f = e;
				e = plist_next( e );
				plist_remove( closure, f );
			}
		}

		MSG( "Closure finished!" );
#if DEBUGLEVEL > 2
		pp_lritems_print( closure, "Closure" );
#endif

		/* Create new states from the items in the closure having a symbol
			right to their dot */
		do
		{
			sym = (ppsym*)NULL;
			plist_clear( part );

			for( e = plist_first( closure ); e; )
			{
				it = (pplritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !sym && !( sym = pp_prod_getfromrhs( it->prod, it->dot ) ) )
				{
					e = plist_next( e );
					continue;
				}

				/* Add item to new state kernel */
				if( sym == pp_prod_getfromrhs( it->prod, it->dot ) )
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
			if( !( it = (pplritem*)plist_access( plist_first( part ) ) ) )
				break;

			/*
				Can we do a shift and reduce in one transition?

				Watch for partitions that are

					x -> y .z

				where x is nonterminal, y is a possible sequence of
				terminals and/or nonterminals, or even epsilon, and z is a
				terminal or nonterminal.
			*/
			if( optimize && ( plist_count( part ) == 1
								&& !pp_prod_getfromrhs( it->prod, it->dot ) ) )
			{
				MSG( "State optimization" );

				if( !st->closed )
					pp_lrcolumn_create( st, sym,
							(pplrstate*)it->prod, it->prod );

				/* Forget current partition; its not needed anymore... */
				plist_for( part, e )
					pp_lritem_free( (pplritem*)plist_access( e ) );

				continue;
			}

			MSG( "Check in state pool for same kernel configuration" );
			plist_for( states, e )
			{
				nst = (pplrstate*)plist_access( e );

				if( lr_compare( part, nst->kernel ) )
					break;
			}

			/* State does not already exists?
				Create it as new! */
			if( !e )
			{
				MSG( "No such state, creating new state from current config" );
#if DEBUGLEVEL > 2
				pp_lritems_print( part, "NEW Kernel" );
#endif
				nst = pp_lrstate_create( states, part );
			}
			else
			/* State already exists?
				Merge lookaheads (if needed). */
			{
				MSG( "There is a state with such configuration" );

				/* Merge lookahead */
				cnt = 0;
				prev_cnt = 0;

				for( e = plist_first( nst->kernel ),
						f = plist_first( part ); e;
							e = plist_next( e ), f = plist_next( f ) )
				{
					it = (pplritem*)plist_access( e );
					cit = (pplritem*)plist_access( f );

					prev_cnt += plist_count( it->lookahead );
					plist_union( it->lookahead, cit->lookahead );
					cnt += plist_count( it->lookahead );
				}

				if( cnt != prev_cnt )
					nst->done = FALSE;

#if DEBUGLEVEL > 2
				pp_lritems_print( st->kernel, "EXT Kernel" );
#endif
			}

			if( sym && !st->closed )
				pp_lrcolumn_create( st, sym, nst, (ppprod*)NULL );
		}
		while( TRUE );

		st->closed = TRUE;
		MSG( "State closed" );
	}

	plist_free( closure );
	plist_free( part );

	MSG( "Performing reductions" );

	prodcnt = (int*)pmalloc( plist_count( gram->prods ) * sizeof( int ) );

	plist_for( states, e )
	{
		printed = FALSE;
		st = (pplrstate*)plist_access( e );

		LOG( "State %d", plist_offset( plist_get_by_ptr( states, st ) ) );

		/* Reductions */
		for( part = st->kernel; part;
				part = ( part == st->kernel ? st->epsilon : (plist*)NULL ) )
		{
			plist_for( part, f )
			{
				it = (pplritem*)plist_access( f );

				/* Only for items which have the dot at the end */
				if( pp_prod_getfromrhs( it->prod, it->dot ) )
					continue;

				/* Put entries for each lookahead */
				plist_for( it->lookahead, g )
				{
					sym = (ppsym*)plist_access( g );
					pp_lrcolumn_create( st, sym, (pplrstate*)NULL, it->prod );
				}
			}
		}

		MSG( "Detect and report, or resolve conflicts" );

		for( f = plist_first( st->actions ); f; )
		{
			col = (pplrcolumn*)plist_access( f );

			for( g = plist_next( f ); g; g = plist_next( g ) )
			{
				ccol = (pplrcolumn*)plist_access( g );

				if( ccol->symbol == col->symbol
						&& ccol->reduce ) /* Assertion: Shift-shift entries
														are never possible! */
				{
					LOG( "State %d encounters %s/reduce-conflict on %s",
							plist_offset( e ),
								col->reduce ? "reduce" : "shift",
									col->symbol->name );

					if( resolve )
					{
						/* Try to resolve reduce-reduce conflict */
						if( col->reduce )
						{
							/* Resolve by lower production! */
							if( col->reduce->idx > ccol->reduce->idx )
							{
								MSG( "Conflict resolved in favor of "
										"lower production" );
								col->reduce = ccol->reduce;

								LOG( "Conflict resolved by lower production %d",
										ccol->reduce );
							}

							plist_remove( st->actions, g );

							/* Clear non-associative entry! */
							if( col->symbol->assoc == PPASSOC_NOT )
							{
								LOG( "Symbol '%s' is non-associative, "
										"removing entry",
										pp_sym_to_str( col->symbol ) );

								plist_remove( st->actions, f );
								f = g = (plistel*)NULL;
							}

							break; /* Restart search! */
						}
						/* Try to resolve shift-reduce conflict */
						else
						{
							/* In case there are precedences,
								resolving is possible */
							if( col->symbol->prec && ccol->reduce->prec )
							{
								/* Resolve by symbol/production precedence,
									or by associativity */
								if( col->symbol->prec < ccol->reduce->prec
									|| ( col->symbol->prec == ccol->reduce->prec
										&& col->symbol->assoc == PPASSOC_LEFT )
										)
								{
									MSG( "Conflict resolved "
											"in favor of reduce" );
									col->reduce = ccol->reduce;
									col->shift = (pplrstate*)NULL;
								}
								/* Clear non-associative entry! */
								else if( col->symbol->assoc == PPASSOC_NOT )
								{
									LOG( "Symbol '%s' is non-associative, "
											"removing entry",
											pp_sym_to_str( col->symbol ) );

									plist_remove( st->actions, f );
									f = g = (plistel*)NULL;
									break;
								}
								else
								{
									MSG( "Conflict resolved in favor"
											"of shift" );
								}

								plist_remove( st->actions, g );
								break;
							}
						}
					}

					/* If no resolution is possible or wanted,
						report conflict */
					if( !printed )
					{
#if DEBUGLEVEL > 0
						fprintf( stderr, "\n\n--- CONFLICTS ---\n\n" );
#endif
						pp_lrstate_print( (pplrstate*)plist_access( e ) );
						printed = TRUE;
					}

					fprintf( stderr,
						"State %d encounters %s/reduce-conflict on %s\n",
							plist_offset( e ),
								col->reduce ? "reduce" : "shift",
									col->symbol->name );
				}
			}

			if( g )
				continue;

			if( f )
				f = plist_next( f );
			else
				f = plist_first( st->actions );
		}

		#if 0
		MSG( "Detect default productions" );

		memset( prodcnt, 0, plist_count( gram->prods ) * sizeof( int ) );
		cnt = 0;

		plist_for( st->actions, f )
		{
			col = (pplrcolumn*)plist_access( f );

			if( col->reduce && prodcnt[ col->reduce->idx ]++ > cnt )
			{
				cnt = prodcnt[ col->reduce->idx ];
				st->def_prod = col->reduce;
			}
		}

		/* Remove all parser actions that match the default production */
		if( st->def_prod )
		{
			for( f = plist_first( st->actions ); f; )
			{
				col = (pplrcolumn*)plist_access( f );

				if( col->reduce == st->def_prod )
				{
					g = plist_next( f );
					plist_remove( st->actions, f );
					f = g;
				}
				else
					f = plist_next( f );
			}
		}
		#endif
	}

	pfree( prodcnt );

	MSG( "Finished" );
	VARS( "States generated", "%d", plist_count( states ) );

	RETURN( states );
}

/** Build parse tables */
pboolean pp_lr_build( unsigned int* cnt, unsigned int*** dfa, ppgram* grm )
{
	plist*			states;
	unsigned int**	tab;
	pplrstate*		st;
	pplrcolumn*		col;
	unsigned int	total;
	int				i;
	int				j;
	plistel*		e;
	plistel*		f;

	PROC( "pp_lr_build" );
	PARMS( "cnt", "%p", cnt );
	PARMS( "dfa", "%p", dfa );
	PARMS( "grm", "%p", grm );

	if( !grm )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !( grm->flags & PPFLAG_FINALIZED ) )
	{
		fprintf( stderr, "Grammar is not finalized, "
			"please run pp_gram_prepare() first!\n" );
		RETURN( FALSE );
	}

	/* Compute LALR(1) states */
	states = pp_lr_closure( grm, TRUE, TRUE );

	#if DEBUGLEVEL > 0
	fprintf( stderr, "\n\n--- FINAL GLR STATES ---\n\n" );
	pp_lr_print( states );
	#endif

	/* Allocate and fill return tables, free states */
	MSG( "Filling return array" );

	tab = (unsigned int**)pmalloc( plist_count( states ) * sizeof( int* ) );

	for( i = 0, e = plist_first( states ); e; e = plist_next( e ), i++ )
	{
		st = (pplrstate*)plist_access( e );
		VARS( "State", "%d", i );

		total = plist_count( st->actions ) * 3
					+ plist_count( st->gotos ) * 3
						+ 2;

		tab[ i ] = (unsigned int*)pmalloc( total * sizeof( int ) );
		tab[ i ][ 0 ] = total;
		tab[ i ][ 1 ] = st->def_prod ? st->def_prod->idx + 1 : 0;

		/* Actions */
		for( j = 2, f = plist_first( st->actions );
				f; f = plist_next( f ), j += 3 )
		{
			col = (pplrcolumn*)plist_access( f );

			tab[ i ][ j ] = col->symbol->idx + 1;

			if( col->shift )
				tab[ i ][ j + 1 ] = PPLR_SHIFT;

			if( col->reduce )
			{
				tab[ i ][ j + 1 ] |= PPLR_REDUCE;
				tab[ i ][ j + 2 ] = col->reduce->idx + 1;
			}
			else
				tab[ i ][ j + 2 ] = col->shift->idx + 1;
		}

		/* Gotos */
		for( f = plist_first( st->gotos );
				f; f = plist_next( f ), j += 3 )
		{
			col = (pplrcolumn*)plist_access( f );

			tab[ i ][ j ] = col->symbol->idx + 1;

			if( col->shift )
				tab[ i ][ j + 1 ] = PPLR_SHIFT;

			if( col->reduce )
			{
				tab[ i ][ j + 1 ] |= PPLR_REDUCE;
				tab[ i ][ j + 2 ] = col->reduce->idx + 1;
			}
			else
				tab[ i ][ j + 2 ] = col->shift->idx + 1;
		}
	}

	total = plist_count( states );

	/* Clean-up */
	pp_lr_free( states );

	/* Fill return pointer or print */
	if( !dfa )
	{
		/* DEBUG */
		fprintf( stderr, "count = %d\n", total );

		for( i = 0; i < plist_count( states ); i++ )
		{
			fprintf( stderr, "%02d:", i );

			fprintf( stderr, " def:%02d",  tab[ i ][ 1 ] );

			for( j = 2; j < tab[ i ][ 0 ]; j += 3 )
				fprintf( stderr, " %02d:%s%s:%02d",
					tab[ i ][ j ],
					tab[ i ][ j + 1 ] & PPLR_SHIFT ? "s" : "-",
					tab[ i ][ j + 1 ] & PPLR_REDUCE ? "r" : "-",
					tab[ i ][ j + 2 ] );

			fprintf( stderr, "\n" );
		}

		pfree( tab );
	}
	else
		*dfa = tab;

	if( cnt )
		*cnt = total;

	VARS( "states total", "%d", total );
	RETURN( TRUE );
}

