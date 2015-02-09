/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	compile.c
Usage:	Compiler for BNF grammar definition language.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define NAMELEN			80
#define DERIVCHAR	'\''


#define T_CCL			PPSYMTYPE_CCL
#define T_STRING		PPSYMTYPE_STRING
#define T_REGEX			PPSYMTYPE_REGEX
#define T_IDENT			10
#define T_INT			11

#define T_SYMBOL		20

#define T_KLEENE		25
#define T_POSITIVE		26
#define T_OPTIONAL		27

#define T_PRODUCTION	30
#define T_NONTERMDEF	31
#define T_INLINE		32

#define T_TERMDEF		35

#define T_FLAG			40
#define T_GFLAG			41
#define T_EMIT			42


char* pp_derive_name( ppgram* g, char* base )
{
	int             i;
	static
	char    deriv   [ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !pp_sym_get_by_name( g, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

static pboolean pp_bnf_ast_to_gram( ppgram* g, parray* ast )
{
	typedef struct
	{
		int			emit;
		char*		buf;
		ppsym*		sym;
		parray*		seq;
		int			i;
	} ATT;

	ATT			att;
	ATT*		attp;
	parray*		st;
	ppmatch*	e;
	ppsym*		scope		= (ppsym*)NULL;
	ppprod*		p;
	int			i;
	pboolean	ignore;
	pboolean	doemit;
	pboolean	emitall		= FALSE;
	int			emit;
	int			emit_max	= 0;
	char		name		[ NAMELEN * 2 + 1 ];

	st = parray_create( sizeof( ATT ), 0 );

	for( e = (ppmatch*)parray_first( ast );
			e <= (ppmatch*)parray_last( ast ); e++ )
	{
		memset( &att, 0, sizeof( ATT ) );

		if( e->type & PPMATCH_BEGIN && !( e->type & PPMATCH_END ) )
		{
			if( e->emit == T_NONTERMDEF )
			{
				e++;

				att.buf = pstrndup( e->start, e->end - e->start );

				if( !( scope = pp_sym_get_by_name( g, att.buf ) ) )
					scope = pp_sym_create( g, PPSYMTYPE_NONTERM,
								att.buf, (char*)NULL );

				pfree( att.buf );
			}
			else if( e->emit == T_PRODUCTION )
				/* Push an empty sequence delimiter */
				parray_push( st, &att );

			continue;
		}

		switch( ( att.emit = e->emit ) )
		{
			case T_IDENT:
				att.buf = pstrndup( e->start, e->end - e->start );
				break;

			case T_INT:
				att.i = atoi( e->start );
				break;

			case T_CCL:
			case T_STRING:
			case T_REGEX:
				att.buf = pstrndup( e->start + 1, e->end - e->start - 2 );
				break;

			case T_INLINE:
				/* Inline nonterminal */
				att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
							pp_derive_name( g, scope->name ), (char*)NULL );
				att.sym->flags |= PPFLAG_DEFINED;

				while( ( attp = (ATT*)parray_last( st ) )
							&& attp->emit == T_PRODUCTION )
				{
					parray_pop( st );
					p = pp_prod_create( g, att.sym, (ppsym*)NULL );

					while( parray_count( attp->seq ) )
						pp_prod_append( p,
							*( (ppsym**)parray_pop( attp->seq ) ) );

					parray_free( attp->seq );
				}
				break;

			case T_SYMBOL:
				/* Symbol */
				attp = parray_pop( st );

				if( attp->sym )
					att.sym = attp->sym;
				else if( attp->buf )
				{
					if( attp->emit == T_IDENT )
					{
						if( !( att.sym = pp_sym_get_by_name( g, attp->buf ) ) )
							att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
										attp->buf, (char*)NULL );
					}
					else
					{
						att.sym = pp_sym_create( g, attp->emit,
													(char*)NULL, attp->buf );
						att.sym->flags |= PPFLAG_DEFINED;
					}

					pfree( attp->buf );
				}
				else
				{
					fprintf( stderr, "CHECK YOUR CODE:\n" );
					fprintf( stderr, "%s, %d: Can't find symbol.\n",
										__FILE__, __LINE__ );
				}

				att.sym->flags |= PPFLAG_CALLED;
				break;

			case T_KLEENE:
			case T_POSITIVE:
			case T_OPTIONAL:
				attp = parray_pop( st );

				if( e->emit == T_KLEENE || e->emit == T_POSITIVE )
				{
					sprintf( name, "%s%c", attp->sym->name, PPMOD_POSITIVE );

					if( !( att.sym = pp_sym_get_by_name( g, name ) ) )
					{
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
														name, (char*)NULL );

						if( g->flags & PPFLAG_PREVENTLREC )
							pp_prod_create( g, att.sym,
								attp->sym, att.sym, (ppsym*)NULL );
						else
							pp_prod_create( g, att.sym,
								att.sym, attp->sym, (ppsym*)NULL );

						pp_prod_create( g, att.sym, attp->sym, (ppsym*)NULL );
					}

					attp->sym = att.sym;
				}

				if( e->emit == T_OPTIONAL || e->emit == T_KLEENE )
				{
					sprintf( name, "%s%c", attp->sym->name, PPMOD_OPTIONAL );

					if( !( att.sym = pp_sym_get_by_name( g, name ) ) )
					{
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
														name, (char*)NULL );

						pp_prod_create( g, att.sym, attp->sym, (ppsym*)NULL );
						pp_prod_create( g, att.sym, (ppsym*)NULL );
					}
				}

				att.emit = T_SYMBOL;
				break;

			case T_PRODUCTION:
				att.seq = parray_create( sizeof( ppsym* ), 16 );

				while( ( attp = (ATT*)parray_last( st ) )
							&& attp->emit == T_SYMBOL )
				{
					parray_pop( st );
					parray_push( att.seq, &attp->sym );
				}

				if( !attp->emit )
					parray_pop( st ); /* Pop seqence delimiter */

				break;

			case T_FLAG:
				att.buf = pstrndup( e->start, e->end - e->start );
				break;

			case T_GFLAG:
				if( !strncmp( e->start, "emitall", e->end - e->start ) )
					emitall = TRUE;
				else if( !strncmp( e->start, "emitnone", e->end - e->start ) )
					emitall = FALSE;
				else if( !strncmp( e->start, "lrec", e->end - e->start ) )
					g->flags &= ~PPFLAG_PREVENTLREC;
				else if( !strncmp( e->start, "rrec", e->end - e->start ) )
					g->flags |= PPFLAG_PREVENTLREC;

				att.emit = 0;
				break;

			case T_EMIT:
				if( ( attp = (ATT*)parray_last( st ) ) && attp->emit == T_INT )
				{
					parray_pop( st );
					att.i = attp->i;
				}
				break;

			case T_NONTERMDEF:
			case T_TERMDEF:
				doemit = emitall;
				emit = 0;
				ignore = FALSE;

				while( ( attp = (ATT*)parray_last( st ) )
							&& ( attp->emit == T_FLAG
									|| attp->emit == T_EMIT ) )
				{
					parray_pop( st );

					if( attp->emit == T_EMIT )
					{
						doemit = TRUE;
						emit = attp->i;
						continue;
					}
					else if( strcmp( attp->buf, "goal" ) == 0 )
					{
						if( scope && !g->goal ) /* fixme */
						{
							g->goal = scope;
							scope->flags |= PPFLAG_CALLED;
						}
					}
					else if( strcmp( attp->buf, "noemit" ) == 0 )
						doemit = FALSE;
					else if( strcmp( attp->buf, "ignore" ) == 0 )
						ignore = TRUE;
					else if( strcmp( attp->buf, "lexem" ) == 0 )
						scope->flags |= PPFLAG_LEXEM;

					pfree( attp->buf );
				}

				if( e->emit == T_TERMDEF )
				{
					attp = parray_pop( st ); 	/* Definition */

					if( parray_last( st )
						&& ( (ATT*)parray_last( st ) )->emit == T_IDENT )
					{
						attp = parray_pop( st );	/* Identifier */

						scope = pp_sym_create( g, ( attp + 1 )->emit,
											attp->buf, ( attp + 1 )->buf );

						pfree( ( attp + 1 )->buf );
					}
					else
						scope = pp_sym_create( g, attp->emit,
													(char*)NULL, attp->buf );

					pfree( attp->buf );

					scope->flags |= PPFLAG_DEFINED;

					/* Whitespace now only for terminals */
					if( ignore )
					{
						scope->flags |= PPFLAG_WHITESPACE;
						plist_push( g->ws, scope );
					}
				}

				while( e->emit == T_NONTERMDEF
						&& ( attp = (ATT*)parray_last( st ) )
							&& attp->emit == T_PRODUCTION )
				{
					parray_pop( st );
					p = pp_prod_create( g, scope, (ppsym*)NULL );

					while( parray_count( attp->seq ) )
						pp_prod_append( p,
							*( (ppsym**)parray_pop( attp->seq ) ) );

					parray_free( attp->seq );
				}

				scope->flags |= PPFLAG_DEFINED;

				/* Set emit ID if configured */
				if( doemit )
				{
					if( emit )
					{
						scope->emit = emit;

						if( emit > emit_max )
							emit_max = emit;
					}
					else
						scope->emit = ++emit_max;
				}

				att.emit = 0;
				break;
		}

		if( att.emit > 0 )
			parray_push( st, &att );
	}

	if( parray_count( st ) )
	{
		fprintf( stderr, "CHECK YOUR CODE:\n" );
		fprintf( stderr, "%s, %d: Still %d elements on stack\n",
			__FILE__, __LINE__, parray_count( st ) );

		while( attp = parray_pop( st ) )
			printf( "%d\n", attp->emit );

		parray_free( st );
		return FALSE;
	}

	parray_free( st );

	return TRUE;
}

pboolean pp_bnf_to_gram( ppgram* g, char* bnf )
{
	ppgram*		bnfgram;
	char*		s = bnf;
	char*		e;
	parray*		a;

	/* Define grammar for BNF */
	bnfgram = pp_gram_create( (char*)NULL );
	pp_bnf_define( bnfgram );
	pp_gram_prepare( bnfgram );

	if( !pp_lr_parse( &a, bnfgram, s, &e ) )
	{
		pp_gram_free( bnfgram );
		return FALSE;
	}

	/* pp_ast_simplify( a ); */

	pp_gram_free( bnfgram );

	if( !pp_bnf_ast_to_gram( g, a ) )
		return FALSE;

	parray_free( a );
	return TRUE;
}
