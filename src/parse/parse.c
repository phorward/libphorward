/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.c
Usage:	Parser maintainance object.
----------------------------------------------------------------------------- */

#include "phorward.h"

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
			|| sym->flags & PPFLAG_SPECIAL )
			continue;

		VARS( "sym->name", "%s", sym->name ? sym->name : "(null)" );
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

/** Initializes a parser context //ctx// for parser //par//.

Parser contexts are objects holding state and semantics information on a
current parsing process. */
ppparctx* pp_parctx_init( ppparctx* ctx, pppar* par )
{
	PROC( "pp_parctx_init" );
	PARMS( "ctx", "%p", ctx );
	PARMS( "par", "%p", par );

	if( !( ctx && par ) )
	{
		WRONGPARAM;
		RETURN( (ppparctx*)NULL );
	}

	memset( ctx, 0, sizeof( ppparctx ) );

	ctx->par = par;
	ctx->state = PPPAR_STATE_INITIAL;

	parray_init( &ctx->stack, sizeof( pplrse ), 0 );

	RETURN( ctx );
}

/** Creates a new parser context for parser //par//.

Parser contexts are objects holding state and semantics information on a
current parsing process. */
ppparctx* pp_parctx_create( pppar* par )
{
	ppparctx*	ctx;

	PROC( "pp_parctx_create" );
	PARMS( "par", "%p", par );

	if( !par )
	{
		WRONGPARAM;
		RETURN( (ppparctx*)NULL );
	}

	ctx = (ppparctx*)pmalloc( sizeof( ppparctx ) );
	pp_parctx_init( ctx, par );

	RETURN( ctx );
}

/** Resets the parser context object //ctx//. */
ppparctx* pp_parctx_reset( ppparctx* ctx )
{
	if( !ctx )
		return (ppparctx*)NULL;

	parray_erase( &ctx->stack );
	ctx->ast = pp_ast_free( ctx->ast );

	ctx->state = PPPAR_STATE_INITIAL;

	return ctx;
}

/** Frees the parser context object //ctx//. */
ppparctx* pp_parctx_free( ppparctx* ctx )
{
	if( !ctx )
		return (ppparctx*)NULL;

	pp_parctx_reset( ctx );
	pfree( ctx );

	return (ppparctx*)NULL;
}

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


/** Let parser and context //ctx// run on next token //sym//.

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
ppparstate pp_parctx_next( ppparctx* ctx, ppsym* sym, pany* val )
{
	int			i;
	pplrse*		tos;
	int			shift;
	ppprod*		prod;
	pppar*		par;
	ppast*		node;

	PROC( "pp_parctx_next" );

	if( !( ctx && sym ) )
	{
		WRONGPARAM;
		RETURN( PPPAR_STATE_ERROR );
	}

	PARMS( "ctx", "%p", ctx );
	PARMS( "sym", "%p", sym );

	if( val )
		PANY_DUMP( val );

	par = ctx->par;

	/* Initialize parser on first call */
	if( ctx->state == PPPAR_STATE_INITIAL )
	{
		MSG( "Initial call" );
		pp_parctx_reset( ctx );

		tos = (pplrse*)parray_malloc( &ctx->stack );
		tos->sym = par->gram->goal;
	}
	else
		tos = (pplrse*)parray_last( &ctx->stack );

	/* Until all reductions are performed... */
	do
	{
		/* Reduce */
		while( ctx->reduce )
		{
			VARS( "ctx->reduce", "%d", ctx->reduce );

			prod = pp_prod_get( par->gram, ctx->reduce - 1 );

			LOG( "reduce by production '%s'", pp_prod_to_str( prod ) );
			LOG( "popping %d items off the stack, replacing by %s\n",
						plist_count( prod->rhs ),
							prod->lhs->name );

			if( ctx->reducefn )
			{
				LOG( "Calling reduce function %p", ctx->reducefn );
				(*ctx->reducefn)( ctx, prod );
			}

			node = (ppast*)NULL;

			for( i = 0; i < plist_count( prod->rhs ); i++ )
			{
				tos = (pplrse*)parray_pop( &ctx->stack );

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

			tos = (pplrse*)parray_last( &ctx->stack );

			/* Construction of AST node */
			if( prod->emit )
				node = pp_ast_create( prod->emit, prod->lhs, prod, node );
			else if( prod->lhs->emit )
				node = pp_ast_create( prod->lhs->emit, prod->lhs, prod, node );

			/* Goal symbol reduced? */
			if( prod->lhs == par->gram->goal
					&& parray_count( &ctx->stack ) == 1 )
			{
				MSG( "Parsing succeeded!" );

				ctx->ast = node;

				RETURN( ( ctx->state = PPPAR_STATE_DONE ) );
			}

			/* Check for entries in the parse table */
			for( i = par->dfa[tos->state][0] - 3, shift = 0, ctx->reduce = 0;
					i >= 2; i -= 3 )
			{
				if( par->dfa[tos->state][i] == prod->lhs->idx + 1 )
				{
					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = par->dfa[tos->state][ i + 2 ];

					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						ctx->reduce = par->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			tos = (pplrse*)parray_malloc( &ctx->stack );

			tos->sym = prod->lhs;
			tos->state = shift - 1;
			tos->node = node;

			LOG( "New top state is %d", tos->state );
		}

		VARS( "State", "%d", tos->state );

		/* Check for entries in the parse table */
		if( tos->state > -1 )
		{
			for( i = 2, shift = 0, ctx->reduce = 0;
					i < par->dfa[tos->state][0]; i += 3 )
			{
				if( par->dfa[tos->state][i] == sym->idx + 1 )
				{
					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = par->dfa[tos->state][ i + 2 ];

					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						ctx->reduce = par->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			if( !shift && !ctx->reduce )
				ctx->reduce = par->dfa[ tos->state ][ 1 ];
		}

		VARS( "shift", "%d", shift );
		VARS( "ctx->reduce", "%d", ctx->reduce );

		if( !shift && !ctx->reduce )
		{
			/* Parse Error */
			/* TODO: Error Recovery */
			fprintf( stderr, "Parse Error @ %s\n", sym->name );

			MSG( "Parsing failed" );
			RETURN( ( ctx->state = PPPAR_STATE_ERROR ) );
		}
	}
	while( !shift && ctx->reduce );

	if( ctx->reduce )
		LOG( "shift on %s and reduce by production %d\n",
					sym->name, ctx->reduce - 1 );
	else
		LOG( "shift on %s to state %d\n", sym->name, shift - 1 );

	tos = (pplrse*)parray_malloc( &ctx->stack );

	tos->sym = sym;
	tos->state = ctx->reduce ? 0 : shift - 1;

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
	RETURN( ( ctx->state = PPPAR_STATE_NEXT ) );
}

/** Let parser //ctx// run on next token identified by //name//.

The function is a wrapper for pp_parctx_next() with same behavior.
*/
ppparstate pp_parctx_next_by_name( ppparctx* ctx, char* name, pany* val )
{
	ppsym*		sym;

	PROC( "pp_parctx_next_by_name" );
	PARMS( "ctx", "%p", ctx );
	PARMS( "name", "%s", name );

	if( val )
		PANY_DUMP( val );

	if( !( ctx && name && *name ) )
	{
		WRONGPARAM;
		RETURN( PPPAR_STATE_ERROR );
	}

	if( !( sym = pp_sym_get_by_name( ctx->par->gram, name ) ) )
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

	RETURN( pp_parctx_next( ctx, sym, val ) );
}


/** Let parser //ctx// run on next token identified by //idx//.

The function is a wrapper for pp_parctx_next() with same behavior.
*/
ppparstate pp_parctx_next_by_idx( ppparctx* ctx, unsigned int idx, pany* val )
{
	ppsym*		sym;

	PROC( "pp_parctx_next_by_idx" );
	PARMS( "ctx", "%p", ctx );
	PARMS( "idx", "%d", idx );

	if( val )
		PANY_DUMP( val );

	if( !( ctx ) )
	{
		WRONGPARAM;
		RETURN( PPPAR_STATE_ERROR );
	}

	if( !( sym = pp_sym_get( ctx->par->gram, idx ) ) )
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

	RETURN( pp_parctx_next( ctx, sym, val ) );
}

/* Helper for pp_par_parse() */
static ppsym* pp_par_scan( pppar* par, plex* lex,
							char** start, char** end, pboolean lazy )
{
	unsigned int	tok;
	ppsym*			sym;

	PROC( "pp_par_scan" );

	while( TRUE )
	{
		if( ( !lazy && ( tok = plex_lex( lex, *start, end ) ) )
			|| ( lazy && ( *start = plex_next( lex, *start, &tok, end ) ) ) )
		{
			VARS( "tok", "%d", tok );

			if( !( sym = pp_sym_get( par->gram, tok ) )
				|| sym->flags & PPFLAG_WHITESPACE )
			{
				*start = *end;
				continue;
			}
		}
		else
			sym	= par->gram->eof;

		break;
	}

	if( sym )
		LOG( "Next token '%s' @ >%.*s<\n", sym->name, *end - *start, *start );

	RETURN( sym );
}

/** Parse //start// with parser //par//.

If parsing succeeds, the function returns TRUE. And ppast-pointer is then
saved to //root// if provided. */
pboolean pp_par_parse( ppast** root, pppar* par, char* start )
{
	plex*			lex;
	char*			end;
	pany*			val;
	pboolean		lazy	= TRUE;
	pboolean		ret		= FALSE;
	unsigned int	i;
	ppsym*			sym;
	ppparctx		ctx;

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

	for( i = 0; ( sym = pp_sym_get( par->gram, i ) ); i++ )
	{
		if( PPSYM_IS_TERMINAL( sym ) && sym->flags & PPFLAG_WHITESPACE )
		{
			lazy = FALSE;
			break;
		}
	}

	VARS( "lazy", "%s", BOOLEAN_STR( lazy ) );

	pp_parctx_init( &ctx, par );

	do
	{
		sym = pp_par_scan( par, lex, &start, &end, lazy );

		LOG( "symbol %d, %s", sym->idx, sym->name );

		if( end > start )
		{
			val = pany_create( (char*)NULL );
			pany_set_strndup( val, start, end - start );

			LOG( "val = >%s<", pany_get_str( val ) );
		}
		else
			val = (pany*)NULL;

		switch( pp_parctx_next( &ctx, sym, val ) )
		{
			case PPPAR_STATE_NEXT:
				MSG( "Next symbol requested" );
				start = end;
				break;

			case PPPAR_STATE_DONE:
				MSG( "We have a successful parse!" );

				if( root )
				{
					*root = ctx.ast;
					ctx.ast = (ppast*)NULL;
				}

				sym = (ppsym*)NULL;
				ret = TRUE;

				break;

			default:
				sym = (ppsym*)NULL;
				break;
		}
	}
	while( sym );

	pp_parctx_reset( &ctx );
	plex_free( lex );

	RETURN( ret );
}
