/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	gram.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define DERIVCHAR	'#'

/** Creates a new symbol of the type //type// in the grammar //g//.

	//name// is the name for nonterminal symbols, for terminal symbols it
	can be left empty.
	//def// contains the definition of the symbol in case of a terminal type.
	It will be ignored else.
*/
ppsym* pp_sym_create( ppgram* g, ppsymtype type, char* name, char* def )
{
	ppsym*	sym;

	if( !g )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	name = pstrdup( name );

	/* Insert into symbol table */
	if( !( sym = (ppsym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		fprintf( stderr, "Symbol '%s' already exists\n", name );

		pfree( name );
		return (ppsym*)NULL;
	}

	sym->id = -1;
	sym->emit_id = -1;
	sym->name = name;

	switch( ( sym->type = type ) )
	{
		case PPSYMTYPE_NONTERM:
			sym->prods = plist_create( 0, PLIST_MOD_PTR  );
			break;

		case PPSYMTYPE_CCL:
			sym->ccl = p_ccl_create( 0, 255, def );

			if( !sym->name )
				sym->name = pstrdup( p_ccl_to_str( sym->ccl, TRUE ) );

			break;

		case PPSYMTYPE_STRING:
			sym->str = pstrdup( def );

			if( !sym->name )
				sym->name = pstrdup( def );

			break;

		case PPSYMTYPE_REGEX:
		printf( "MK REGEX >%s<\n", def );
			switch( pregex_ptn_parse( &sym->ptn, def, PREGEX_MOD_NONE ) )
			{
				case 1:
					fprintf( stderr, "Parse error in regex >%s<\n", def );
					break;

				case ERR_OK:
				default:
					sym->ptn->accept->accept = 0;
					break;
			}

			if( !sym->name )
				pregex_ptn_to_regex( &sym->name, sym->ptn );

			break;

		default:
			break;
	}

	/* FIRST */
	sym->first = plist_create( 0, PLIST_MOD_PTR );

	if( sym->type != PPSYMTYPE_NONTERM )
		plist_push( sym->first, sym );

	return sym;
}

/** Returns the string representation of symbol //p//.

	Nonterminals are not expanded, they are just returned as their name.
	The returned pointer is part of //sym// and can be referenced multiple
	times. It may not be freed by the caller. */
char* pp_sym_to_str( ppsym* sym )
{
	if( !sym )
	{
		WRONGPARAM;
		return "";
	}

	if( !sym->strval )
	{
		sym->strval = (char*)pmalloc(
						( strlen( sym->name ) + 2 + 1 + 1 )
							* sizeof( char ) );

		switch( sym->type )
		{
			case PPSYMTYPE_NONTERM:
				strcpy( sym->strval, sym->name );
				break;

			case PPSYMTYPE_CCL:
				sprintf( sym->strval, "'%s'", sym->name );
				break;

			case PPSYMTYPE_STRING:
				sprintf( sym->strval, "\"%s\"", sym->name );
				break;

			case PPSYMTYPE_REGEX:
				sprintf( sym->strval, "/%s/", sym->name );
				break;

			case PPSYMTYPE_SPECIAL:
				sprintf( sym->strval, "@%s", sym->name );
				break;

			default:
				MISSINGCASE;
				break;
		}
	}

	return sym->strval;
}

/** Creates a new production on left-hand-side //lhs//
	within the grammar //g//. */
ppprod* pp_prod_create( ppgram* g, ppsym* lhs, ... )
{
	ppprod*	prod;
	ppsym*	sym;
	va_list	varg;

	if( !( g && lhs ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	prod = (ppprod*)plist_malloc( g->prods );

	prod->id = -1;
	prod->lhs = lhs;
	prod->rhs = plist_create( 0, PLIST_MOD_PTR );

	plist_push( lhs->prods, prod );

	va_start( varg, lhs );

	while( ( sym = va_arg( varg, ppsym* ) ) )
		pp_prod_append( prod, sym );

	va_end( varg );

	return prod;
}

/** Appends the symbol //sym// to the right-hand-side of production //p//. */
pboolean pp_prod_append( ppprod* p, ppsym* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	plist_push( p->rhs, sym );
	pfree( p->strval );

	return TRUE;
}

/** Returns the //off//s element from the right-hand-side of
	production //p//. Returns (ppsym*)NULL if the requested element does
	not exist. */
ppsym* pp_prod_getfromrhs( ppprod* p, int off )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get( p->rhs, off ) );
}

/** Returns the string representation of production //p//.

	The returned pointer is part of //p// and can be referenced multiple times.
	It may not be freed by the caller. */
char* pp_prod_to_str( ppprod* p )
{
	plistel*	e;
	ppsym*		sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	if( !p->strval )
	{
		if( p->lhs )
			p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );

		p->strval = pstrcatstr( p->strval, " : ", FALSE );

		plist_for( p->rhs, e )
		{
			sym = (ppsym*)plist_access( e );

			if( e != plist_first( p->rhs ) )
				p->strval = pstrcatstr( p->strval, " ", FALSE );

			if( sym->type != PPSYMTYPE_NONTERM )
				p->strval = pstrcatstr( p->strval, "@", FALSE );

			p->strval = pstrcatstr( p->strval, sym->name, FALSE );
		}
	}

	return p->strval;
}

/* Parse BNF */

#define NAMELEN			80

#define PARSEERROR( def, expect )  \
	fprintf( stderr, "Parse error at >%.30s< %s%s%s\n", \
		*def, ( pstrlen( *def ) > 30 ) ? "..." : "", \
			expect ? ", expecting " : "", expect ? expect : "" )

#define SKIPWHITE()  \
	while( isspace( **def ) ) (*def)++

static pboolean read_char( char** def, char ch )
{
	char	chstr	[3+1];

	if( **def != ch )
	{
		sprintf( chstr, "'%c'", ch );
		PARSEERROR( def, chstr );
		return FALSE;
	}

	(*def)++;
	return TRUE;
}

static pboolean parse_alternatives( ppgram* g, ppsym* lhs, char** def );

static char* derive_name( ppgram* g, char* base )
{
	int		i;
	static
	char	deriv	[ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !plist_get_by_key( g->symbols, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

static ppsymtype parse_terminal( char* name, char** def )
{
	char		stopch;
	char*		start;
	char*		end;

	SKIPWHITE();

	if( **def == '\"' || **def == '\'' )
	{
		stopch = **def;
		start = end = ++(*def);

		while( *end != stopch && ( end - start < NAMELEN ) )
		{
			if( *end == '\\' )
				end++;

			end++;
		}

		*def = end + 1;
		sprintf( name, "%.*s", end - start, start );

		if( stopch == '\"' )
			return PPSYMTYPE_STRING;

		return PPSYMTYPE_CCL;
	}
	else if( **def == '/' )
	{
		start = end = ++(*def);

		while( *end != '/' && ( end - start < NAMELEN ) )
		{
			if( *end == '\\' )
				end++;

			end++;
		}

		*def = end + 1;
		sprintf( name, "%.*s", end - start, start );

		return PPSYMTYPE_REGEX;
	}

	/* No symbol found */
	return PPSYMTYPE_NONTERM;
}

static pboolean parse_ident( char* name, char** def )
{
	char*		start;
	char*		end;

	SKIPWHITE();

	if( isalpha( **def ) )
	{
		start = end = *def;

		while( ( isalnum( *end ) || *end == '_' )
				&& ( end - start < NAMELEN ) )
			end++;

		sprintf( name, "%.*s", end - start, start );
		*def = end;

		return TRUE;
	}

	return FALSE;
}

static pboolean parse_integer( int* val, char** def )
{
	char*		start;
	char*		end;

	SKIPWHITE();

	if( isdigit( **def ) )
	{
		start = end = *def;

		while( isdigit( *end ) )
			end++;

		*def = end;
		*val = atoi( start );
		return TRUE;
	}

	return FALSE;
}

static pboolean parse_factor( ppgram* g, ppsym* lhs, ppprod* p, char** def )
{
	ppsymtype	type;
	ppsym*		sym		= (ppsym*)NULL;
	ppsym*		mod;
	char		name	[ NAMELEN + 1 ];
	char		op;
	int			i;

	SKIPWHITE();

	if( parse_ident( name, def ) )
	{
		if( !( sym = (ppsym*)plist_access(
								plist_get_by_key( g->symbols, name ) ) ) )
			sym = pp_sym_create( g, PPSYMTYPE_NONTERM, name, (char*)NULL );

		sym->flags |= PPFLAG_CALLED;
	}
	else if( **def == '(' )
	{
		(*def)++;

		sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
					derive_name( g, lhs->name ), (char*)NULL );

		if( !parse_alternatives( g, sym, def ) )
			return FALSE;

		if( !read_char( def, ')' ) )
			return FALSE;
	}
	else if( ( type = parse_terminal( name, def ) ) > 0 )
		sym = pp_sym_create( g, type, (char*)NULL, name );
	else
	{
		return TRUE;
	}

	SKIPWHITE();

	/* Replication quantifiers */
	if( **def == PPMOD_KLEENE /* no or many */
			|| **def == PPMOD_POSITIVE /* one or many */
				|| **def == PPMOD_OPTIONAL /* none or one */ )
	{
		op = *( (*def)++ );

		/* Right-recursive quantifiers? */
		if( g->flags & PPFLAG_PREVENTLREC )
		{
			if( op == PPMOD_KLEENE || op == PPMOD_POSITIVE )
			{
				mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, lhs->name ), (char*)NULL );

				pp_prod_create( g, mod, sym, mod, (ppsym*)NULL );
				pp_prod_create( g, mod, sym, (ppsym*)NULL );

				sym = mod;
			}

			if( op == PPMOD_OPTIONAL || op == PPMOD_KLEENE )
			{
				mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, lhs->name ), (char*)NULL );

				pp_prod_create( g, mod, sym, (ppsym*)NULL );
				pp_prod_create( g, mod, (ppsym*)NULL );

				sym = mod;
			}
		}
		/* Left-recursive quantifier */
		else
		{
			if( op == PPMOD_KLEENE || op == PPMOD_POSITIVE )
			{
				mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, lhs->name ), (char*)NULL );

				pp_prod_create( g, mod, mod, sym, (ppsym*)NULL );
				pp_prod_create( g, mod, sym, (ppsym*)NULL );

				sym = mod;
			}

			if( op == PPMOD_OPTIONAL || op == PPMOD_KLEENE )
			{
				mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, lhs->name ), (char*)NULL );

				pp_prod_create( g, mod, sym, (ppsym*)NULL );
				pp_prod_create( g, mod, (ppsym*)NULL );

				sym = mod;
			}
		}
	}

	plist_push( p->rhs, sym );
	return TRUE;
}

static pboolean parse_sequence( ppgram* g, ppsym* lhs, ppprod* p, char** def )
{
	pboolean	err;

	while( ( err = parse_factor( g, lhs, p, def ) ) )
	{
		SKIPWHITE();
		if( **def == ';' || **def == '|' || **def == ')' || **def == '%' )
			break;
	}

	return err;
}

static pboolean parse_alternatives( ppgram* g, ppsym* lhs, char** def )
{
	ppprod*		p;

	while( TRUE )
	{
		p = pp_prod_create( g, lhs, (ppsym*)NULL );

		if( !parse_sequence( g, lhs, p, def ) )
			return FALSE;

		if( **def != '|' )
			break;
		(*def)++;
	}

	return TRUE;
}

static int pp_gram_read( ppgram* g, char** def )
{
	ppsymtype	type;
	ppsym*		sym;
	ppsym*		rsym;
	plistel*	e;
	ppprod*		p;
	int			defcount	= 0;
	char		name		[ NAMELEN + 1 ];
	char		symdef		[ NAMELEN + 1 ];
	int			dflags		= 0;

	while( *def )
	{
		SKIPWHITE();

		if( ! **def )
			break;

		if( parse_ident( name, def ) )
		{
			SKIPWHITE();

			/* Is nonterminal definition? */
			if( **def == ':' )
			{
				(*def)++;

				if( !( sym = (ppsym*)plist_access(
											plist_get_by_key(
												g->symbols, name ) ) ) )
					sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
												name, (char*)NULL );

				sym->flags |= PPFLAG_DEFINED | dflags;

				if( !parse_alternatives( g, sym, def ) )
					return -1;
			}
			/* Or is named terminal definition? */
			else if( **def == '=' )
			{
				(*def)++;
				SKIPWHITE();

				if( !( type = parse_terminal( symdef, def ) ) )
					PARSEERROR( def, "terminal definition" );

				sym = pp_sym_create( g, type, name, symdef );
				sym->flags |= dflags;
			}
			else
			{
				PARSEERROR( def, "':' or '=' for nonterminal"
									" or named terminal definition" );
				return -1;
			}

			/* More descriptive symbol attribution */
			while( TRUE )
			{
				SKIPWHITE();

				if( **def == '%' )
				{
					(*def)++;
					SKIPWHITE();

					if( !parse_ident( name, def ) )
					{
						PARSEERROR( def, "identifier" );
						return -1;
					}

					if( !strcmp( name, "goal" ) )
					{
						if( sym->type > PPSYMTYPE_NONTERM )
							/* TODO: error reporting */
							fprintf( stderr,
								"Goal symbol must be a nonterminal.\n" );
						else if( g->goal && g->goal != sym )
							/* TODO: error reporting */
							fprintf( stderr,
								"Symbol '%s' already defined as goal\n",
								g->goal->name );
						else
							g->goal = sym;
					}
					else if( !strcmp( name, "emit") )
						sym->flags |= PPFLAG_EMIT;
					else if( !strcmp( name, "noemit" ) )
						sym->flags &= ~PPFLAG_EMIT;
					else if( !strcmp( name, "lexem" ) )
						sym->flags |= PPFLAG_LEXEM;
					else if( !strcmp( name, "whitespace" ) )
					{
						if( sym->type == PPSYMTYPE_NONTERM )
							printf( "%whitespace currently only allowed for "
									"terminal symbols in this implementation" );
						else
						{
							sym->flags |= PPFLAG_WHITESPACE | PPFLAG_CALLED;
							plist_push( g->ws, sym );
						}
					}
					else if( !strcmp( name, "id") )
					{
						sym->flags |= PPFLAG_EMIT;

						if( !parse_integer( &sym->emit_id, def ) )
						{
							PARSEERROR( def, "integer" );
							return -1;
						}
					}
				}
				else
					break;
			}

			SKIPWHITE();

			if( !read_char( def, ';' ) )
				return -1;

			defcount++;
		}
		else if( **def == '%' )
		{
			(*def)++;
			SKIPWHITE();

			if( !parse_ident( name, def ) )
			{
				PARSEERROR( def, "identifier" );
				return -1;
			}

			if( !strcmp( name, "emitall" ) )
				dflags |= PPFLAG_EMIT;
			else if( !strcmp( name, "rrec" ) )
				g->flags |= PPFLAG_PREVENTLREC;
			else if( !strcmp( name, "lrec" ) )
				g->flags &= ~PPFLAG_PREVENTLREC;
			else if( !strcmp( name, "emitnone" ) )
				dflags &= ~PPFLAG_EMIT;
			else if( !strcmp( name, "whitespace" ) )
			{
				SKIPWHITE();

				/* on the fly */
				if( ( type = parse_terminal( name, def ) ) > 0 )
					sym = pp_sym_create( g, type, (char*)NULL, name );
				/* or named? */
				else if( parse_ident( name, def ) )
					sym = (ppsym*)plist_access(
							plist_get_by_key( g->symbols, name ) );
				else
					sym = (ppsym*)NULL;

				if( !sym || sym->type == PPSYMTYPE_NONTERM )
				{
					PARSEERROR( def, "terminal definition for whitespace" );
					return -1;
				}

				sym->flags |=  PPFLAG_CALLED | PPFLAG_WHITESPACE;
				plist_push( g->ws, sym );
			}
		}
		else
		{
			PARSEERROR( def, "nonterminal definition" );
			return -1;
		}
	}

	/* If no goal symbol provided, mark first nontermal as goal */
	if( !g->goal )
	{
		plist_for( g->symbols, e )
		{
			sym = (ppsym*)plist_access( e );

			if( sym->type == PPSYMTYPE_NONTERM )
			{
				g->goal = sym;
				break;
			}
		}

		if( !g->goal )
		{
			/* TODO: error reporting */
			fprintf( stderr, "Grammar must at least provide one "
								"nonterminal as goal symbol\n",
						g->goal->name );
			return -1;
		}
	}

	/* Goal symbol is always "called"! */
	g->goal->flags |= PPFLAG_CALLED;

	return defcount;
}

ppgram* pp_gram_free( ppgram* g )
{
	plistel*	e;
	ppsym*		s;
	ppprod*		p;

	if( !g )
		return (ppgram*)NULL;

	/* Erase symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		pfree( s->name );

		pfree( s->str );
		p_ccl_free( s->ccl );
		pregex_ptn_free( s->ptn );

		plist_free( s->prods );
		plist_free( s->first );
	}

	/* Erase productions */
	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		plist_free( p->rhs );
	}

	plist_free( g->symbols );
	plist_free( g->prods );
	plist_free( g->ws );
	pfree( g );

	return (ppgram*)NULL;
}

ppgram* pp_gram_create( char* def )
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
	int			id;
	int			cnt;
	int			pcnt;
	ppgram*		g;

	/* Setup grammar description */
	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ),
					PLIST_MOD_RECYCLE
						| PLIST_MOD_EXTKEYS
							| PLIST_MOD_UNIQUE );

	g->prods = plist_create( sizeof( ppprod ),
					PLIST_MOD_RECYCLE );
	g->ws = plist_create( sizeof( ppsym* ), PLIST_MOD_PTR );

	g->eof = pp_sym_create( g, PPSYMTYPE_SPECIAL, "eof", (char*)NULL );

	/* Parse grammar into description */
	if( def && *def && pp_gram_read( g, &def ) <= 0 )
		return pp_gram_free( g );

	/* Look for unique goal sequence */
	if( plist_count( g->goal->prods ) > 1 )
	{
		s = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, g->goal->name ), (char*)NULL );

		pp_prod_create( g, s, g->goal, (ppsym*)NULL );
		g->goal = s;
	}

	/* Set ID values for symbols and productions */
	for( id = 0, e = plist_first( g->symbols ); e; e = plist_next( e ), id++ )
	{
		s = (ppsym*)plist_access( e );
		s->id = id;
	}

	for( id = 0, e = plist_first( g->prods ); e; e = plist_next( e ), id++ )
	{
		p = (ppprod*)plist_access( e );
		p->id = id;
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

					if( s->type == PPSYMTYPE_NONTERM )
					{
						/* Union first set */
						plist_union( cp->lhs->first, s->first );

						/* Put prods on stack */
						plist_for( s->prods, ep )
						{
							p = (ppprod*)plist_access( ep );

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

	/* Pull-trough all lexem symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		if( s->type != PPSYMTYPE_NONTERM 
				|| !( s->flags & PPFLAG_LEXEM ) )
			continue;

		plist_push( call, s );
	}

	while( plist_pop( call, &s ) )
	{
		plist_push( done, s );

		plist_for( s->prods, e )
		{
			p = (ppprod*)plist_access( e );
			plist_for( p->rhs, er )
			{
				s = (ppsym*)plist_access( er );
				if( s->type == PPSYMTYPE_NONTERM )
				{
					s->flags |= PPFLAG_LEXEM;

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

	return g;
}

void pp_gram_print( ppgram* g )
{
	plistel*	e;
	plistel*	f;

	ppprod*		p;
	ppsym*		s;
	int			maxlhslen	= 0;

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		if( s->type == PPSYMTYPE_NONTERM && pstrlen( s->name ) > maxlhslen )
			maxlhslen = pstrlen( s->name );
	}

	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		printf( "%s%s%s%s%s%s %-*s : ",
			g->goal == p->lhs ? "G" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			p->lhs->flags & PPFLAG_EMIT ? "E" : " ",
			p->lhs->flags & PPFLAG_LEXEM ? "X" : " ",
			p->lhs->flags & PPFLAG_WHITESPACE ? "W" : " ",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (ppsym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				printf( " " );

			printf( "%s", pp_sym_to_str( s ) );
		}

		printf( "\n" );
	}

	printf( "\n" );

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		if( s->type != PPSYMTYPE_NONTERM )
			continue;

		printf( "FIRST %-*s {", maxlhslen, s->name );
		plist_for( s->first, f )
		{
			s = (ppsym*)plist_access( f );
			printf( " " );
			printf( "%s", pp_sym_to_str( s ) );
		}

		printf( " }\n" );
	}
}

