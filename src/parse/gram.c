/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	gram.c
Usage:	Grammar construction and preparation.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Prepares the grammar //g// by computing all necessary stuff required for
runtime and parser generator.

The preparation process includes:
- Setting up final symbol and productions IDs
- Nonterminals FIRST-set computation
- Marking of left-recursions
- The 'lexem'-flag pull-through the grammar.
-

This function is only run internally.
Don't call it if you're unsure ;)... */
pboolean pp_gram_prepare( ppgram* g )
{
	plistel*	e;
	plistel*	er;
	plistel*	ep;
	ppprod*		p;
	ppprod*		cp;
	ppsym*		s;
	pboolean	nullable;
	plist*		call;
	plist*		done;
	int			i;
	int			cnt;
	int			pcnt;

	if( !g )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !g->goal )
	{
		/* No such goal! */
		return FALSE;
	}

	/* Reset symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		plist_erase( s->first );

		if( PPSYM_IS_TERMINAL( s ) )
			plist_push( s->first, s );
	}

	/* Compute FIRST sets and mark left-recursions */
	cnt = 0;
	call = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	do
	{
		pcnt = cnt;
		cnt = 0;

		plist_for( g->prods, e )
		{
			cp = (ppprod*)plist_access( e );
			plist_push( call, cp );

			while( plist_pop( call, &p ) )
			{
				plist_push( done, p );

				plist_for( p->rhs, er )
				{
					s = (ppsym*)plist_access( er );

					nullable = FALSE;

					if( PPSYM_IS_NONTERMINAL( s ) )
					{
						/* Union first set */
						plist_union( cp->lhs->first, s->first );

						/* Put prods on stack */
						for( i = 0; ( p = pp_sym_getprod( s, i ) ); i++ )
						{
							if( plist_count( p->rhs ) == 0 )
							{
								nullable = TRUE;
								continue;
							}

							if( p == cp )
							{
								p->lhs->flags |= PPFLAG_LEFTREC;
								p->flags |= PPFLAG_LEFTREC;
							}
							else if( !plist_get_by_ptr( done, p ) )
								plist_push( call, p );
						}
					}
					/* Extend first set if required */
					else if( !plist_get_by_ptr( cp->lhs->first, s ) )
						plist_push( cp->lhs->first, s );

					if( !nullable )
						break;
				}

				/* Flag nullable */
				if( !er )
				{
					cp->flags |= PPFLAG_NULLABLE;
					cp->lhs->flags |= PPFLAG_NULLABLE;
				}

				cnt += plist_count( cp->lhs->first );
			}

			plist_erase( done );
		}
	}
	while( pcnt < cnt );

	plist_clear( call );
	plist_clear( done );

	/* Pull-through all lexem symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		if( PPSYM_IS_TERMINAL( s ) || !( s->flags & PPFLAG_LEXEM ) )
			continue;

		plist_push( call, s );
	}

	while( plist_pop( call, &s ) )
	{
		plist_push( done, s );

		for( i = 0; ( p = pp_sym_getprod( s, i ) ); i++ )
		{
			plist_for( p->rhs, er )
			{
				s = (ppsym*)plist_access( er );
				s->flags |= PPFLAG_LEXEM;

				if( PPSYM_IS_NONTERMINAL( s ) )
				{
					if( !plist_get_by_ptr( done, s )
						&& !plist_get_by_ptr( call, s ) )
						plist_push( call, s );
				}
			}
		}
	}

	/* Clear all lists */
	plist_free( call );
	plist_free( done );

	return TRUE;
}

/** Creates a new ppgram-object. */
ppgram* pp_gram_create( void )
{
	ppsym*		s;
	ppgram*		g;

	/* Setup grammar description */
	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ), PLIST_MOD_RECYCLE
													| PLIST_MOD_EXTKEYS
														| PLIST_MOD_UNIQUE );
	g->prods = plist_create( sizeof( ppprod ), PLIST_MOD_RECYCLE );

	g->eof = pp_sym_create( g, PPSYM_T_EOF );
	g->eof->flags |= PPFLAG_SPECIAL;

	return g;
}

/** Compiles a grammar definition into a grammar.

//g// is the grammar that receives the result of the parse.
//bnf// is the BNF definition string that defines the grammar.
*/
pboolean pp_gram_from_bnf( ppgram* g, char* bnf )
{
	plex*		bnflex;
	ppgram*		bnfgram;
	char*		s = bnf;
	char*		e;
	ppast*		ast;

	if( !( g && bnf ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Define grammar for BNF */
	bnfgram = pp_gram_create();
	bnflex = plex_create( 0 );

	pp_bnf_define( bnfgram, bnflex );

	pp_gram_prepare( bnfgram );
	plex_prepare( bnflex );

	pp_gram_dump( stdout, bnfgram ); /* DEBUG */

	if( !pp_lr_parse( &ast, bnfgram, bnflex, s, &e ) )
	{
		pp_gram_free( bnfgram );
		return FALSE;
	}

	pp_ast_dump_short( stdout, ast );

	/* fixme
	if( !ast_to_gram( g, ast ) )
		return FALSE;

	pp_gram_free( bnfgram );
	pp_ast_free( ast );
	*/

	return TRUE;
}


/** Dumps the grammar //g// to //stream//. */
void pp_gram_dump( FILE* stream, ppgram* g )
{
	plistel*	e;
	plistel*	f;
	ppprod*		p;
	ppsym*		s;
	size_t		maxlhslen	= 0;
	size_t		maxemitlen	= 0;
	size_t		maxsymlen	= 0;

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		if( pstrlen( s->emit ) > maxemitlen )
			maxemitlen = pstrlen( s->emit );

		if( PPSYM_IS_NONTERMINAL( s ) && pstrlen( s->name ) > maxlhslen )
			maxlhslen = pstrlen( s->name );

		if( pstrlen( s->name ) > maxsymlen )
			maxsymlen = pstrlen( s->name );
	}

	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		fprintf( stream,
			"%s%s%s%s%s%s%-*s %-*s : ",
			g->goal == p->lhs ? "$" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			p->lhs->flags & PPFLAG_LEXEM ? "X" : " ",
			p->lhs->flags & PPFLAG_WHITESPACE ? "W" : " ",
			p->emit ? "@" : " ",
			maxemitlen, p->emit ? p->emit : "",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (ppsym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				fprintf( stream, " " );

			fprintf( stream, "%s", pp_sym_to_str( s ) );
		}

		fprintf( stream, "\n" );
	}

	fprintf( stream, "\n" );

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		fprintf( stream, "%03d  %-*s  %-*s",
			s->id, maxemitlen, s->emit ? s->emit : "",
				maxsymlen, s->name );

		if( PPSYM_IS_NONTERMINAL( s ) )
		{
			fprintf( stream, " {" );

			plist_for( s->first, f )
			{
				s = (ppsym*)plist_access( f );
				fprintf( stream, " " );
				fprintf( stream, "%s", pp_sym_to_str( s ) );
			}

			fprintf( stream, " }" );
		}

		fprintf( stream, "\n" );
	}
}

/** Frees grammar //g// and all its related memory. */
ppgram* pp_gram_free( ppgram* g )
{
	plistel*	e;
	ppsym*		s;
	ppprod*		p;

	if( !g )
		return (ppgram*)NULL;

	/* Erase symbols */
	while( plist_count( g->symbols ) )
		pp_sym_free( (ppsym*)plist_access( plist_first( g->symbols ) ) );

	/* Erase productions */
	while( plist_count( g->prods ) )
		pp_prod_free( (ppprod*)plist_access( plist_first( g->prods ) ) );

	plist_free( g->symbols );
	plist_free( g->prods );

	pfree( g );

	return (ppgram*)NULL;
}
