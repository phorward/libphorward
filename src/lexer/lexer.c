/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pglexer* pg_lexer_create( pggrammar* grammar )
{
	pregex_ptn*	p;
	pglexer*	l;
	pgterminal*	t;
	int			i;

	PROC( "pg_lexer_create" );
	PARMS( "grammar", "%p", grammar );

	if( !( grammar ) )
	{
		WRONGPARAM;
		RETURN( (pglexer*)NULL );
	}

	l = (pglexer*)pmalloc( sizeof( pglexer ) );

	l->nfa = pregex_nfa_create();
	l->dfa = pregex_dfa_create();

	l->tokens = plist_create( sizeof( pgtoken ), PLIST_MOD_RECYCLE );
	l->grammar = grammar;

	MSG( "Turning terminal symbols into lexer symbols" );
	for( i = 0; ( t = pg_terminal_get( l->grammar, i ) ); i++ )
	{
		VARS( "t", "%s", pg_symbol_get_name( t ) );

		if( ( p = pg_terminal_get_pattern( t ) ) )
		{
			p->accept->accept = pg_symbol_get_id( t ) + 1;
			if( pregex_ptn_to_nfa( l->nfa, p ) != ERR_OK )
			{
				MSG( "Cannot integrate symbol" );

				pg_lexer_free( l );
				RETURN( (pglexer*)NULL );
			}
		}
	}

	MSG( "Setting whitespace" );
	if( ( p = pg_grammar_get_whitespace( grammar ) ) )
	{
		MSG( "Whitespace defined by grammar" );

		p->accept->accept = 0;

		if( pregex_ptn_to_nfa( l->nfa, p ) != ERR_OK )
		{
			MSG( "Cannot integrate whitespace" );

			pg_lexer_free( l );
			RETURN( (pglexer*)NULL );
		}
	}
	else
	{
		MSG( "Any other input is whitespace" );
		TODO;
	}

	MSG( "Finalize" );
	if( pregex_dfa_from_nfa( l->dfa, l->nfa ) != ERR_OK )
	{
		MSG( "Something is !cool..." );

		pg_lexer_free( l );
		RETURN( (pglexer*)NULL );
	}

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

	plist_clear( lex->tokens );

	RETURN( TRUE );
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
