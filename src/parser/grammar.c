/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	grammar.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

/* Constructor */

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

/* Destructor */

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
	LIST*			m;
	pgproduction*	p;
	pgsymbol*		s;
	int				i;

	printf( "--- Productions ---\n" );
	LISTFOR( g->productions, l )
	{
		p = (pgproduction*)list_access( l );
		printf( "%02d %s\n", pg_production_get_id( p ),
								pg_production_to_string( p ) );

		if( p->select )
		{
			printf( "    SELECT => " );

			LISTFOR( p->select, m )
				printf( "%s ", pg_symbol_get_name(
									(pgsymbol*)list_access( m ) ) );

			printf( "\n" );
		}
	}

	printf( "--- Symbols ---\n" );
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		printf( "%02d %s\n", pg_symbol_get_id( s ), pg_symbol_get_name( s ) );

		if( s->first )
		{
			printf( "    FIRST  => " );

			LISTFOR( s->first, l )
				printf( "%s ", pg_symbol_get_name(
									(pgsymbol*)list_access( l ) ) );

			printf( "\n" );
		}

		if( s->follow )
		{
			printf( "    FOLLOW => " );
			LISTFOR( s->follow, l )
				printf( "%s ", pg_symbol_get_name(
								(pgsymbol*)list_access( l ) ) );

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

	if( !( g ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Required dependencies */
	if( !( pg_grammar_get_goal( g ) && pg_grammar_get_eoi( g ) ) )
	{
		PGERR( "Grammar must provide a goal symbol and end-of-file." );
		return FALSE;
	}

	/* Reset all symbols */
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		if( pg_symbol_is_terminal( s ) )
		{
			/* Terminal symbols are their own FIRST set - this must be set only
				once in the entire symbol's lifetime. */
			if( !s->first )
				s->first = list_push( s->first, s );
		}
		else
			/* Nonterminal symbols must be reset */
			s->first = list_free( s->first );

		s->nullable = FALSE;
	}

	/* Loop until no more changes appear */
	do
	{
		pf = f;
		f = 0;

		/* Loop trough nonterminal symbols */
		for( i = 0; cs = pg_nonterminal_get( g, i ); i++ )
		{
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
		PGERR( "Grammar must provide a goal symbol and end-of-file." );
		return FALSE;
	}

	/* Reset all symbols */
	for( i = 0; ( s = pg_symbol_get( g, i ) ); i++ )
	{
		/* First set computation must be done first */
		if( !( s->first ) )
		{
			PGERR( "FIRST-set must be computed first" );
			return FALSE;
		}

		s->follow = list_free( s->follow );
	}

	/* Goal symbol has end-of-input in its follow set */
	fs->follow = list_push( fs->follow, pg_grammar_get_eoi( g ) );

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
						s->follow = list_union( s->follow, fs->first );

						if( !fs->nullable )
							break;
					}

					if( !fs )
						s->follow = list_union( s->follow, ns->follow );

					f += list_count( s->follow );
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
	BOOLEAN			nullable;

	/* Check parameter validity and bounding */
	if( !( g ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !pg_production_get( g, 0 ) )
	{
		PGERR( "Grammar must contain at least one production" );
		return FALSE;
	}

	for( i = 0; ( p = pg_production_get( g, i ) ); i++ )
	{
		p->select = list_free( p->select );

		nullable = TRUE;
		for( j = 0; ( s = pg_production_get_rhs( p, j ) ); j++ )
		{
			p->select = list_union( p->select, s->first );

			if( !( nullable |= s->nullable ) )
				break;
		}

		if( nullable )
		{
			/* TODO: Multiple lhs */
			s = pg_production_get_lhs( p );
			p->select = list_union( p->select, s->first );
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
