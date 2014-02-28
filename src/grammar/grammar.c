/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	grammar.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pggrammar* pg_grammar_create( void )
{
	pggrammar*	g;

	g = (pggrammar*)pmalloc( sizeof( pggrammar ) );

	g->symbols = plist_create( sizeof( pgsymbol ),
						PLIST_MOD_RECYCLE | PLIST_MOD_UNIQUE |
							PLIST_MOD_EXTKEYS );
	g->productions = plist_create( sizeof( pgproduction ), PLIST_MOD_RECYCLE );

	g->eoi = pg_terminal_create( g, "@eof", "" );

	g->asttypes = plist_create( sizeof( pgasttype ),
						PLIST_MOD_RECYCLE | PLIST_MOD_UNIQUE );

	return g;
}

/* Destructor */

pggrammar* pg_grammar_free( pggrammar* g )
{
	if( !( g ) )
		return (pggrammar*)NULL;

	g->symbols = plist_free( g->symbols );
	g->productions = plist_free( g->productions );
	TODO;
	pfree( g );

	return (pggrammar*)NULL;
}

/* Debug */

void pg_grammar_print( pggrammar* g )
{
	plistel*		ep;
	plistel*		es;
	pgproduction*	p;
	pgsymbol*		s;
	int				i;

	printf( "--- Productions ---\n" );
	plist_for( g->productions, ep )
	{
		p = (pgproduction*)plist_access( ep );
		printf( "%02d %s\n", pg_production_get_id( p ),
								pg_production_to_string( p ) );

		if( plist_count( p->select ) )
		{
			printf( "    SELECT => " );

			plist_for( p->select, es )
				printf( "%s ", pg_symbol_get_name(
									(pgsymbol*)plist_access( es ) ) );

			printf( "\n" );
		}
	}

	printf( "--- Symbols ---\n" );
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		printf( "%02d %s\n", pg_symbol_get_id( s ), pg_symbol_get_name( s ) );

		if( plist_count( s->first ) )
		{
			printf( "    FIRST  => " );

			plist_for( s->first, es )
				printf( "%s ", pg_symbol_get_name(
									(pgsymbol*)plist_access( es ) ) );

			printf( "\n" );
		}

		if( plist_count( s->follow ) )
		{
			printf( "    FOLLOW => " );
			plist_for( s->follow, es )
				printf( "%s ", pg_symbol_get_name(
								(pgsymbol*)plist_access( es ) ) );

			printf( "\n" );
		}
	}
}

/* FIRST set computation */

BOOLEAN pg_grammar_compute_first( pggrammar* g )
{
	int				i;
	int				j;
	int				k;
	pgsymbol*		cs;						/* Current symbol */
	pgproduction*	p;						/* Production */
	pgsymbol*		s;						/* RHS symbol */
	pboolean		nullable;				/* Nullable flag */
	int				f			= 0;		/* Current FIRST count */
	int				pf;						/* Previous FIRST count */

	PROC( "pg_grammar_compute_first" );
	PARMS( "g", "%p", g );

	if( !( g ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	MSG( "Required dependencies" );
	if( !( pg_grammar_get_goal( g ) && pg_grammar_get_eoi( g ) ) )
	{
		PGERR( g, __FILE__, __LINE__,
				"Grammar must provide a goal symbol and end-of-file" );

		RETURN( FALSE );
	}

	MSG( "Reset all symbols" );
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		if( pg_symbol_is_terminal( s ) )
		{
			MSG( "Terminal" );
			/* Terminal symbols are their own FIRST set - this must be set only
				once in the entire symbol's lifetime. */
			if( !plist_count( s->first ) )
				plist_push( s->first, s );
		}
		else
		{
			MSG( "Nonterminal" );

			/* Nonterminal symbols must be reset */
			plist_clear( s->first );
		}

		s->nullable = FALSE;
	}

	MSG( "Loop until no more changes appear" );
	do
	{
		MSG( "Begin of new loop" );
		pf = f;
		f = 0;

		/* Loop trough nonterminal symbols */
		for( i = 0; cs = pg_nonterminal_get( g, i ); i++ )
		{
			VARS( "cs", "%s", pg_symbol_get_name( cs ) );

			/* Loop trough all nonterminal productions */
			for( j = 0; p = pg_production_get_by_lhs( cs, j ); j++ )
			{
				nullable = FALSE;

				/* Right-hand side is not empty? */
				if( pg_production_get_rhs( p, 0 ) )
				{
					/* Loop trough right-hand side */
					for( k = 0; s = pg_production_get_rhs( p, k ); k++ )
					{
						/* Union FIRST sets... */
						VARS( "union", "%s", pg_symbol_get_name( s ) );
						VARS( "with", "%s", pg_symbol_get_name( cs ) );

						plist_union( cs->first, s->first );

						/* ...until the right-hand side symbol is not
								nullable */
						if( !( nullable = s->nullable ) )
							break;
					}
				}
				else
					nullable = TRUE;

				cs->nullable |= nullable;
			}

			f += plist_count( cs->first );
		}
	}
	while( pf != f );

	RETURN( TRUE );
}

/* FOLLOW set computation */

BOOLEAN pg_grammar_compute_follow( pggrammar* g )
{
	int				i;
	int				j;
	int				k;
	int				l;
	pgsymbol*		ns;						/* Nonterminal symbol */
	pgproduction*	p;						/* Production */
	pgsymbol*		s;						/* RHS symbol */
	pgsymbol*		fs;						/* Following symbol */
	pboolean		nullable;				/* Nullable flag */
	int				f			= 0;		/* Current FIRST count */
	int				pf;						/* Previous FIRST count */

	/* Check parameter validity and bounding */
	if( !( g ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Required dependencies */
	if( !( ( fs = pg_grammar_get_goal( g ) ) && pg_grammar_get_eoi( g ) ) )
	{
		PGERR( g, __FILE__, __LINE__,
				"grammar must provide a goal symbol and end-of-file" );

		return FALSE;
	}

	/* Reset all symbols */
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		/* First set computation must be done first */
		if( !plist_count( s->first ) )
		{
			PGERR( g, __FILE__, __LINE__,
				"FIRST-sets must be computed first" );

			return FALSE;
		}

		plist_clear( s->follow );
	}

	/* Goal symbol has end-of-input in its follow set */
	plist_push( fs->follow, pg_grammar_get_eoi( g ) );

	/* Loop until no more changes appear */
	do
	{
		pf = f;
		f = 0;

		/* Loop trough all nonterminal symbols */
		for( i = 0; ns = pg_nonterminal_get( g, i ); i++ )
		{
			/* Loop trough all productions */
			for( j = 0; p = pg_production_get_by_lhs( ns, j ); j++ )
			{
				/* Loop trough right-hand side */
				for( k = 0; s = pg_production_get_rhs( p, k ); k++ )
				{
					for( l = k + 1; fs = pg_production_get_rhs( p, l ); l++ )
					{
						plist_union( s->follow, fs->first );

						if( !fs->nullable )
							break;
					}

					if( !fs )
						plist_union( s->follow, ns->follow );

					f += plist_count( s->follow );
				}
			}
		}
	}
	while( pf != f );

	return TRUE;
}

/* Finding SELECT sets */

BOOLEAN pg_grammar_compute_select( pggrammar* g )
{
	int				i;
	int				j;
	pgproduction*	p;
	pgsymbol*		s;

	/* Check parameter validity and bounding */
	if( !( g ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !pg_production_get( g, 0 ) )
	{
		PGERR( g, __FILE__, __LINE__,
			"Grammar must contain at least one production" );
		return FALSE;
	}

	for( i = 0; ( p = pg_production_get( g, i ) ); i++ )
	{
		plist_free( p->select );
		p->select = plist_create( 0, PLIST_MOD_PTR );

		for( j = 0; ( s = pg_production_get_rhs( p, j ) ); j++ )
		{
			plist_union( p->select, s->first );

			if( !( s->nullable ) )
				break;
		}

		if( !s )
		{
			/* TODO: Multiple lhs */
			s = pg_production_get_lhs( p );
			plist_union( p->select, s->follow );
		}
	}

	return TRUE;
}

/* Attribute: goal */

pgterminal* pg_grammar_get_goal( pggrammar* g )
{
	if( !( g ) )
	{
		WRONGPARAM;
		return (pgterminal*)NULL;
	}

	return g->goal;
}

/** Set the goal symbol of grammar //g// to nonterminal //goal//. */
BOOLEAN pg_grammar_set_goal( pggrammar* g, pgnonterminal* goal )
{
	if( !( g ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !( g->goal = goal ) )
		g->goal = pg_nonterminal_get( g, 0 );

	return TRUEBOOLEAN( ( g->goal ) );
}

/* Attribute: eoi */

pgterminal* pg_grammar_get_eoi( pggrammar* g )
{
	if( !( g ) )
	{
		WRONGPARAM;
		return (pgterminal*)NULL;
	}

	return g->eoi;
}

/** Set the end-of-input symbol of grammar //g// to terminal //eoi//. */
BOOLEAN pg_grammar_set_eoi( pggrammar* g, pgterminal* eoi )
{
	if( !( g ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !( g->eoi = eoi ) )
		g->eoi = pg_terminal_get( g, 0 );

	return TRUEBOOLEAN( g->eoi );
}

/* Attribute: whitespace */

BOOLEAN pg_grammar_parse_whitespace( pggrammar* grammar, char* str )
{
	pregex_ptn*		whitespace		= (pregex_ptn*)NULL;

	if( !grammar )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( str )
	{
		if( pregex_ptn_parse( &whitespace,
				str, PREGEX_MOD_NONE ) != ERR_OK )
			return FALSE;
	}

	pg_grammar_set_whitespace( grammar, whitespace );

	return TRUE;
}

BOOLEAN pg_grammar_set_whitespace( pggrammar* grammar, pregex_ptn* whitespace )
{
	if( !grammar )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( grammar->whitespace )
		pregex_ptn_free( grammar->whitespace );

	grammar->whitespace = whitespace;

	return TRUE;
}

pregex_ptn* pg_grammar_get_whitespace( pggrammar* grammar )
{
	if( !grammar )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	return grammar->whitespace;
}
