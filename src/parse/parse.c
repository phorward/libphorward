/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.c
Usage:	Parser maintainance object.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Creates a new parser object with the underlying grammar //g//.

The grammar must either be parsed first via one of the BNF parsers
(pp_gram_from_pbnf(), pp_gram_from_ebnf(), pp_gram_from_bnf()) or hand-crafted.

The provided grammar gets "frozen" when a parser is created from it.
Modifications on the grammar with a parser based on an older grammar state may
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

	/* Lexer token mappings */
	p->ntokens = p->tokens = (ppsym**)pmalloc(
								( plist_count( g->symbols ) + 1 )
									* sizeof( ppsym* ) );
	g->flags |= PPFLAG_FROZEN;

	/* Grammar */
	p->gram = g;

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
pppar* pp_par_free( pppar* p )
{
	if( !p )
		return (pppar*)NULL;

	plex_free( p->lex );

	pfree( p->tokens );
	pfree( p );

	return (pppar*)NULL;
}

/** Automatically generate lexical analysis for terminal symbols which
have not been defined with pp_par_lex().

Returns the number of inserted tokens on success. */
int pp_par_autolex( pppar* p )
{
	ppsym**		ptr;
	ppsym*		sym;
	plistel*	e;
	int			gen 		= 0;

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

		/* Check if symbol was not already defined previously */
		for( ptr = p->tokens; ptr < p->ntokens; ptr++ )
			if( sym == *ptr )
				break;

		if( ptr == p->ntokens )
		{
			MSG( "Creating auto lexer" );
			VARS( "sym->name", "%s", sym->name ? sym->name : "(null)" );
			VARS( "sym->ptn", "%p", sym->ptn );

			if( sym->ptn )
				pp_par_lex( p, sym, (char*)sym->ptn, PREGEX_COMP_PTN );
			else
				pp_par_lex( p, sym, sym->name, PREGEX_COMP_STATIC );

			gen++;
		}
	}

	VARS( "gen", "%d", gen );
	RETURN( gen );
}

/** Defines a lexical matcher for //sym// in parser //p// from a
regular expression. //sym// must be a terminal symbol.

Returns TRUE on success, FALSE if the symbol already was defined as a lexing
token. */
pboolean pp_par_lex( pppar* p, ppsym* sym, char* pat, int flags )
{
	ppsym**	ptr;

	PROC( "pp_par_lex" );
	PARMS( "p", "%p", p );

	if( !( p && sym && pat ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	PARMS( "sym->name", "%s", sym->name );
	PARMS( "pat", "%s", pat );
	PARMS( "flags", "%d", flags );

	if( !PPSYM_IS_TERMINAL( sym ) )
	{
		fprintf( stderr, "'%s' is not a terminal\n", sym->name );
		MSG( "Cannot create lexical recognition for nonterminal symbols" );
		RETURN( FALSE );
	}

	/* Check if symbol was not already defined previously */
	for( ptr = p->tokens; ptr < p->ntokens; ptr++ )
	{
		if( sym == *ptr )
		{
			MSG( "Symbol was already defined previously" );
			RETURN( FALSE );
		}
	}

	/* Once construct a lexer */
	if( !p->lex )
		p->lex = plex_create( 0 );

	/* Insert symbol */
	*(p->ntokens++) = sym;
	plex_define( p->lex, pat, (int)( p->ntokens - p->tokens ), flags );

	RETURN( TRUE );
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

static ppsym* pp_par_scan( pppar* p, char** start, char** end )
{
	ppsym*			sym;
	unsigned int	id;

	PROC( "pp_par_scan" );

	if( ( *start = plex_next( p->lex, *start, &id, end ) ) )
		sym = p->tokens[ id - 1 ];
	else
		sym = p->gram->eof;

	LOG( "Next token '%s' @ >%.*s<\n", sym->name, *end - *start, *start );
	RETURN( sym );
}

/** Run parser //p// with input //start//.

The function uses the parsing method defined when then parser was created.
//end// receives a pointer to the position where the parser stopped.

It returns an abstract syntax tree to //root// on success. */
pboolean pp_par_parse( ppast** root, pppar* par, char* start )
{
	int			i;
	int			row		= 1;
	int			col		= 1;
	char*		end;
	int			lrow;
	int			lcol;
	char*		lstart;
	char*		lend;
	ppsym*		sym;
	parray*		stack;
	pplrse*		tos;
	int			shift;
	int			reduce;
	ppprod*		prod;
	ppast*		node;

	PROC( "pp_par_parse" );

	if( !( par && start ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	PARMS( "root", "%p", root );
	PARMS( "par", "%p", par );
	PARMS( "start", "%s", start );

	/* Init */
	if( par->lex )
		plex_prepare( par->lex );

	stack = parray_create( sizeof( pplrse ), 0 );

	tos = (pplrse*)parray_malloc( stack );
	tos->sym = par->gram->goal;
	tos->start = start;

	/* Read token */
	lend = end = start;
	sym = pp_par_scan( par, &start, &end );

	do
	{
		VARS( "State", "%d", tos->state );

		/* Check for entries in the parse table */
		if( tos->state > -1 )
		{
			for( i = 2, shift = 0, reduce = 0;
					i < par->dfa[tos->state][0]; i += 3 )
			{
				if( par->dfa[tos->state][i] == sym->idx + 1 )
				{
					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = par->dfa[tos->state][ i + 2 ];

					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						reduce = par->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			if( !shift && !reduce )
				reduce = par->dfa[ tos->state ][ 1 ];
		}

		VARS( "shift", "%d", shift );
		VARS( "reduce", "%d", reduce );

		if( !shift && !reduce )
		{
			/* Parse Error */
			/* TODO: Error Recovery */
			fprintf( stderr, "Parse Error [line:%d col:%d] @ >%s<\n",
				row, col, end );

			MSG( "Parsing failed" );
			RETURN( FALSE );
		}

		/* Shift */
		if( shift )
		{
			if( reduce )
				LOG( "shift on %s and reduce by production %d\n",
							sym->name, reduce - 1 );
			else
				LOG( "shift on %s to state %d\n", sym->name, shift - 1 );

			tos = (pplrse*)parray_malloc( stack );

			tos->sym = sym;
			tos->state = reduce ? 0 : shift - 1;
			tos->start = start;
			tos->row = row;
			tos->col = col;

			/* Shifted symbol becomes AST node? */
			if( root && sym->emit )
			{
				lend = end;
				tos->node = pp_ast_create( sym->emit, sym,
											(ppprod*)NULL, start, end,
												row, col, (ppast*)NULL );
			}

			/* Read next token */
			lend = start = end;
			sym = pp_par_scan( par, &start, &end );
		}

		/* Reduce */
		while( reduce )
		{
			prod = pp_prod_get( par->gram, reduce - 1 );

			LOG( "reduce by production '%s'", pp_prod_to_str( prod ) );
			LOG( "popping %d items off the stack, replacing by %s\n",
						plist_count( prod->rhs ),
							prod->lhs->name );

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
										prod->lhs, prod, lstart, lend,
											lrow, lcol, node );
			else if( root && prod->lhs->emit )
				node = pp_ast_create( prod->lhs->emit,
										prod->lhs, prod, lstart, lend,
											lrow, lcol, node );

			/* Goal symbol reduced? */
 			if( prod->lhs == par->gram->goal && parray_count( stack ) == 1 )
			{
				if( root )
					*root = node;

				break;
			}

			/* Check for entries in the parse table */
			for( i = par->dfa[tos->state][0] - 3, shift = 0, reduce = 0;
					i >= 2; i -= 3 )
			{
				if( par->dfa[tos->state][i] == prod->lhs->idx + 1 )
				{
					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = par->dfa[tos->state][ i + 2 ];

					if( par->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						reduce = par->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			tos = (pplrse*)parray_malloc( stack );

			tos->sym = prod->lhs;
			tos->state = shift - 1;
			tos->start = lstart;
			tos->node = node;
			tos->row = lrow;
			tos->col = lcol;
		}
	}
	while( !reduce );

	MSG( "Parsing succeeded" );
	RETURN( TRUE );
}
