/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pglexer* pg_lexer_create_by_parser( pgparser* parser )
{
	pregex_dfa_st*	st;
	pregex_dfa_tr*	tr;
	pregex_nfa*		nfa;
	pregex_dfa*		dfa;
	pregex_ptn*		p;
	pglexer*		l;
	pgterminal*		t;
	int				i;
	int				j;
	int				k;
	int				cnt;
	plistel*		e;
	plistel*		f;
	pchar			from;
	pchar			to;

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

	MSG( "Finalize" );
	dfa = pregex_dfa_create();

	pregex_nfa_print( nfa );

	if( pregex_dfa_from_nfa( dfa, nfa ) <= 0 )
	/* 			|| pregex_dfa_minimize( dfa ) <= 0 */
	{
		MSG( "Something is !cool..." );

		pg_lexer_free( l );
		RETURN( (pglexer*)NULL );
	}

	pregex_dfa_print( stdout, dfa );

	nfa = pregex_nfa_free( nfa );

	/*
		Now that we have a DFA state machine from the regular expressions
		expressed by the grammar, this DFA can be copied into simple integer
		tables for a better performance experience when lexical analysis
		is performed.
	*/

	MSG( "Convert DFA tables into lexer" );
	l->dcount = plist_count( dfa->states );
	l->dtrans = (int**)pmalloc( l->dcount * sizeof( int* ) );

	for( i = 0, e = plist_first( dfa->states ); e; e = plist_next( e ), i++ )
	{
		VARS( "state( i )", "%d", i );
		st = (pregex_dfa_st*)plist_access( e );

		/*
			Row formatting:

			Number-of-columns;Accept;Default-Goto;From-Char;To-Char;Goto;...

			The rest consists of triples containing
			"From-Char;To-Char;Goto;" each.
		*/

		MSG( "Examining required number of columns" );
		for( cnt = 3, f = plist_first( st->trans ); f; f = plist_next( f ) )
		{
			tr = (pregex_dfa_tr*)plist_access( f );
			cnt += ( pregex_ccl_size( tr->ccl ) * 3 );
		}

		VARS( "required( cnt )", "%d", cnt );

		l->dtrans[i] = (int*)pmalloc( cnt * sizeof( int ) );

		l->dtrans[i][0] = cnt;
		l->dtrans[i][1] = st->accept.accept;
		l->dtrans[i][2] = st->def_trans ? st->def_trans->go_to : -1;

		MSG( "Fill column" );
		for( j = 3, f = plist_first( st->trans ); f; f = plist_next( f ) )
		{
			tr = (pregex_dfa_tr*)plist_access( f );

			for( k = 0; pregex_ccl_get( &from, &to, tr->ccl, k ); k++ )
			{
				l->dtrans[i][j++] = from;
				l->dtrans[i][j++] = to;
				l->dtrans[i][j++] = tr->go_to;
			}
		}
	}

	dfa = pregex_dfa_free( dfa );

#if 1
	for( i = 0; i < l->dcount; i++ )
	{
		printf( "%02d: cnt=%d, accept=%d, default=%d",
					i, l->dtrans[i][0], l->dtrans[i][1], l->dtrans[i][2] );

		for( j = 3; j < l->dtrans[i][0]; j += 3 )
		{
			printf( " %d;%d:%02d",
				l->dtrans[i][j], l->dtrans[i][j+1], l->dtrans[i][j+2] );
		}

		printf( "\n" );
	}
#endif

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
