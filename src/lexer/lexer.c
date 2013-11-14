/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pglexer* pg_lexer_create( void )
{
	pglexer*	l;

	PROC( "pg_lexer_create" );

	l = (pglexer*)pmalloc( sizeof( pglexer ) );

	l->nfa = pregex_nfa_create();
	l->dfa = pregex_dfa_create();

	l->tokens = plist_create( sizeof( pgtoken ), PLIST_MOD_RECYCLE );

	RETURN( l );
}

pglexer* pg_lexer_create_from_grammar( pggrammar* grammar )
{
	pregex_ptn*	p;
	pglexer*	l;
	pgterminal*	t;
	int			i;

	PROC( "pg_lexer_create_from_grammar" );
	PARMS( "grammar", "%p", grammar );

	if( !( grammar ) )
	{
		WRONGPARAM;
		RETURN( (pglexer*)NULL );
	}

	l = pg_lexer_create();
	l->grammar = grammar;

	MSG( "Turning terminal symbols into lexer symbols" );
	for( i = 0; ( t = pg_terminal_get( l->grammar, i ) ); i++ )
	{
		if( ( p = pg_terminal_get_pattern( t ) ) )
			pregex_ptn_to_nfa( l->nfa, p );
	}

	RETURN( l );
}

pglexer* pg_lexer_free( pglexer* lex )
{
	if( !lex )
		return (pglexer*)NULL;

	lex->nfa = pregex_nfa_free( lex->nfa );
	lex->dfa = pregex_dfa_free( lex->dfa );

	lex->tokens = plist_free( lex->tokens );

	pfree( lex );

	return (pglexer*)NULL;
}
