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
#define PPLR_SHIFT		1
#define PPLR_REDUCE		2

#define DEBUGLEVEL		0
#define ALGORITHM_TRACE	0

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
	int				state;			/* State */

	char*			start;			/* Begin of match */
	char*			end;			/* End of match */

	ppast*			node;			/* AST construction */

	int				row;			/* Positioning in source */
	int				col;			/* Positioning in source */
} pplrse;

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

/* Debug for lookahead */
static void pp_lritem_lookahead_print( plist* list )
{
	plistel*	e;
	ppsym*		s;

	fprintf( stderr, "[[" );

	plist_for( list, e )
		fprintf( stderr, " %s", pp_sym_to_str( (ppsym*)plist_access( e ) ) );

	fprintf( stderr, " ]]\n" );
}

/* Priority sort function for the lookahead-sets */
static int pp_lritem_lookahead_sort( plist* list, plistel* el, plistel* er )
{
	ppsym*	l	= (ppsym*)plist_access( el );
	ppsym*	r	= (ppsym*)plist_access( er );

	/* By id order */
	if( l->id < r->id )
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
	int			id;
	plistel*	e;
	pplrstate*	state;

	if( !( states ) )
	{
		WRONGPARAM;
		return (pplrstate*)NULL;
	}

	id = plist_count( states );
	state = plist_malloc( states );

	state->id = id;

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

	fprintf( stderr, "\n-- State %d %p --\n", st->id, st );

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
						col->symbol->name, col->shift->id );
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
						col->shift->id,
							col->symbol->name );
		else
			MISSINGCASE;
	}
}

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

static unsigned int pp_lr_to_dfa( unsigned int*** ret, plist* states )
{
	pplrstate*		st;
	pplrcolumn*		col;
	int				cnt;
	int				i;
	int				j;
	plistel*		e;
	plistel*		f;
	unsigned int**	dfa;

	PROC( "pp_lr_to_dfa" );
	PARMS( "dfa", "%p", dfa );
	PARMS( "states", "%p", states );

	if( !states )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	/* Allocate and fill return tables, free states */
	MSG( "Filling return array" );

	dfa = (unsigned int**)pmalloc( plist_count( states ) * sizeof( int* ) );

	for( i = 0, e = plist_first( states ); e; e = plist_next( e ), i++ )
	{
		st = (pplrstate*)plist_access( e );
		VARS( "State", "%d", i );

		cnt = plist_count( st->actions ) * 3
				+ plist_count( st->gotos ) * 3
					+ 2;

		dfa[ i ] = (unsigned int*)pmalloc( cnt * sizeof( int ) );
		dfa[ i ][ 0 ] = cnt;
		dfa[ i ][ 1 ] = st->def_prod ? st->def_prod->id : 0;

		/* Actions */
		for( j = 2, f = plist_first( st->actions );
				f; f = plist_next( f ), j += 3 )
		{
			col = (pplrcolumn*)plist_access( f );

			dfa[ i ][ j ] = col->symbol->id;

			if( col->shift )
				dfa[ i ][ j + 1 ] = PPLR_SHIFT;

			if( col->reduce )
			{
				dfa[ i ][ j + 1 ] |= PPLR_REDUCE;
				dfa[ i ][ j + 2 ] = col->reduce->id;
			}
			else
				dfa[ i ][ j + 2 ] = col->shift->id;
		}

		/* Gotos */
		for( f = plist_first( st->gotos );
				f; f = plist_next( f ), j += 3 )
		{
			col = (pplrcolumn*)plist_access( f );

			dfa[ i ][ j ] = col->symbol->id;

			if( col->shift )
				dfa[ i ][ j + 1 ] = PPLR_SHIFT;

			if( col->reduce )
			{
				dfa[ i ][ j + 1 ] |= PPLR_REDUCE;
				dfa[ i ][ j + 2 ] = col->reduce->id;
			}
			else
				dfa[ i ][ j + 2 ] = col->shift->id;
		}
	}

	if( !ret )
	{
		/* DEBUG */
		fprintf( stderr, "count = %d\n", plist_count( states ) );

		for( i = 0; i < plist_count( states ); i++ )
		{
			fprintf( stderr, "%02d:", i );

			fprintf( stderr, " def:%02d",  dfa[ i ][ 1 ] );

			for( j = 2; j < dfa[ i ][ 0 ]; j += 3 )
				fprintf( stderr, " %02d:%s%s:%02d",
					dfa[ i ][ j ],
					dfa[ i ][ j + 1 ] & PPLR_SHIFT ? "s" : "-",
					dfa[ i ][ j + 1 ] & PPLR_REDUCE ? "r" : "-",
					dfa[ i ][ j + 2 ] );

			fprintf( stderr, "\n" );
		}

		pfree( dfa );
	}
	else
		*ret = dfa;

	VARS( "states count", "%d", plist_count( states ) );
	RETURN( plist_count( states ) );
}

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

plist* pp_lr_closure( ppgram* gram, pboolean optimize )
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
	pboolean		printed;

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
	it = pp_lritem_create( nst->kernel, pp_sym_getprod( gram->goal, 0 ), 0 );

	plist_push( it->lookahead, gram->eof );

	MSG( "Initializing part and closure lists" );
	part = plist_create( sizeof( pplritem ), PLIST_MOD_RECYCLE );
	closure = plist_create( sizeof( pplritem ), PLIST_MOD_RECYCLE );

	MSG( "Run the closure loop" );
	while( ( st = lr_get_undone( states ) ) )
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

				if( lr_compare( part, nst->kernel ) )
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

	prodcnt = (int*)pmalloc( plist_count( gram->prods ) * sizeof( int ) );

	plist_for( states, e )
	{
		printed = FALSE;
		st = (pplrstate*)plist_access( e );

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

		/* Detect and report conflicts */
		plist_for( st->actions, f )
		{
			col = (pplrcolumn*)plist_access( f );
			sym = (ppsym*)NULL;

			for( g = plist_next( f ); g; g = plist_next( g ) )
			{
				ccol = (pplrcolumn*)plist_access( g );

				if( ccol->symbol == col->symbol )
				{
					if( !printed )
					{
						pp_lrstate_print( (pplrstate*)plist_access( e ) );
						printed = TRUE;
					}

					/* TODO Conflict resolution */
					fprintf( stderr,
						"State %d encouters %s/reduce-conflict on %s\n",
							plist_offset( e ),
								col->reduce ? "reduce" : "shift",
									col->symbol->name );
				}
			}
		}

		/* Detect default prods */
		memset( prodcnt, 0, plist_count( gram->prods ) * sizeof( int ) );
		cnt = 0;

		plist_for( st->actions, f )
		{
			col = (pplrcolumn*)plist_access( f );

			/* fixme: id as index? */
			if( col->reduce && prodcnt[ col->reduce->id - 1 ]++ > cnt )
			{
				/* fixme: id as index? */
				cnt = prodcnt[ col->reduce->id - 1 ];
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

	MSG( "Finished" );
	VARS( "States generated", "%d", plist_count( states ) );

	RETURN( states );
}

#if DEBUGLEVEL > 2
/* Function to dump the parse stack content */
static void print_stack( char* title, plist* states, parray* stack )
{
	pplrse*	e;
	int		i;

	fprintf( stderr, "STACK DUMP %s\n", title );

	for( i = 0; i < parray_count( stack ); i++ )
	{
		e = (pplrse*)parray_get( stack, i );
		fprintf( stderr, "%02d: %s %d >%.*s<\n",
			i, e->symbol ? e->symbol->name : "(null)",
				e->state ?
					plist_offset( plist_get_by_ptr( states, e->state ) ) : -1,
			e->end - e->start, e->start );
	}
}
#endif


static pboolean pp_lr_PARSE( ppast** root, ppgram* grm, plex* lex,
								char* start, char** end,
									unsigned int** dfa )
{
	int			i;
	int			id;
	int			row		= 1;
	int			col		= 1;
	int			lrow;
	int			lcol;
	char*		lstart;
	ppsym*		lhs;
	ppsym*		sym;
	plistel*	e;
	parray*		stack;
	pplrse		item;
	pplrse*		tos;
	int			shift;
	int			reduce;
	ppprod*		prod;
	pboolean	token;
	ppast*		node;

	*end = start;

	/* Get symbols */
	if( ( start = plex_next( lex, start, &id, end ) ) )
		sym = pp_sym_get( grm, id - 1 ); /* fixme!! */
	else
	{
		id = grm->eof->id;
		sym = grm->eof;
	}

	#if DEBUGLEVEL > 2
	fprintf( stderr, "New symbol: '%s' @ >%.*s<\n",
				sym->name, *end - start, start );
	#endif

	/* Initialize parser */
	stack = parray_create( sizeof( pplrse ), 0 );

	tos = (pplrse*)parray_malloc( stack );
	tos->start = start;
	tos->end = *end;

	do
	{
		/* Parse */
		#if DEBUGLEVEL > 1
		fprintf( stderr, "State %d\n",
					plist_offset( plist_get_by_ptr(
									states, tos->state ) ) );

		/* AST construction debug */
		/*
		{
			int			i;
			pplrse*		x;


			for( i = 0; i < parray_count( stack ); i++ )
			{
				x = parray_get( stack, i );
				fprintf( stderr, "%d: %p\n", i, x->node );
				if( x->node )
					pp_ast_printnew( x->node );
			}

			fprintf( stderr, "---\n" );
			getchar();
		}
		*/
		#endif
		#if DEBUGLEVEL > 2
		fprintf( stderr, "BEFORE >%s<\n", *end );
		#endif

		/* Check for entries in the parse table */
		for( i = 2, shift = -1, reduce = 0; i < dfa[tos->state][0]; i += 3 )
		{
			if( dfa[tos->state][i] == id )
			{
				if( dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
					shift = dfa[tos->state][ i + 2 ];

				if( dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
					reduce = dfa[tos->state][ i + 2 ];

				break;
			}
		}

		if( shift == -1 && reduce == 0 )
		{
			if( !( reduce = dfa[tos->state][1] ) )
			{
				/* Parse Error */
				/* TODO: Error Recovery */
				fprintf( stderr, "Parse Error [line:%d col:%d] @ >%s<\n", row, col, *end );
				return FALSE;
			}
		}

		#if DEBUGLEVEL > 2
		fprintf( stderr, "AFTER  >%s<\n", *end );
		#endif

		/* Shift */
		if( shift >= 0 )
		{
			#if DEBUGLEVEL > 1
			if( reduce )
				fprintf( stderr,
					"shift on %s and reduce by production %d\n",
						sym->name,
						reduce->id );
			else
				fprintf( stderr,
					"shift on %s to state %d\n",
						sym->name,
						plist_offset( plist_get_by_ptr( states, shift ) ) );
			#endif

			tos = (pplrse*)parray_malloc( stack );

			tos->state = reduce ? 0 : shift;
			tos->start = start;
			tos->end = *end;
			tos->row = row;
			tos->col = col;

			/* Shifted symbol becomes AST node? */
			if( root && sym->emit )
				tos->node = pp_ast_create( sym->emit, sym,
											(ppprod*)NULL, start, *end,
												row, col, (ppast*)NULL );

			start = *end;

			if( ( start = plex_next( lex, start, &id, end ) ) )
				sym = pp_sym_get( grm, id - 1 ); /* fixme!! */
			else
			{
				id = grm->eof->id;
				sym = grm->eof;
			}

			#if DEBUGLEVEL > 2
			fprintf( stderr, "New symbol: '%s' @ >%.*s<\n",
						sym->name, *end - start, start );
			#endif
		}

		/* Update current position */
		/* fixme
		pp_pos_in_input( &row, &col, start, *end );
		*/

		/* Reduce */
		while( reduce )
		{
			#if DEBUGLEVEL > 1
			fprintf( stderr,
				"reduce by production '%s'\n"
				"popping %d items off the stack, replacing by %s\n",
				pp_prod_to_str( reduce ),
				plist_count( reduce->rhs ),
				reduce->lhs->name );
			#endif

			#if DEBUGLEVEL > 2
			print_stack( "Before Reduce", states, stack );
			#endif

			prod = pp_prod_get( grm, reduce - 1 ); /* fixme!! */
			node = (ppast*)NULL;

			for( i = 0; i < plist_count( prod->rhs ); i++ )
			{
				tos = (pplrse*)parray_pop( stack );

				lstart = tos->start;

				/* Connecting nodes, remember last node. */
				if( tos->node )
				{
					if( node )
					{
						while( tos->node->next )
							tos->node = tos->node->next;

						tos->node->next = node;
						node->prev = tos->node;
					}

					node = tos->node;

					while( node->prev )
						node = node->prev;

					lrow = tos->row;
					lcol = tos->col;
				}
			}

			tos = (pplrse*)parray_last( stack );

			/* Construction of AST node */
			if( root && prod->emit )
				node = pp_ast_create( prod->emit,
										prod->lhs, prod, lstart, *end,
											lrow, lcol, node );
			else if( root && prod->lhs->emit )
				node = pp_ast_create( prod->lhs->emit,
										prod->lhs, prod, lstart, *end,
											lrow, lcol, node );

			/* Goal symbol reduced? */
 			if( prod->lhs == grm->goal && parray_count( stack ) == 1 )
			{
				if( root )
					*root = node;

				break;
			}

			/* Check for entries in the parse table */
			for( i = dfa[tos->state][0] - 3, shift = -1, reduce = 0;
					i >= 2; i -= 3 )
			{
				if( dfa[tos->state][i] == prod->lhs->id )
				{
					if( dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = dfa[tos->state][ i + 2 ];

					if( dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						reduce = dfa[tos->state][ i + 2 ];

					break;
				}
			}

			tos = (pplrse*)parray_malloc( stack );

			tos->state = shift;
			tos->start = lstart;
			tos->end = *end;
			tos->node = node;
			tos->row = lrow;
			tos->col = lcol;

			#if DEBUGLEVEL > 2
			print_stack( "Behind Reduce", states, stack );
			#endif
		}

		#if 0
		/* Skip over whitespace */
		if( !( tos->symbol->flags & PPFLAG_LEXEM ) )
		{
			#if DEBUGLEVEL > 1
			fprintf( stderr, "tos->symbol >%s< is not LEXEM\n",
						tos->symbol->name );
			#endif

			/*
			if( pp_white_in_input( grm, ( lstart = *end ), end ) )
				pp_pos_in_input( &row, &col, lstart, *end );
			*/
		}
		#endif
	}
	while( !reduce );

	return TRUE;
}

/** Parses the string //str// using the grammar //grm// with a LALR(1) parser.

Parsing stops at least when reading the zero terminator of //str//.

//root// receives the root-node of the constructed abstract syntax tree as
an ppast object.

//end// receives the position of the last character matched.

The function returns TRUE if no parse error occured. */
pboolean pp_lr_parse( ppast** root, ppgram* grm, plex* lex,
						char* start, char** end )
{
	unsigned int	cnt;
	unsigned int**	dfa;
	pboolean		ret;
	plist*			states;

	if( !( grm && lex && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Compute LALR(1) states */
	states = pp_lr_closure( grm, TRUE );

	#if DEBUGLEVEL > 0
	/* Dump states */
	pp_lr_print( states );
	#endif

	/* Convert to DFA */
	/* pp_lr_to_dfa( NULL, states ); */ /* DEBUG */
	pp_lr_to_dfa( &dfa, states );

	/* Clean-up */
	pp_lr_free( states );

	/* Run parser */
	ret = pp_lr_PARSE( root, grm, lex, start, end, dfa );

	pfree( dfa );

	return ret;
}
