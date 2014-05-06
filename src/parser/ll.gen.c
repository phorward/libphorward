/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ll.gen.c
Usage:	LL parse table generator
----------------------------------------------------------------------------- */

#include "phorward.h"
#include "ll.h"

static void print_tab( pgparser* parser )
{
	plistel*		e;
	plistel*		f;
	plist*			trans;
	pgprod*			p;

	plist_for( parser->states, e )
	{
		trans = (plist*)plist_access( e );
		fprintf( stderr, "%s:\n", pg_symbol_get_name(
									(pgsymbol*)plist_key( e ) ) );

		plist_for( trans, f )
		{
			p = (pgprod*)plist_access( f );
			fprintf( stderr, "	%s => %s\n",
				pg_symbol_get_name( (pgsymbol*)plist_key( f ) ),
				pg_prod_to_string( p ) );
		}
	}
}

pboolean pg_parser_ll_closure( pgparser* parser )
{
	int				i;
	pgprod*			p;
	pgnonterm*		lhs;
	pgterm*			term;
	plistel*		e;
	plist*			trans;

	PROC( "pg_parser_ll_closure" );
	PARMS( "parser", "%p", parser );

	if( !( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !pg_parser_is_ll( parser ) )
	{
		PGERR( pg_parser_get_grammar( parser ),
				__FILE__, __LINE__, "Wrong paradigm to function" );
		RETURN( FALSE );
	}

	pg_parser_ll_reset( parser );

	MSG( "Performing FIRST set computation" );
	if( !pg_grammar_compute_first( pg_parser_get_grammar( parser ) ) )
		RETURN( FALSE );

	MSG( "Performing FOLLOW set computation" );
	if( !pg_grammar_compute_follow( pg_parser_get_grammar( parser ) ) )
		RETURN( FALSE );

	MSG( "Performing SELECT set computation" );
	if( !pg_grammar_compute_select( pg_parser_get_grammar( parser ) ) )
		RETURN( FALSE );

	MSG( "Building the LL parse tables" );
	for( i = 0; ( p = pg_prod_get( pg_parser_get_grammar( parser ), i ) );
			i++ )
	{
		lhs = pg_prod_get_lhs( p );

		if( !( trans = (plist*)plist_access(
							plist_get_by_key( parser->states, (char*)lhs ) ) ) )
		{
			trans = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_PTRKEYS );
			plist_insert( parser->states, (plistel*)NULL, (char*)lhs, trans );
		}

		plist_for( p->select, e )
		{
			term = (pgsymbol*)plist_access( e );
			plist_insert( trans, (plistel*)NULL, (char*)term, p );
		}
	}

	print_tab( parser );

	RETURN( TRUE );
}

pboolean pg_parser_ll_reset( pgparser* parser )
{
	plistel*	e;

	if( !( parser ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !pg_parser_is_ll( parser ) )
	{
		PGERR( pg_parser_get_grammar( parser ),
				__FILE__, __LINE__, "Wrong paradigm to function" );
		return FALSE;
	}

	if( parser->states )
	{
		plist_for( parser->states, e )
			plist_free( (plist*)plist_access( e ) );

		plist_clear( parser->states );
	}
	else
		parser->states = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE
											| PLIST_MOD_PTRKEYS );

	return TRUE;
}
