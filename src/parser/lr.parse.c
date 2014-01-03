/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.parse.c
Usage:	Parse LR/LALR parser
----------------------------------------------------------------------------- */

#include "local.h"

/* Stack Element */
typedef struct
{
	pglrstate*		state;	/* State */
	pgtoken*		token;	/* Token */
} pglrse;

/* Parser Command Block */
typedef struct
{
	pgparser*		p;		/* Parser */
	pggrammar*		g;		/* Grammar */

	pstack*			st;		/* Stack */
	pglrse*			tos;	/* Top of stack */

	pgtoken*		la;		/* Current look ahead */

	/* State */
	pglrstate*		shift;	/* Shift to state */
	pgproduction*	reduce;	/* Reduce by production */
	pgsymbol*		lhs;	/* Default left-hand side */
}
pglrpcb;

/* Push state on stack */
static pboolean push( pglrpcb* pcb, pglrstate* state, pgtoken* token )
{
	pglrse	e;

	e.state = state;
	e.token = token;

	if( !( pcb->tos = (pglrse*)pstack_push( pcb->st, &e ) ) )
		return FALSE;

	return TRUE;
}

/* Pop elements off the stack */
static pboolean pop( pglrpcb* pcb, int n )
{
	int		i;

	for( i = 0; i < n; i++ )
		pstack_pop( pcb->st );

	pcb->tos = (pglrse*)pstack_top( pcb->st );

	return TRUE;
}

/* Get action on current look-ahead */
static pboolean get_action( pglrpcb* pcb, pgsymbol* sym )
{
	pglrcolumn*	col;
	plistel*	e;

	pcb->shift = (pglrstate*)NULL;
	pcb->reduce = (pgproduction*)NULL;

	plist_for( pcb->tos->state->actions, e )
	{
		col = (pglrcolumn*)plist_access( e );

		if( col->symbol == sym )
		{
			if( !( pcb->shift = col->shift ) &&
					!( pcb->reduce = col->reduce ) )
				return FALSE; /* Forced parse error! */

			return TRUE;
		}
	}

	/* If no entry was found, reduce by default production */
	if( ( pcb->reduce = pcb->tos->state->def_prod ) )
		return TRUE;

	return FALSE;
}

/* Get goto on current left-hand side */
static pboolean get_goto( pglrpcb* pcb )
{
	pglrcolumn*	col;
	plistel*	e;

	pcb->shift = (pglrstate*)NULL;
	pcb->reduce = (pgproduction*)NULL;

	plist_for( pcb->tos->state->gotos, e )
	{
		col = (pglrcolumn*)plist_access( e );

		if( col->symbol == pcb->lhs )
		{
			pcb->shift = col->shift;
			pcb->reduce = col->reduce;

			return TRUE;
		}
	}

	return FALSE;
}


pboolean pg_parser_lr_eval( pgparser* parser, char* input )
{
	pglrpcb		PCB;
	pglrpcb*	pcb = &PCB;
	pgtoken		tok;

	PROC( "pg_parser_eval" );

	if( !pg_parser_is_lr( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	/* Initialize parser control block */
	memset( pcb, 0, sizeof( pglrpcb ) );

	pcb->p = parser;
	pcb->g = pg_parser_get_grammar( pcb->p );
	pcb->st = pstack_create( sizeof( pglrse ), 64 );

	memset( &tok, 0, sizeof( pgtoken ) );
	tok.symbol = pg_grammar_get_goal( pcb->g );
	push( pcb, 0, &tok );

	do
	{
		/* TODO: Read lookahead */

		if( !get_action( pcb, pcb->la->symbol ) )
		{
			/* TODO: Error recovery */
		}

		if( pcb->shift )
		{
			push( pcb, pcb->reduce ? (pglrstate*)NULL : pcb->shift, pcb->la );
			/* TODO: Get next token */
		}

		while( pcb->reduce )
		{
			pcb->lhs = pg_production_get_lhs( pcb->reduce );

			pop( pcb, pg_production_get_rhs_length( pcb->reduce ) );

			/* Goal symbol reduced? */
			if( pcb->lhs == pg_grammar_get_goal( pcb->g )
					&& pstack_count( pcb->st ) == 1 )
				break;

			get_goto( pcb );

			tok.symbol = pcb->lhs;
			push( pcb, pcb->shift, &tok );
		}
	}
	while( !pcb->reduce ); /* Break on goal */

}
