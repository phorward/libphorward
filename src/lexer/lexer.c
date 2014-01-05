/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pglexer* pg_lexer_create_by_parser( pgparser* parser )
{

	pregex_nfa*		nfa;
	pregex_dfa*		dfa;
	pregex_ptn*		p;
	pglexer*		l;
	pgterminal*		t;
	int				i;


	PROC( "pg_lexer_create" );
	PARMS( "parser", "%p", parser );

	if( !( parser ) )
	{
		WRONGPARAM;
		RETURN( (pglexer*)NULL );
	}

	l = (pglexer*)pmalloc( sizeof( pglexer ) );

	l->grammar = pg_parser_get_grammar( parser );

	MSG( "Turning terminal symbols into lexer symbols" );
	nfa = pregex_nfa_create();

	for( i = 0; ( t = pg_terminal_get( l->grammar, i ) ); i++ )
	{
		VARS( "t", "%s", pg_symbol_get_name( t ) );

		if( ( p = pg_terminal_get_pattern( t ) ) )
		{
			p->accept->accept = pg_symbol_get_id( t ) + 1;

			if( pregex_ptn_to_nfa( nfa, p ) != ERR_OK )
			{
				MSG( "Cannot integrate symbol" );

				pg_lexer_free( l );
				RETURN( (pglexer*)NULL );
			}
		}
	}

	MSG( "Setting whitespace" );
	if( ( p = pg_grammar_get_whitespace( l->grammar ) ) )
	{
		MSG( "Whitespace defined by grammar" );

		p->accept->accept = 0;

		if( pregex_ptn_to_nfa( nfa, p ) != ERR_OK )
		{
			MSG( "Cannot integrate whitespace" );

			pg_lexer_free( l );
			RETURN( (pglexer*)NULL );
		}
	}
	else
	{
		MSG( "Any other input is whitespace" );
		l->ignore_unknown = TRUE;
	}

	MSG( "Construct a DFA from NFA" );
	dfa = pregex_dfa_create();

	if( pregex_dfa_from_nfa( dfa, nfa ) <= 0
			|| pregex_dfa_minimize( dfa ) <= 0 )
	{
		MSG( "Subset construction error" );

		pregex_nfa_free( nfa );
		pg_lexer_free( l );
		RETURN( (pglexer*)NULL );
	}

	pregex_nfa_free( nfa );

	MSG( "Convert DFA tables into lexer" );
	l->states_cnt = pregex_dfa_to_matrix( &l->states, dfa );
	pregex_dfa_free( dfa );

	MSG( "Lexer created" );
	RETURN( l );
}

pboolean pg_lexer_reset( pglexer* lex )
{
	PROC( "pg_lexer_reset" );
	PARMS( "lex", "%p", lex );

	if( !( lex ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	RETURN( TRUE );
}

pglexer* pg_lexer_free( pglexer* lex )
{
	if( !lex )
		return (pglexer*)NULL;

	pg_lexer_reset( lex );
	pfree( lex );

	return (pglexer*)NULL;
}

pgtoken* pg_lexer_fetch( pglexer* lex )
{
	PROC( "pg_lexer_fetch" );
	PARMS( "lex", "%p", lex );

	if( !( lex ) )
	{
		WRONGPARAM;
		RETURN( (pgtoken*)NULL );
	}


}
