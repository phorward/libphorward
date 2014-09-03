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

ppsym* pp_sym_create( ppgram* g, ppsymtype type, char* name, char* def )
{
	ppsym*	sym;

	if( !g )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	name = pstrdup( name );

	if( !( sym = (ppsym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		fprintf( stderr, "Symbol '%s' already exists\n", name );
		pfree( name );
		return (ppsym*)NULL;
	}

	switch( ( sym->type = type ) )
	{
		case PPSYMTYPE_NONTERM:
			sym->productions = plist_create( 0, PLIST_MOD_PTR  );
			break;

		case PPSYMTYPE_CCL:
			sym->ccl = p_ccl_create( 0, 255, def );
			name = pstrdup( p_ccl_to_str( sym->ccl, TRUE ) );
			break;

		case PPSYMTYPE_STRING:
			name = pstrdup( def );
			break;

		default:
			break;
	}

	sym->id = plist_count( g->symbols ) - 1;
	sym->name = name;
	sym->first = plist_create( 0, PLIST_MOD_PTR );

	if( sym->type != PPSYMTYPE_NONTERM )
		plist_push( sym->first, sym );

	return sym;
}

pboolean pp_prod_append( ppprod* p, ppsym* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	plist_push( p->rhs, sym );
	return TRUE;
}

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

	prod = (ppprod*)plist_malloc( g->productions );

	prod->id = plist_count( g->productions ) - 1;
	prod->lhs = lhs;
	prod->rhs = plist_create( 0, PLIST_MOD_PTR );

	plist_push( lhs->productions, prod );

	va_start( varg, lhs );

	while( ( sym = va_arg( varg, ppsym* ) ) )
		pp_prod_append( prod, sym );

	va_end( varg );

	return prod;
}

ppsym* pp_prod_getfromrhs( ppprod* p, int off )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get( p->rhs, off ) );
}

char* pp_prod_to_str( ppprod* p )
{
	plistel*	e;
	ppsym*		sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	p->strval = (char*)pfree( p->strval );

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

	sprintf( deriv, "%s'", base );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !plist_get_by_key( g->symbols, deriv ) )
			return deriv;

		strcat( deriv, "'" );
	}

	return (char*)NULL;
}

static pboolean parse_factor( ppgram* g, ppsym* lhs, ppprod* p, char** def )
{
	ppsym*		sym		= (ppsym*)NULL;
	ppsym*		mod;
	char*		start;
	char*		end;
	char		name	[ NAMELEN + 1 ];
	char		stopch;
	int			i;

	SKIPWHITE();

	if( isalpha( **def ) )
	{
		start = end = *def;

		while( ( isalnum( *end ) || *end == '_' )
				&& ( end - start < NAMELEN ) )
			end++;

		sprintf( name, "%.*s", end - start, start );
		*def = end;

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
	else if( **def == '\"' || **def == '\'' )
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
			sym = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, name );
		else
			sym = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, name );
	}
	else
	{
		return TRUE;
	}

	SKIPWHITE();
	if( **def == '*' || **def == '+' || **def == '?' )
	{
		if( **def == '*' || **def == '+' )
		{
			mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

			pp_prod_create( g, mod, mod, sym, (ppsym*)NULL );
			pp_prod_create( g, mod, sym, (ppsym*)NULL );

			sym = mod;
		}

		if( **def == '?' || **def == '*' )
		{
			mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

			pp_prod_create( g, mod, sym, (ppsym*)NULL );
			pp_prod_create( g, mod, (ppsym*)NULL );

			sym = mod;
		}

		(*def)++;
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
		if( **def == ';' || **def == '|' || **def == ')' )
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
	char*		start;
	char*		end;
	ppsym*		nonterm;
	int			defcount	= 0;
	char		name		[ NAMELEN + 1 ];

	while( *def )
	{
		SKIPWHITE();

		start = end = *def;

		if( ! *start )
			break;

		if( isalpha( *start ) || *start == '_' )
		{
			while( ( isalnum( *end ) || *end == '_' )
					&& end - start < NAMELEN )
				end++;

			*def = end;
			sprintf( name, "%.*s", end - start, start );

			if( !( nonterm = (ppsym*)plist_access(
										plist_get_by_key(
											g->symbols, name ) ) ) )
				nonterm = pp_sym_create( g, PPSYMTYPE_NONTERM,
											name, (char*)NULL );

			nonterm->flags |= PPFLAG_DEFINED;

			if( !g->goal )
			{
				g->goal = nonterm;
				nonterm->flags |= PPFLAG_CALLED;
			}

			SKIPWHITE();
			if( !read_char( def, ':' ) )
				return -1;

			if( !parse_alternatives( g, nonterm, def ) )
				return -1;

			if( !read_char( def, ';' ) )
				return -1;

			defcount++;
		}
		else
		{
			PARSEERROR( def, "nonterminal definition" );
			return -1;
		}
	}

	return defcount;
}

ppgram* pp_gram_free( ppgram* g )
{
	plistel*	e;
	ppsym*		sym;
	ppprod*		prod;

	if( !g )
		return (ppgram*)NULL;

	/* Erase symbols */
	plist_for( g->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		pfree( sym->name );

		plist_free( sym->productions );
		plist_free( sym->first );
		p_ccl_free( sym->ccl );
	}

	plist_free( g->symbols );

	/* Erase productions */
	plist_for( g->productions, e )
	{
		prod = (ppprod*)plist_access( e );

		plist_free( prod->rhs );
		pfree( prod->strval );
	}

	plist_free( g->productions );

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
	int			cnt			= 0;
	int			pcnt;
	ppgram*		g;

	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ),
					PLIST_MOD_EXTKEYS | PLIST_MOD_UNIQUE );
	g->productions = plist_create( sizeof( ppprod ), PLIST_MOD_NONE );

	g->eof = pp_sym_create( g, PPSYMTYPE_SPECIAL, "eof", (char*)NULL );

	if( def && *def && pp_gram_read( g, &def ) <= 0 )
		return pp_gram_free( g );

	/* Look for unique goal sequence */
	if( plist_count( g->goal->productions ) > 1 )
	{
		s = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, g->goal->name ), (char*)NULL );

		pp_prod_create( g, s, g->goal, (ppsym*)NULL );
		g->goal = s;
	}

	/* Compute FIRST sets and mark left-recursions */
	call = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	do
	{
		pcnt = cnt;
		cnt = 0;

		plist_for( g->productions, e )
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

						/* Put productions on stack */
						plist_for( s->productions, ep )
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

	plist_free( call );
	plist_free( done );

	return g;
}

void pp_sym_print( ppsym* s )
{
	if( !s )
		return;

	switch( s->type )
	{
		case PPSYMTYPE_NONTERM:
			printf( "%s", s->name );
			break;

		case PPSYMTYPE_CCL:
			printf( "'%s'", s->name );
			break;

		case PPSYMTYPE_STRING:
			printf( "\"%s\"", s->name );
			break;

		case PPSYMTYPE_SPECIAL:
			printf( "@%s", s->name );
			break;

		default:
			MISSINGCASE;
			break;
	}
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

	plist_for( g->productions, e )
	{
		p = (ppprod*)plist_access( e );
		printf( "%s%s%s   %-*s : ",
			g->goal == p->lhs ? "G" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (ppsym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				printf( " " );

			pp_sym_print( s );
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
			pp_sym_print( s );
		}

		printf( " }\n" );
	}
}

