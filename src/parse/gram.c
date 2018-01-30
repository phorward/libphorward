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
	plistel*		e;
	plistel*		er;
	plistel*		ep;
	ppprod*			prod;
	ppprod*			cprod;
	ppsym*			sym;
	pboolean		nullable;
	plist*			call;
	plist*			done;
	int				i;
	int				cnt;
	int				pcnt;
	unsigned int	idx;

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
	for( idx = 0, e = plist_first( g->symbols ); e; e = plist_next( e ), idx++ )
	{
		sym = (ppsym*)plist_access( e );
		sym->idx = idx;

		if( PPSYM_IS_TERMINAL( sym ) )
		{
			if( !plist_first( sym->first ) )
				plist_push( sym->first, sym );
		}
		else
			plist_erase( sym->first );
	}

	/* Reset productions */
	for( idx = 0, e = plist_first( g->prods ); e; e = plist_next( e ), idx++ )
	{
		prod = (ppprod*)plist_access( e );
		prod->idx = idx;
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
			cprod = (ppprod*)plist_access( e );
			plist_push( call, cprod );

			while( plist_pop( call, &prod ) )
			{
				plist_push( done, prod );

				plist_for( prod->rhs, er )
				{
					sym = (ppsym*)plist_access( er );

					nullable = FALSE;

					if( PPSYM_IS_NONTERMINAL( sym ) )
					{
						/* Union first set */
						plist_union( cprod->lhs->first, sym->first );

						/* Put prods on stack */
						for( i = 0; ( prod = pp_sym_getprod( sym, i ) ); i++ )
						{
							if( plist_count( prod->rhs ) == 0 )
							{
								nullable = TRUE;
								continue;
							}

							if( prod == cprod )
							{
								prod->lhs->flags |= PPFLAG_LEFTREC;
								prod->flags |= PPFLAG_LEFTREC;
							}
							else if( !plist_get_by_ptr( done, prod ) )
								plist_push( call, prod );
						}
					}
					/* Extend first set if required */
					else if( !plist_get_by_ptr( cprod->lhs->first, sym ) )
						plist_push( cprod->lhs->first, sym );

					if( !nullable )
						break;
				}

				/* Flag nullable */
				if( !er )
				{
					cprod->flags |= PPFLAG_NULLABLE;
					cprod->lhs->flags |= PPFLAG_NULLABLE;
				}

				cnt += plist_count( cprod->lhs->first );
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
		sym = (ppsym*)plist_access( e );

		if( PPSYM_IS_TERMINAL( sym ) || !( sym->flags & PPFLAG_LEXEM ) )
			continue;

		plist_push( call, sym );
	}

	while( plist_pop( call, &sym ) )
	{
		plist_push( done, sym );

		for( i = 0; ( prod = pp_sym_getprod( sym, i ) ); i++ )
		{
			plist_for( prod->rhs, er )
			{
				sym = (ppsym*)plist_access( er );
				sym->flags |= PPFLAG_LEXEM;

				if( PPSYM_IS_NONTERMINAL( sym ) )
				{
					if( !plist_get_by_ptr( done, sym )
						&& !plist_get_by_ptr( call, sym ) )
						plist_push( call, sym );
				}
			}
		}
	}

	/* Clear all lists */
	plist_free( call );
	plist_free( done );

	/* Set finalized */
	g->flags |= PPFLAG_FINALIZED;
	g->strval = pfree( g->strval );

	return TRUE;
}

/** Creates a new ppgram-object. */
ppgram* pp_gram_create( void )
{
	ppgram*		g;

	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ), PLIST_MOD_RECYCLE
													| PLIST_MOD_EXTKEYS
														| PLIST_MOD_UNIQUE );
	g->prods = plist_create( sizeof( ppprod ), PLIST_MOD_RECYCLE );

	g->eof = pp_sym_create( g, PPSYM_T_EOF, PPFLAG_SPECIAL );

	return g;
}


#define NAMELEN			80
#define DERIVCHAR		'\''


static pboolean traverse_production( ppgram* g, ppsym* lhs, ppast* node );

/* Derive name from basename */
static char* derive_name( ppgram* g, char* base )
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

#define NODE_IS( n, s ) 	( !strcmp( (n)->emit, s ) )

static ppsym* traverse_symbol( ppgram* g, ppsym* lhs, ppast* node )
{
	ppsym*		sym			= (ppsym*)NULL;
	ppast*		child;
	char		name		[ NAMELEN * 2 + 1 ];

	/* fprintf( stderr, "sym >%s<\n", node->emit ); */

	if( NODE_IS( node, "inline") )
	{
		sym = pp_sym_create( g, derive_name( g, lhs->name ),
				PPFLAG_FREENAME | PPFLAG_DEFINED | PPFLAG_GENERATED );

		for( child = node->child; child; child = child->next )
			if( !traverse_production( g, sym, child->child ) )
				return (ppsym*)NULL;
	}
	else if( NODE_IS( node, "Ident") )
	{
		sprintf( name, "%.*s", node->len, node->start );

		if( !( sym = pp_sym_get_by_name( g, name ) ) )
			sym = pp_sym_create( g, name, PPFLAG_FREENAME );
	}
	else
		MISSINGCASE;

	if( sym )
		sym->flags |= PPFLAG_CALLED;

	return sym;
}


static pboolean traverse_production( ppgram* g, ppsym* lhs, ppast* node )
{
	ppsym*		sym;
	ppsym*		csym;
	ppprod*		prod;
	ppprod*		popt;
	ppast*		child;
	char*		str;
	char		name		[ NAMELEN * 2 + 1 ];
	int			i;
	plistel*	e;

	prod = pp_prod_create( g, lhs, (ppsym*)NULL );

	for( ; node; node = node->next )
	{
		/* fprintf( stderr, "prod >%s<\n", node->emit ); */

		if( NODE_IS( node, "symbol" ) )
		{
			if( !( sym = traverse_symbol( g, lhs, node->child ) ) )
				return FALSE;

			pp_prod_append( prod, sym );
		}
		else
		{
			sym = traverse_symbol( g, lhs, node->child->child );
			str = sym->name;

			if( NODE_IS( node, "kle" ) || NODE_IS( node, "pos" ) )
			{
				sprintf( name, "%s%c", str, PPMOD_POSITIVE );

				if( !( csym = pp_sym_get_by_name( g, name ) ) )
				{
					csym = pp_sym_create( g, name,
								PPFLAG_FREENAME | PPFLAG_DEFINED
									| PPFLAG_CALLED | PPFLAG_GENERATED );

					if( g->flags & PPFLAG_PREVENTLREC )
						pp_prod_create( g, csym, sym, csym, (ppsym*)NULL );
					else
						pp_prod_create( g, csym, csym, sym, (ppsym*)NULL );

					pp_prod_create( g, csym, sym, (ppsym*)NULL );
				}

				sym = csym;
			}

			if( NODE_IS( node, "opt" ) || NODE_IS( node, "kle" ) )
			{
				sprintf( name, "%s%c", str, PPMOD_OPTIONAL );

				if( !( csym = pp_sym_get_by_name( g, name ) ) )
				{
					csym = pp_sym_create( g, name,
								PPFLAG_FREENAME | PPFLAG_DEFINED
									| PPFLAG_CALLED | PPFLAG_GENERATED );

					pp_prod_create( g, csym, sym, (ppsym*)NULL );
					pp_prod_create( g, csym, (ppsym*)NULL );
				}

				sym = csym;
			}

			pp_prod_append( prod, sym );
		}
	}

	return TRUE;
}

static pboolean ast_to_gram( ppgram* g, ppast* ast )
{
	ppsym*		sym			= (ppsym*)NULL;
	ppast*		child;
	char		name		[ NAMELEN * 2 + 1 ];

	for( ast; ast; ast = ast->next )
	{
		if( NODE_IS( ast, "nonterm" ) )
		{
			child = ast->child;
			sprintf( name, "%.*s", child->len, child->start );

			/* Create the non-terminal symbol */
			if( !( sym = pp_sym_get_by_name( g, name ) ) )
				sym = pp_sym_create( g, name,
						PPFLAG_FREENAME | PPFLAG_DEFINED );

			for( child = child->next; child; child = child->next )
			{
				if( NODE_IS( child, "production" ) )
				{
					if( !traverse_production( g, sym, child->child ) )
						return FALSE;
				}
				else
					MISSINGCASE;
			}
		}
		else
			MISSINGCASE;
	}

	/* If there is no goal, then the last defined nonterminal
		becomes the goal symbol */
	if( !g->goal )
		g->goal = sym;

	/* Look for unique goal sequence; if this is not the case, wrap it with
		another, generated nonterminal. */
	if( pp_sym_getprod( g->goal, 1 ) )
	{
		sym = pp_sym_create( g, derive_name( g, g->goal->name ),
				PPFLAG_FREENAME | PPFLAG_DEFINED
					| PPFLAG_CALLED | PPFLAG_GENERATED );

		pp_prod_create( g, sym, g->goal, (ppsym*)NULL );
		g->goal = sym;
	}

	return TRUE;
}

/** Compiles a grammar definition into a grammar.

//g// is the grammar that receives the result of the parse.
//source// is the BNF definition string that defines the grammar.
*/
pboolean pp_gram_from_bnf( ppgram* g, char* source )
{
	pparser*	par;
	ppgram*		bnf;
	char*		s = source;
	char*		e;
	ppast*		ast;

	ppsym*		ident;
	ppsym*		colon;
	ppsym*		semi;
	ppsym*		pipe;
	ppsym*		brop;
	ppsym*		brcl;
	ppsym*		star;
	ppsym*		quest;
	ppsym*		plus;

	ppsym*		n_symbol;
	ppsym*		n_mod;
	ppsym*		n_seq;
	ppsym*		n_prod;
	ppsym*		n_alt;
	ppsym*		n_nonterm;
	ppsym*		n_defs;
	ppsym*		n_grammar;

	ppprod*		p;

	if( !( g && source ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Define a grammar for BNF */

	bnf = pp_gram_create();

	/*
		Ident : [a-z]+;

		symbol : '(' alternation ')' | Ident ;
		modifier : symbol | symbol '*' | symbol '+' | symbol '?' ;
		sequence : sequence modifier | modifier ;
		production : sequence | ;
		alternation : alternation '|' production | production ;

		nonterm : Ident ':' alternation ';';
		defs : defs nonterm | nonterm ;

		grammar$ : defs ;
	*/

	/* Terminals */
	ident = pp_sym_create( bnf, "Ident", PPFLAG_NONE );
	ident->emit = "Ident";

	colon = pp_sym_create( bnf, "Colon", PPFLAG_NONE );
	semi = pp_sym_create( bnf, "Semi", PPFLAG_NONE );
	pipe = pp_sym_create( bnf, "Pipe", PPFLAG_NONE );
	brop = pp_sym_create( bnf, "BrOp", PPFLAG_NONE );
	brcl = pp_sym_create( bnf, "BrCl", PPFLAG_NONE );
	star = pp_sym_create( bnf, "Star", PPFLAG_NONE );
	quest = pp_sym_create( bnf, "Quest", PPFLAG_NONE );
	plus = pp_sym_create( bnf, "Plus", PPFLAG_NONE );

	/* Nonterminals */
	n_symbol = pp_sym_create( bnf, "symbol", PPFLAG_NONE );
	n_symbol->emit = "symbol";

	n_mod = pp_sym_create( bnf, "modifier", PPFLAG_NONE );
	n_seq = pp_sym_create( bnf, "sequence", PPFLAG_NONE );

	n_prod = pp_sym_create( bnf, "production", PPFLAG_NONE );
	n_prod->emit = "production";

	n_alt = pp_sym_create( bnf, "alternation", PPFLAG_NONE );

	n_nonterm = pp_sym_create( bnf, "nonterm", PPFLAG_NONE );
	n_nonterm->emit = "nonterm";

	n_defs = pp_sym_create( bnf, "defs", PPFLAG_NONE );

	n_grammar = pp_sym_create( bnf, "grammar", PPFLAG_NONE );
	bnf->goal = n_grammar;

	/* Productions */

	p = pp_prod_create( bnf, n_symbol, brop, n_alt, brcl, (ppsym*)NULL );
	p->emit = "inline";
	p = pp_prod_create( bnf, n_symbol, ident, (ppsym*)NULL );

	p = pp_prod_create( bnf, n_mod, n_symbol, (ppsym*)NULL );
	p = pp_prod_create( bnf, n_mod, n_symbol, star, (ppsym*)NULL );
	p->emit = "star";
	p = pp_prod_create( bnf, n_mod, n_symbol, plus, (ppsym*)NULL );
	p->emit = "pos";
	p = pp_prod_create( bnf, n_mod, n_symbol, quest, (ppsym*)NULL );
	p->emit = "opt";

	pp_prod_create( bnf, n_seq, n_seq, n_mod, (ppsym*)NULL );
	pp_prod_create( bnf, n_seq, n_mod, (ppsym*)NULL );

	pp_prod_create( bnf, n_prod, n_seq, (ppsym*)NULL );
	pp_prod_create( bnf, n_prod, (ppsym*)NULL );

	pp_prod_create( bnf, n_alt, n_alt, pipe, n_prod, (ppsym*)NULL );
	pp_prod_create( bnf, n_alt, n_prod, (ppsym*)NULL );

	pp_prod_create( bnf, n_nonterm, ident, colon, n_alt, semi, (ppsym*)NULL );

	pp_prod_create( bnf, n_defs, n_defs, n_nonterm, (ppsym*)NULL );
	pp_prod_create( bnf, n_defs, n_nonterm, (ppsym*)NULL );

	pp_prod_create( bnf, n_grammar, n_defs, (ppsym*)NULL );

	/* Setup a parser */
	par = pp_parser_create( bnf );

	/* Lexer */
	plex_define( par->lex, "[^:;|()*?+ \t\r\n]+", ident->idx + 1, 0 );
	plex_define( par->lex, ":", colon->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, ";", semi->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, "|", pipe->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, "(", brop->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, ")", brcl->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, "*", star->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, "?", quest->idx + 1, PREGEX_COMP_STATIC );
	plex_define( par->lex, "+", plus->idx + 1, PREGEX_COMP_STATIC );

	plex_prepare( par->lex );

	if( !pp_parser_parse( &ast, par, s, &e ) )
	{
		pp_parser_free( par );
		pp_gram_free( bnf );
		return FALSE;
	}

	/* pp_ast_dump_short( stdout, ast ); */ /* DEBUG! */

	if( !ast_to_gram( g, ast ) )
		return FALSE;

	pp_parser_free( par );
	pp_gram_free( bnf );
	pp_ast_free( ast );

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
			s->idx, maxemitlen, s->emit ? s->emit : "",
				maxsymlen, s->name );

		if( PPSYM_IS_NONTERMINAL( s ) && plist_count( s->first ) )
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

/** Get grammar string representation */
char* pp_gram_to_str( ppgram* grm )
{
	ppsym*		sym;
	ppsym*		psym;
	ppprod*		prod;
	size_t		maxlhslen	= 0;
	size_t		maxsymlen	= 0;
	plistel*	e;
	plistel*	f;
	plistel*	g;
	char		name		[ NAMELEN + 3 + 1 ];
	char*		symname;
	pboolean	first;

	if( !grm )
	{
		WRONGPARAM;
		return "";
	}

	if( !( grm->flags & PPFLAG_FINALIZED ) )
		pp_gram_prepare( grm );

	if( grm->strval )
		return grm->strval;

	/* Find longest lhs */
	plist_for( grm->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		if( PPSYM_IS_NONTERMINAL( sym ) )
		{
			if( pstrlen( sym->name ) > maxlhslen )
				maxlhslen = pstrlen( sym->name );
			else if( !sym->name && maxlhslen < 4 )
				maxlhslen = 4;
		}

		if( pstrlen( sym->name ) > maxsymlen )
			maxsymlen = pstrlen( sym->name );
		else if( !sym->name && maxsymlen < 4 )
			maxsymlen = 4;
	}

	if( maxlhslen > NAMELEN )
		maxlhslen = NAMELEN;

	if( maxsymlen > NAMELEN )
		maxsymlen = NAMELEN;

	/* Generate! */
	plist_for( grm->symbols, e )
	{
		if( PPSYM_IS_TERMINAL( ( sym = (ppsym*)plist_access( e ) ) ) )
			continue;

		first = TRUE;
		sprintf( name, "%-*s : ", maxlhslen, pp_sym_to_str( sym ) );

		grm->strval = pstrcatstr( grm->strval, name, FALSE );

		plist_for( grm->prods, f )
		{
			prod = (ppprod*)plist_access( f );

			if( prod->lhs != sym )
				continue;

			if( !first )
			{
				sprintf( name, "%-*s | ", maxlhslen, "" );
				grm->strval = pstrcatstr( grm->strval, name, FALSE );
			}
			else
				first = FALSE;

			if( !plist_first( prod->rhs ) )
				grm->strval = pstrcatchar( grm->strval, '\n' );

			plist_for( prod->rhs, g )
			{
				psym = (ppsym*)plist_access( g );

				sprintf( name, "%.*s%s", maxsymlen, pp_sym_to_str( psym ),
					g == plist_last( prod->rhs ) ? "\n" : " ");
				grm->strval = pstrcatstr( grm->strval, name, FALSE );
			}
		}

		sprintf( name, "%-*s ;\n", maxlhslen, "" );
		grm->strval = pstrcatstr( grm->strval, name, FALSE );
	}

	return grm->strval;
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

	pfree( g->strval );

	pfree( g );

	return (ppgram*)NULL;
}
