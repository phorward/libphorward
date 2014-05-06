/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.parse.c
Usage:	Parse LR/LALR parser
----------------------------------------------------------------------------- */

#include "phorward.h"
#include "lr.h"

/* Stack Element */
typedef struct
{
	pgsymbol*		symbol;	/* Symbol */

	pglrstate*		state;	/* State */
	pgtoken*		token;	/* Token */

	pgastnode*		node;	/* Node */
} pglrse;

/* Parser Command Block */
typedef struct
{
	pgparser*		p;		/* Parser */
	pggrammar*		g;		/* Grammar */
	pgast*			ast;	/* AST */

	pstack*			st;		/* Stack */
	pglrse*			tos;	/* Top of stack */

	pgtoken*		la;		/* Current look ahead */

	/* State */
	pglrstate*		shift;	/* Shift to state */
	pgprod*			reduce;	/* Reduce by production */
	pgsymbol*		lhs;	/* Default left-hand side */
}
pglrpcb;

static void print_stack( pglrpcb* pcb )
{
	int		i;
	pglrse*	e;

	for( i = 0; i < pstack_count( pcb->st ); i++ )
	{
		e = (pglrse*)pstack_access( pcb->st, i );

		fprintf( stderr, "%02d: sym: '%s' state: %d "
							"token: %s(%s) ast: %s/%s\n",
				i,
				e->symbol ? pg_symbol_get_name( e->symbol ) : "(X)",
				e->state ? plist_offset( plist_get_by_ptr(
								pcb->p->states, e->state ) ) : -1,
				/* Token */
				e->token ?
					pg_symbol_get_name( pg_token_get_symbol( e->token ) )
						: "(X)",
				e->token ?
					pg_value_get_string( pg_token_get_lexem( e->token ) )
						: "(X)",
				/* AST */
				e->node && e->node->symbol ?
					pg_symbol_get_name( e->node->symbol ) : "(X)",
				e->node && e->node->token ?
					pg_value_get_string(
						pg_token_get_lexem( e->node->token ) ): "(X)"

					) ;
	}

}

/*
static void traverse_ast( pgastnode* node )
{
	pgastnode*	child;
	char*		name;

	while( node )
	{
		if( node->child )
		{
			if( node->type )
				pg_asttype_call_topdown( node->type, node );

			traverse_ast( node->child );

			if( node->type )
				pg_asttype_call_bottomup( node->type, node );
		}

		if( node->token )
			fprintf( stderr, "PUSH %s = >%s< w>%ls<\n",
				pg_symbol_get_name( node->symbol ),
					pg_value_get_string( pg_token_get_lexem( node->token ) ) );

		if( node->type )
			pg_asttype_call_passover( node->type, node );

		node = node->next;
	}
}
*/

/* Push state on stack */
static pboolean push( pglrpcb* pcb, pgsymbol* sym, pglrstate* st, pgtoken* tok )
{
	pglrse		e;
	pgastnode*	node;

	memset( &e, 0, sizeof( pglrse ) );

	e.state = st;

	if( ( e.token = tok ) )
		e.symbol = pg_token_get_symbol( tok );
	else
		e.symbol = sym;

	/* AST */
	if( pcb->ast &&
			/* TEST TEST TEST */
			( pg_symbol_is_term( e.symbol )
					&& ( isupper( *pg_symbol_get_name( e.symbol ) )
						|| *pg_symbol_get_name( e.symbol ) == '@' ) ) )
	{
		e.node = pg_astnode_create( e.symbol );
		pg_astnode_set_token( e.node, tok );
	}

	if( !( pcb->tos = (pglrse*)pstack_push( pcb->st, &e ) ) )
		return FALSE;

	/* Debug */
	fprintf( stderr, ">>>\n" );
	print_stack( pcb );

	return TRUE;
}

/* Pop elements off the stack */
static pboolean pop( pglrpcb* pcb, int n, pgastnode** chain )
{
	pglrse*		e;
	pgastnode*	node;

	while( n-- > 0 )
	{
		fprintf( stderr, "<<<\n" );
		e = (pglrse*)pstack_pop( pcb->st );

		if( chain && e->node )
		{
			for( node = e->node; node->next; node = node->next )
				;

			if( ( node->next = *chain ) )
				(*chain)->prev = node;

			*chain = e->node;
		}
	}

	pcb->tos = (pglrse*)pstack_top( pcb->st );
	print_stack( pcb );

	return TRUE;
}

/* Get action on current look-ahead */
static pboolean get_action( pglrpcb* pcb, pgsymbol* sym )
{
	pglrcolumn*	col;
	plistel*	e;

	pcb->shift = (pglrstate*)NULL;
	pcb->reduce = (pgprod*)NULL;

	plist_for( pcb->tos->state->actions, e )
	{
		col = (pglrcolumn*)plist_access( e );

		if( col->symbol == sym )
		{
			pcb->shift = col->shift;
			pcb->reduce = col->reduce;

			if( !( pcb->shift || col->reduce ) )
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
	pcb->reduce = (pgprod*)NULL;

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

pboolean pg_parser_lr_parse( pgparser* parser, pgast* ast )
{
	pglrpcb		PCB;
	pglrpcb*	pcb 	= &PCB;
	pgtoken*	tok;
	pgastnode*	node	= (pgastnode*)NULL;
	pgastnode*	nnode;

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
	pcb->ast = ast;
	pcb->st = pstack_create( sizeof( pglrse ), 64 );

	push( pcb, (pgsymbol*)NULL,
				(pglrstate*)plist_access( plist_get( parser->states, 0 ) ),
					(pgtoken*)NULL );

	do
	{
		fprintf( stderr, "get token\n" );

		if( !pcb->la && !( pcb->la = pg_lexer_fetch( parser->lexer ) ) )
		{
			pcb->la = pg_token_create(
						pg_grammar_get_eoi( pcb->g ), (pgvalue*)NULL );
		}

		fprintf( stderr, "got token '%s' lexem '%s'\n",
			pg_symbol_get_name( pg_token_get_symbol( pcb->la ) ),
				pg_token_get_lexem( pcb->la ) );

		if( !get_action( pcb, pg_token_get_symbol( pcb->la ) ) )
		{
			/* TODO: Error recovery */
			fprintf( stderr, "Parse error\n" );
			return FALSE;
		}

		if( pcb->shift )
		{
			if( pcb->reduce )
				fprintf( stderr, "shift/reduce by production %d\n",
					pg_prod_get_id( pcb->reduce ) );
			else
				fprintf( stderr, "shift to state %d\n",
					plist_offset( plist_get_by_ptr(
						parser->states, pcb->shift ) ) );

			push( pcb, (pgsymbol*)NULL,
					pcb->reduce ? (pglrstate*)NULL : pcb->shift, pcb->la );

			pcb->la = (pgtoken*)NULL;
		}

		while( pcb->reduce )
		{
			fprintf( stderr,
				"reduce by production %d\n"
				"popping %d items off the stack, replacing by '%s'\n",
				pg_prod_get_id( pcb->reduce ),
				pg_prod_get_rhs_length( pcb->reduce ),
				pg_symbol_get_name( pg_prod_get_lhs( pcb->reduce ) ) );

			pcb->lhs = pg_prod_get_lhs( pcb->reduce );
			pop( pcb, pg_prod_get_rhs_length( pcb->reduce ), &node );

			/* Goal symbol reduced? */
			if( pcb->lhs == pg_grammar_get_goal( pcb->g )
					&& pstack_count( pcb->st ) == 1 )
			{
				if( pcb->ast )
					pg_ast_set_root( pcb->ast, node );

				break;
			}

			/* Construct abstract syntax tree */
			if( pcb->ast && pg_nonterm_get_emit(
								pg_prod_get_lhs( pcb->reduce ) ) )
			{
				nnode = pg_astnode_create(
							pg_prod_get_lhs( pcb->reduce ) );
				nnode->child = node;
				node = nnode;
			}

			get_goto( pcb );
			push( pcb, pcb->lhs, pcb->shift, (pgtoken*)NULL );

			pcb->tos->node = node;
			node = (pgastnode*)NULL;
		}
	}
	while( !pcb->reduce ); /* Break on goal */

	fprintf( stderr, "goal symbol reduced!\n" );

	return TRUE;
}
