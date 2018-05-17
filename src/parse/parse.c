/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.c
Usage:	Parser maintainance object.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Creates a new parser object using the underlying grammar //g//.

The grammar must either be parsed first via one of the BNF parsers
(pp_gram_from_pbnf(), pp_gram_from_ebnf(), pp_gram_from_bnf()) or it
must be hand-crafted.

The provided grammar gets "frozen" when a parser is created from it.
Modification of the grammar with a parser based on an older grammar state may
cause memory corruption and crashes.

The function returns a valid parser object on success, or (pppar*)NULL in case
the grammar is invalid. */
pppar* pp_par_create( ppgram* g )
{
	pppar*	p;

	PROC( "pp_par_create" );
	PARMS( "g", "%p", g );

	if( !g )
	{
		WRONGPARAM;
		RETURN( (pppar*)NULL );
	}

	if( !pp_gram_prepare( g ) )
	{
		MSG( "The provided grammar object is invalid" );
		RETURN( (pppar*)NULL );
	}

	p = (pppar*)pmalloc( sizeof( pppar ) );

	/* Grammar */
	p->gram = g;
	g->flags |= PPFLAG_FROZEN;

	if( !pp_lr_build( &p->states, &p->dfa, p->gram ) )
	{
		MSG( "Bulding parse tables failed" );

		pp_par_free( p );
		RETURN( (pppar*)NULL );
	}

	PP_GRAM_DUMP( g );
	RETURN( p );
}

/** Frees the parser object //par//. */
pppar* pp_par_free( pppar* par )
{
	if( !par )
		return (pppar*)NULL;

	parray_free( par->stack );
	pp_ast_free( par->ast );

	pfree( par );

	return (pppar*)NULL;
}

/** Automatically generate lexical analyzer from terminal symbols
defined in the grammar.

The lexical analyzer is constructed as a plex scanner and must be freed
after usage with plex_free(). */
plex* pp_par_autolex( pppar* p )
{
	ppsym*		sym;
	plistel*	e;
	plex*		lex				= (plex*)NULL;

	PROC( "pp_par_autolex" );
	PARMS( "p", "%p", p );

	if( !( p ) )
	{
		WRONGPARAM;
		RETURN( 0 );
	}

	plist_for( p->gram->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		if( !PPSYM_IS_TERMINAL( sym )
			|| sym->flags & PPFLAG_SPECIAL
				|| !sym->name )
			continue;

		VARS( "sym->idx", "%d", sym->idx );

		if( !lex )
		{
			MSG( "OK, its time to create a lexer" );
			lex = plex_create( PREGEX_FLAG_NONE );
		}

		if( sym->ptn )
		{
			VARS( "sym->ptn", "%p", sym->ptn );
			plex_define( lex, (char*)sym->ptn, (int)sym->idx, PREGEX_COMP_PTN );
		}
		else
		{
			VARS( "sym->name", "%s", sym->name ? sym->name : "(null)" );
			plex_define( lex, sym->name, (int)sym->idx, PREGEX_COMP_STATIC );
		}
	}

	VARS( "lex", "%p", lex );
	RETURN( lex );
}


/* LR-Stackitem */
typedef struct
{
	int				state;			/* State */
	ppsym*			sym;			/* Symbol */

	char*			start;			/* Start */

	ppast*			node;			/* AST construction */

	int				row;			/* Positioning in source */
	int				col;			/* Positioning in source */
} pplrse;

#if 0
/* Function to dump the parse stack content */
static void print_stack( char* title, parray* stack )
{
	pplrse*	e;
	int		i;

	fprintf( stderr, "STACK DUMP %s\n", title );

	for( i = 0; i < parray_count( stack ); i++ )
	{
		e = (pplrse*)parray_get( stack, i );
		fprintf( stderr, "%02d: %s %d >%.*s<\n",
			i, e->sym->name, e->state
				e->end - e->start, e->start );
	}
}
#endif


/** Let parser //par// run on next token //sym//.

//val// is an optional parameter that holds a semantic value. It will be
assigned to an AST tree node when provided. Set it NULL if the value is not
used or required.

This method is called a push-parsing algorithm, where the scanner calls the
parser to perform the next parsing steps. The used parsing method is LALR(1).

The function returns one of the following values:

- PPPAR_STATE_DONE when the parse succeeded,
- PPPAR_STATE_NEXT when the partial parse succeeded, and next token is wanted,
- PPPAR_STATE_ERROR when an error was encountered.
-
*/
ppparstate pp_par_next( pppar* par, ppsym* sym, pany* val )
{
	int			i;
	pplrse*		tos;
	int			shift;
	ppprod*		prod;
	ppast*		node;

	PROC( "pp_par_next" );

	if( !( par && sym ) )
	{
		WRONGPARAM;
		RETURN( PPPAR_STATE_ERROR );
	}

	PARMS( "par", "%p", par );
	PARMS( "sym", "%p", sym );

	if( val )
		PANY_DUMP( val );

	/* Set up stack if necessary */
	VARS( "par->stack", "%p", par->stack );

	if( !par->stack )
	{
		MSG( "Setting up new stack" );
		par->stack = parray_create( sizeof( pplrse ), 0 );
		par->state = PPPAR_STATE_INITIAL;
	}

	/* Initialize parser on first call */
	if( par->state == PPPAR_STATE_INITIAL )
	{
		MSG( "Initial call" );
		parray_erase( par->stack );
		par->ast = pp_ast_free( par->ast );

		tos = (pplrse*)parray_malloc( par->stack );
		tos->sym = par->gram->goal;
	}
	else
		tos = (pplrse*)parray_last( par->stack );

	/* Until all reductions are performed... */
	do
	{
		/* Reduce */
		while( par->reduce )
		{
			VARS( "par->reduce", "%d", par->reduce );

			prod = pp_prod_get( par->gram, par->reduce - 1 );

			LOG( "reduce by production '%s'", pp_prod_to_str( prod ) );
			LOG( "popping %d items off the stack, replacing by %s\n",
						plist_count( prod->rhs ),
							prod->lhs->name );

			node = (ppast*)NULL;

			for( i = 0; i < plist_count( prod->rhs ); i++ )
			{
				tos = (pplrse*)parray_pop( par->stack );

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
				}
			}

			tos = (pplrse*)parray_last( par->stack );

			/* Construction of AST node */
			if( prod->emit )
				node = pp_ast_create( prod->emit, prod->lhs, prod, node );
			else if( prod->lhs->emit )
				node = pp_ast_create( prod->lhs->emit, prod->lhs, prod, node );

			/* Goal symbol reduced? */
			if( prod->lhs == par->gram->goal
					&& parray_count( par->stack ) == 1 )
			{
				MSG( "Parsing succeeded!" );

				par->ast = node;

				RETURN( ( par->state = PPPAR_STATE_DONE ) );
			}

			/* Check for entries in the parse table */
			for( i = par->dfa[tos->state][0] - 3, shift = 0, par->reduce = 0;
					i >= 2; i -= 3 )
			{
				if( par->dfa[tos->state][i] == prod->lhs->idx + 1 )
				{
					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = par->dfa[tos->state][ i + 2 ];

					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						par->reduce = par->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			tos = (pplrse*)parray_malloc( par->stack );

			tos->sym = prod->lhs;
			tos->state = shift - 1;
			tos->node = node;

			LOG( "New top state is %d", tos->state );
		}

		VARS( "State", "%d", tos->state );

		/* Check for entries in the parse table */
		if( tos->state > -1 )
		{
			for( i = 2, shift = 0, par->reduce = 0;
					i < par->dfa[tos->state][0]; i += 3 )
			{
				if( par->dfa[tos->state][i] == sym->idx + 1 )
				{
					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = par->dfa[tos->state][ i + 2 ];

					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						par->reduce = par->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			if( !shift && !par->reduce )
				par->reduce = par->dfa[ tos->state ][ 1 ];
		}

		VARS( "shift", "%d", shift );
		VARS( "par->reduce", "%d", par->reduce );

		if( !shift && !par->reduce )
		{
			/* Parse Error */
			/* TODO: Error Recovery */
			fprintf( stderr, "Parse Error @ %s\n", sym->name );

			MSG( "Parsing failed" );
			RETURN( ( par->state = PPPAR_STATE_ERROR ) );
		}
	}
	while( !shift && par->reduce );

	if( par->reduce )
		LOG( "shift on %s and reduce by production %d\n",
					sym->name, par->reduce - 1 );
	else
		LOG( "shift on %s to state %d\n", sym->name, shift - 1 );

	tos = (pplrse*)parray_malloc( par->stack );

	tos->sym = sym;
	tos->state = par->reduce ? 0 : shift - 1;

	/* Shifted symbol becomes AST node? */
	if( sym->emit )
	{
		tos->node = pp_ast_create( sym->emit ? sym->emit : sym->name, sym,
									(ppprod*)NULL, (ppast*)NULL );

		if( val && val->type == PANYTYPE_STR )
		{
			tos->node->val = val;

			tos->node->start = pany_get_str( val );
			tos->node->len = strlen( tos->node->start );
			tos->node->end = tos->node->start + tos->node->len;
		}
	}

	MSG( "Next token required" );
	RETURN( ( par->state = PPPAR_STATE_NEXT ) );
}

/** Let parser //par// run on next token identified by //name//.

The function is a wrapper for pp_par_next() with same behavior.
*/
ppparstate pp_par_next_by_name( pppar* par, char* name, pany* val )
{
	ppsym*		sym;

	PROC( "pp_par_next_by_name" );
	PARMS( "par", "%p", par );
	PARMS( "name", "%s", name );

	if( val )
		PANY_DUMP( val );

	if( !( par && name && *name ) )
	{
		WRONGPARAM;
		RETURN( PPPAR_STATE_ERROR );
	}

	if( !( sym = pp_sym_get_by_name( par->gram, name ) ) )
	{
		WRONGPARAM;

		LOG( "Token named '%s' does not exist in the grammar", name );
		RETURN( PPPAR_STATE_ERROR );
	}

	if( !PPSYM_IS_TERMINAL( sym ) )
	{
		WRONGPARAM;

		LOG( "Symbol named '%s' is not a terminal symbol", name );
		RETURN( PPPAR_STATE_ERROR );
	}

	RETURN( pp_par_next( par, sym, val ) );
}


/** Let parser //par// run on next token identified by //idx//.

The function is a wrapper for pp_par_next() with same behavior.
*/
ppparstate pp_par_next_by_idx( pppar* par, unsigned int idx, pany* val )
{
	ppsym*		sym;

	PROC( "pp_par_next_by_idx" );
	PARMS( "par", "%p", par );
	PARMS( "idx", "%d", idx );

	if( val )
		PANY_DUMP( val );

	if( !( par ) )
	{
		WRONGPARAM;
		RETURN( PPPAR_STATE_ERROR );
	}

	if( !( sym = pp_sym_get( par->gram, idx ) ) )
	{
		WRONGPARAM;

		LOG( "Token with index %d does not exist in the grammar", idx );
		RETURN( PPPAR_STATE_ERROR );
	}

	LOG( "Token = %s", sym->name );

	if( !PPSYM_IS_TERMINAL( sym ) )
	{
		WRONGPARAM;

		LOG( "Symbol with index %d is not a terminal symbol", idx );
		RETURN( PPPAR_STATE_ERROR );
	}

	RETURN( pp_par_next( par, sym, val ) );
}

/** Parse string with lexer. */
pboolean pp_par_parse( ppast** root, pppar* par, char* start )
{
	plex*			lex;
	char*			end;
	unsigned int	tok;
	pany*			val;

	PROC( "pp_par_parse" );
	PARMS( "root", "%p", root );
	PARMS( "par", "%p", par );

	if( !( par && start ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !( lex = pp_par_autolex( par ) ) )
	{
		MSG( "Unable to create a lexer from this parser" );
		WRONGPARAM;

		RETURN( FALSE );
	}

	while( ( start = plex_next( lex, start, &tok, &end ) ) )
	{
		val = pany_create( (char*)NULL );
		pany_set_strndup( val, start, end - start );

		LOG( "token = %d", tok );
		LOG( "match = >%s<", pany_get_str( val ) );

		if( pp_par_next_by_idx( par, tok, val ) != PPPAR_STATE_NEXT )
			break;

		start = end;
	}

	plex_free( lex );

	if( !start && pp_par_next_by_idx( par, 0, (pany*)NULL )
					== PPPAR_STATE_DONE )
	{
		MSG( "We have a successful parse!" );
		if( root )
		{
			*root = par->ast;
			par->ast = (ppast*)NULL;
		}

		RETURN( TRUE );
	}

	RETURN( FALSE );
}
