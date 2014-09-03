/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"
#define MALLOCSTEP		255

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
	int				id;
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

/* LR-Stackitem */
typedef struct
{
	ppsym*			symbol;	/* Symbol */
	pplrstate*		state;	/* State */
} pplrse;

#define PPLR_SHIFT	1
#define PPLR_REDUCE	2

/* LR/LALR parser */

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

	pp_sym_print( it->prod->lhs );
	printf( " : " );

	for( i = 0; i < plist_count( it->prod->rhs ); i++ )
	{
		sym = pp_prod_getfromrhs( it->prod, i );

		if( i > 0 )
			printf( " " );

		if( i == it->dot )
			printf( ". " );

		pp_sym_print( sym );
	}

	if( i == it->dot )
	{
		printf( " ." );

		if( it->lookahead )
		{
			printf( "   { " );

			plist_for( it->lookahead, e )
			{
				if( e != plist_first( it->lookahead ) )
					printf( " " );

				/* printf( "(%s)", ((ppsym*)plist_access( e ))->name ); */
				pp_sym_print( (ppsym*)plist_access( e ) );
			}

			printf( " }" );
		}
	}

	printf( "\n" );
}

static void pp_lritems_print( plist* items, char* what )
{
	plistel*	e;

	if( ( !items ) )
		return;

	if( what && *what )
		printf( "%s (%d):\n", what, plist_count( items ) );

	if( !plist_count( items ) )
		printf( "\t(empty)\n" );

	plist_for( items, e )
	{
		if( what && *what )
			printf( "\t" );

		pp_lritem_print( (pplritem*)plist_access( e ) );
	}
}

/* Priority sort function for the lookahead-sets */
static int pp_lritem_lookahead_sort( plist* list, plistel* el, plistel* er )
{
	ppsym*	l	= (ppsym*)plist_access( el );
	ppsym*	r	= (ppsym*)plist_access( er );

	/* Higher type before lower type, then by definition order */
	if( ( l->type == r->type && l->id < r->id ) || l->type > r->type )
		return 1;

	return 0;
}

/* Debug */
static void pp_lritem_lookahead_print( plist* list )
{
	plistel*	e;
	ppsym*		s;

	printf( "[[" );

	plist_for( list, e )
	{
		s = (ppsym*)plist_access( e );

		printf( " " );
		pp_sym_print( s );
	}

	printf( " ]]\n" );
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
	/* plist_set_printfn( item->lookahead, pp_lritem_lookahead_print ); */

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

	if( sym->type != PPSYMTYPE_NONTERM )
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

	state->kernel = plist_create( sizeof( pplritem ), PLIST_MOD_NONE );
	state->epsilon = plist_create( sizeof( pplritem ), PLIST_MOD_NONE );

	state->actions = plist_create( sizeof( pplrcolumn ), PLIST_MOD_NONE );
	state->gotos = plist_create( sizeof( pplrcolumn ), PLIST_MOD_NONE );

	plist_for( kernel, e )
		plist_push( state->kernel, plist_access( e ) );

	return state;
}

static plist* pp_lrstates_free( plist* states )
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

static pp_lrstates_print( plist* states )
{
	plistel*	e;
	plistel*	f;
	pplrstate*	st;
	pplrcolumn*	col;

	plist_for( states, e )
	{
		st = (pplrstate*)plist_access( e );
		fprintf( stderr, "\n-- State %d %p --\n",
			plist_offset( e ), plist_access( e ) );

		pp_lritems_print( st->kernel, "Kernel" );
		pp_lritems_print( st->epsilon, "Epsilon" );

		fprintf( stderr, "\n" );

		plist_for( st->actions, f )
		{
			col = (pplrcolumn*)plist_access( f );

			if( col->shift && col->reduce )
				fprintf( stderr, " <- Shift/Reduce on '%s' by "
									"production '%s'\n",
							col->symbol->name,
								pp_prod_to_str( col->reduce ) );
			else if( col->shift )
				fprintf( stderr, " -> Shift on '%s', goto state %d\n",
							col->symbol->name,
								plist_offset( plist_get_by_ptr(
									states, col->shift ) ) );
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

		plist_for( st->gotos, f )
		{
			col = (pplrcolumn*)plist_access( f );

			if( col->shift && col->reduce )
				fprintf( stderr, " <- Goto/Reduce by production "
										"'%s' in '%s'\n",
							pp_prod_to_str( col->reduce ),
								col->symbol->name );
			else if( col->shift )
				fprintf( stderr, " -> Goto state %d on '%s'\n",
							plist_offset(
								plist_get_by_ptr( states, col->shift ) ),
								col->symbol->name );
			else
				MISSINGCASE;
		}
	}
}

static int pp_lrstates_to_dfa( int*** act_tab, int*** go_tab, plist* states )
{
	pplrstate*	st;
	pplrcolumn*	col;
	int			i;
	int			j;
	plistel*	e;
	plistel*	f;

	PROC( "pp_lrstates_to_dfa" );
	PARMS( "act_tab", "%p", act_tab );
	PARMS( "go_tab", "%p", go_tab );
	PARMS( "states", "%p", states );

	if( !states )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	/* Allocate and fill return tables, free states */
	MSG( "Filling return arrays" );

	if( act_tab )
		*act_tab = (int**)pmalloc( plist_count( states ) * sizeof( int* ) );

	if( go_tab )
		*go_tab = (int**)pmalloc( plist_count( states ) * sizeof( int* ) );

	for( i = 0, e = plist_first( states ); e; e = plist_next( e ), i++ )
	{
		st = (pplrstate*)plist_access( e );
		VARS( "State", "%d", i );

		/* Action table */
		if( act_tab )
		{
			(*act_tab)[ i ] = (int*)pmalloc(
									( ( plist_count( st->actions ) * 3 ) + 2 )
										* sizeof( int ) );

			(*act_tab)[ i ][ 0 ] = ( plist_count( st->actions ) * 3 ) + 2;
			(*act_tab)[ i ][ 1 ] = st->def_prod ? st->def_prod->id : -1;

			for( j = 2, f = plist_first( st->actions );
					f; f = plist_next( f ), j += 3 )
			{
				col = (pplrcolumn*)plist_access( f );

				(*act_tab)[ i ][ j ] = col->symbol->id;

				if( col->shift )
					(*act_tab)[ i ][ j + 1 ] = PPLR_SHIFT;

				if( col->reduce )
				{
					(*act_tab)[ i ][ j + 1 ] |= PPLR_REDUCE;
					(*act_tab)[ i ][ j + 2 ] = col->reduce->id;
				}
				else
					(*act_tab)[ i ][ j + 2 ] = col->shift->id;
			}
		}

		/* Goto table */
		if( go_tab )
		{
			(*go_tab)[ i ] = (int*)pmalloc(
									( ( plist_count( st->actions ) * 3 ) + 1 )
										* sizeof( int ) );

			(*go_tab)[ i ][ 0 ] = ( ( plist_count( st->actions ) ) * 3 ) + 1;

			for( j = 2, f = plist_first( st->gotos );
					f; f = plist_next( f ), j += 3 )
			{
				col = (pplrcolumn*)plist_access( f );

				(*go_tab)[ i ][ j ] = col->symbol->id;

				if( col->shift )
					(*go_tab)[ i ][ j + 1 ] = PPLR_SHIFT;

				if( col->reduce )
				{
					(*go_tab)[ i ][ j + 1 ] |= PPLR_REDUCE;
					(*go_tab)[ i ][ j + 2 ] = col->reduce->id;
				}
				else
					(*go_tab)[ i ][ j + 2 ] = col->shift->id;
			}
		}
	}

	/* DEBUG */
	printf( "count = %d\n", plist_count( states ) );

	for( i = 0; i < plist_count( states ); i++ )
	{
		printf( "%02d:", i );

		printf( " def:%02d",  *(act_tab)[ i ][ 1 ] );

		for( j = 2; j < *(act_tab)[ i ][ 0 ]; j += 3 )
			printf( " %02d:%s%s:%02d",
				*(act_tab)[ i ][ j ],
				*(act_tab)[ i ][ j + 1 ] & PPLR_SHIFT ? "s" : "-",
				*(act_tab)[ i ][ j + 1 ] & PPLR_REDUCE ? "r" : "-",
				*(act_tab)[ i ][ j + 2 ] );

		printf( "\n          " );

		for( j = 1; j < *(go_tab)[ i ][ 0 ]; j += 3 )
			printf( " %02d:%s%s:%02d",
				*(go_tab)[ i ][ j ],
				*(go_tab)[ i ][ j + 1 ] & PPLR_SHIFT ? "g" : "-",
				*(go_tab)[ i ][ j + 1 ] & PPLR_REDUCE ? "r" : "-",
				*(go_tab)[ i ][ j + 2 ] );

		printf( "\n" );
	}

	VARS( "states count", "%d", plist_count( states ) );
	RETURN( plist_count( states ) );
}

static BOOLEAN pp_parser_lr_compare( plist* set1, plist* set2 )
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
					/*
					if( para != PGPARADIGM_LR1
							|| !plist_diff( it1->lookahead, it2->lookahead ) )
					*/
					same++;
				}
			}
		}

		if( plist_count( set1 ) == same )
			return TRUE;
	}

	return FALSE;
}

static pplrstate* pp_parser_lr_get_undone( plist* states )
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

plist* pp_parser_lr_closure( ppgram* gram, pboolean optimize )
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
	plistel*		h;
	int				i;
	int				j;
	int				cnt;
	int				prev_cnt;
	int*			prodcnt;

	PROC( "pp_parser_lr_closure" );
	PARMS( "states", "%p", states );
	PARMS( "gram", "%p", gram );

	if( !( gram ) )
	{
		WRONGPARAM;
		RETURN( (plist*)NULL );
	}

	MSG( "Initializing states list" );
	states = plist_create( sizeof( pplrstate ), PLIST_MOD_RECYCLE );

	MSG( "Creating closure seed" );
	nst = pp_lrstate_create( states, (plist*)NULL );
	it = pp_lritem_create( nst->kernel,
				(ppprod*)plist_access(
					plist_first( gram->goal->productions ) ), 0 );

	plist_push( it->lookahead, gram->eof );

	MSG( "Initializing part and closure lists" );
	part = plist_create( sizeof( pplritem ), PLIST_MOD_RECYCLE );
	closure = plist_create( sizeof( pplritem ), PLIST_MOD_RECYCLE );

	MSG( "Run the closure loop" );
	while( ( st = pp_parser_lr_get_undone( states ) ) )
	{
		st->done = TRUE;

#if ALGORITHM_TRACE
		fprintf( stderr, "---\nClosing state %d\n",
					plist_offset( plist_get_by_ptr( states, st ) ) );
#endif

		MSG( "Closing state" );
		VARS( "State", "%d", plist_offset(
								plist_get_by_ptr( states, st ) ) );

		/* Close all items of the current state */
		cnt = 0;
		plist_clear( closure );

#if ALGORITHM_TRACE
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

			/* Loop trought all items of the current state */
			plist_for( closure, e )
			{
				it = (pplritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !( lhs = pp_prod_getfromrhs( it->prod, it->dot ) )
						|| !( lhs->type == PPSYMTYPE_NONTERM ) )
					continue;

				/* Add all productions of the nonterminal to the closure,
					if not already in */
				for( i = 0; ( prod = (ppprod*)plist_access(
										plist_get( lhs->productions, i ) ) );
						i++ )
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
						non-nullable items on the productions right-hand
						side.
					*/
					for( j = it->dot + 1;
							( sym = pp_prod_getfromrhs( it->prod, j ) );
								j++ )
					{
						plist_union( cit->lookahead, sym->first );

						if( !sym->flags & PPFLAG_NULLABLE )
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
#if ALGORITHM_TRACE
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

				if( pp_parser_lr_compare( part, nst->kernel ) )
					break;
			}

			/* State does not already exists?
				Create it as new! */
			if( !e )
			{
				MSG( "No such state, creating new state from current config" );
#if ALGORITHM_TRACE
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

#if ALGORITHM_TRACE
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

	prodcnt = (int*)pmalloc( plist_count( gram->productions ) * sizeof( int ) );

	plist_for( states, e )
	{
		st = (pplrstate*)plist_access( e );

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

		/* Detect default productions */
		memset( prodcnt, 0, plist_count( gram->productions ) * sizeof( int ) );
		cnt = 0;

		plist_for( st->actions, f )
		{
			col = (pplrcolumn*)plist_access( f );

			if( col->reduce && prodcnt[ col->reduce->id ]++ > cnt )
			{
				cnt = prodcnt[ col->reduce->id ];
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
	}

	pfree( prodcnt );

	/* Detect and report conflicts */
	cnt = 0;
	plist_for( states, e )
	{
		st = (pplrstate*)plist_access( e );

		plist_for( st->actions, f )
		{
			col = (pplrcolumn*)plist_access( f );

			for( g = plist_next( f ); g; g = plist_next( g ) )
			{
				ccol = (pplrcolumn*)plist_access( g );

				if( ccol->symbol == col->symbol )
				{
					/* TODO Conflict resolution */
					fprintf( stderr,
						"\nState %d: %s/reduce on %s",
							plist_offset( e ),
								col->shift ? "shift" : "reduce",
									sym->name );
				}
			}
		}
	}

	MSG( "Finished" );
	VARS( "States generated", "%d", plist_count( states ) );

	RETURN( states );
}

static pplrse* push( pstack* stack, ppsym* symbol, pplrstate* state )
{
	pplrse	e;

	memset( &e, 0, sizeof( pplrse ) );
	e.symbol = symbol;
	e.state = state;

	pstack_push( stack, &e );
	return (pplrse*)pstack_top( stack );
}

static pplrse* pop( pstack* stack, int n )
{
	pplrse*	e;

	while( n-- > 0 )
		e = (pplrse*)pstack_pop( stack );

	return (pplrse*)pstack_top( stack );
}

static pboolean get_action( pplrstate** shift, ppprod** reduce,
								pplrse* tos, char** end )
{
	plistel*	e;
	pplrcolumn*	col;

	*shift = (pplrstate*)NULL;
	*reduce = (ppprod*)NULL;

	plist_for( tos->state->actions, e )
	{
		col = (pplrcolumn*)plist_access( e );

		fprintf( stderr, "Testing %s on >%s<\n", col->symbol->name, *end );

		if( pp_sym_in_input( col->symbol, *end, end ) )
		{
			*shift = col->shift;
			*reduce = col->reduce;
			return TRUE;
		}
	}

	if( ( *reduce = tos->state->def_prod ) )
		return TRUE;

	return FALSE;
}

static pboolean get_goto( pplrstate** shift, ppprod** reduce,
								pplrse* tos, ppsym* lhs )
{
	plistel*	e;
	pplrcolumn*	col;

	*shift = (pplrstate*)NULL;
	*reduce = (ppprod*)NULL;

	plist_for( tos->state->gotos, e )
	{
		col = (pplrcolumn*)plist_access( e );

		if( col->symbol == lhs )
		{
			*shift = col->shift;
			*reduce = col->reduce;
			return TRUE;
		}
	}

	return FALSE;
}

static pboolean pp_lr_PARSE( plist* ast, ppgram* grm, char* start, char** end,
									plist* states )
{
	ppsym*		lhs;
	pstack*		stack;
	pplrse*		tos;
	pplrstate*	shift;
	ppprod*		reduce;
	pboolean	token;

	stack = pstack_create( sizeof( pplrse ), MALLOCSTEP );
	tos = push( stack, (ppsym*)NULL,
			(pplrstate*)plist_access(
				plist_first( states ) ) );

	*end = start;

	do
	{
		fprintf( stderr, "State on Top %d\n",
					plist_offset( plist_get_by_ptr( states, tos->state ) ) );
		fprintf( stderr, "BEFORE >%s<\n", *end );

		/* Action table processing */
		if( !get_action( &shift, &reduce, tos, end ) )
		{
			/* Parse Error */
			/* TODO: Recovery */
			fprintf( stderr, "PARSE ERROR\n" );
			return FALSE;
		}

		fprintf( stderr, "AFTER  >%s<\n", *end );

		/* Shift */
		if( shift )
		{
			if( reduce )
				fprintf( stderr,
					"shift/reduce by production %d\n", reduce->id );
			else
				fprintf( stderr,
					"shift to state %d\n",
						plist_offset( plist_get_by_ptr( states, shift ) ) );

			tos = push( stack, (ppsym*)NULL,
							reduce ? (pplrstate*)NULL : shift );
		}

		/* Reduce */
		while( reduce )
		{
			fprintf( stderr,
				"reduce by production %d\n"
				"popping %d items off the stack, replacing by '%s'\n",
				reduce->id,
				plist_count( reduce->rhs ),
				reduce->lhs->name );

			tos = pop( stack, plist_count( reduce->rhs ) );

			/* Goal symbol reduced? */
			if( reduce->lhs == grm->goal && pstack_count( stack ) == 1 )
				break;

			lhs = reduce->lhs;
			get_goto( &shift, &reduce, tos, reduce->lhs );
			tos = push( stack, lhs, shift );
		}
	}
	while( !reduce );

	return TRUE;
}

pboolean pp_lr_parse( plist* ast, ppgram* grm, char* start, char** end )
{
	pboolean	ret;
	pboolean	myast		= FALSE;
	plist*		states;

	if( !( grm && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	states = pp_parser_lr_closure( grm, TRUE );
	pp_lrstates_print( states );
	ret = pp_lr_PARSE( (plist*)NULL, grm, start, end, states );

	/*
	if( !ast )
	{
		ast = plist_create( sizeof( ppmatch ), PLIST_MOD_RECYCLE );
		myast = TRUE;
	}
	*/
	pp_lrstates_free( states );

	return ret;
}
