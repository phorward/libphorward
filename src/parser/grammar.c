/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See \LICENSE for more information.

File:	grammar.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

pggrammar* pg_grammar_create( void )
{
	pggrammar*	g;

	g = (pggrammar*)pmalloc( sizeof( pggrammar ) );

	g->symbols = plist_create( sizeof( pgsymbol ),
						PLIST_MOD_RECYCLE | PLIST_MOD_UNIQUE |
							PLIST_MOD_EXTKEYS );

	g->eoi = pg_terminal_create( g, "@eof", (char*)NULL );

	return g;
}

pggrammar* pg_grammar_free( pggrammar* g )
{
	if( !( g ) )
		return (pggrammar*)NULL;

	g->symbols = plist_free( g->symbols );
	TODO;
	pfree( g );

	return (pggrammar*)NULL;
}

/* Debug */

void pg_grammar_print( pggrammar* g )
{
	LIST*			l;
	pgproduction*	p;
	pgsymbol*		s;
	int				i;

	LISTFOR( g->productions, l )
	{
		p = (pgproduction*)list_access( l );
		printf( "% 2d %s\n", pg_production_get_id( p ),
								pg_production_to_string( p ) );
	}

	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		printf( "% 2d %s FIRST => ", pg_symbol_get_id( s ),
										pg_symbol_get_name( s ) );
		LISTFOR( s->first, l )
		{
			printf( "%s ", pg_symbol_get_name( (pgsymbol*)list_access( l ) ) );
		}

		printf( "\n" );
	}
}

/* FIRST set computation */

pboolean pg_grammar_compute_first( pggrammar* g, pgparadigm para )
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

	/* Check parameter validity and bounding */
	if( !( g && ( para > PGPARADIGM_UNDEFINED && para < PGPARADIGM_EOP ) ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Required dependencies */
	if( !( pg_grammar_get_goal( g ) && pg_grammar_get_eoi( g ) ) )
		return FALSE;

	/* Reset all symbols */
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		pg_symbol_reset( s );

		if( pg_symbol_is_terminal( s ) )
			s->first = list_push( s->first, s );
	}

	/* Loop until no more changes appear */
	do
	{
		pf = f;
		f = 0;

		/* Loop trough symbol database */
		for( i = 0; cs = pg_symbol_get( g, i ); i++ )
		{
			/* Terminal symbols define theirself als FIRST */
			if( pg_symbol_is_terminal( cs ) )
				continue;

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
						cs->first = list_union( cs->first, s->first );

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

			f += list_count( cs->first );
		}
	}
	while( pf != f );

	return TRUE;
}

/* FOLLOW set computation */


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
