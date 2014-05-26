/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	rd.c
Usage:	Recursive Descent Parser Generator Test
		This is a test source for some better understanding.

		Compile from here with
		cc -g -o rd -I ../src/ rd.c ../src/libphorward.a

---------------------------------------------------------------------------- */

#include "phorward.h"

typedef enum
{
	PGPTNTYPE_RULE,
	PGPTNTYPE_CHAR,
	PGPTNTYPE_REF,
	PGPTNTYPE_ALT
} pgptntype;

static char	types[][4+1]	= { "rule", "char", "ref ", "alt " };

typedef struct _pgptn	pgptn;

struct _pgptn
{
	pgptntype	type;

	int			id;
	char*		name;
	pboolean	used;
	pboolean	defined;

	char		mod;
#define MOD_NONE		'\0'
#define MOD_OPTIONAL	'?'
#define MOD_KLEENE		'*'
#define MOD_POSITIVE	'+'

	pboolean	lrec;
	pboolean	nullable;

	union
	{
		struct
		{
			wchar_t	from;
			wchar_t	to;
		} range;

		pgptn*		ptn;
	}			att;

	pgptn*		next;
};

typedef struct
{
	plist*		ptns;
	pgptn*		start;
} pgpar;

typedef struct
{
	pgptn*		ptn;

	char*		start;
	char*		end;
} pgmatch;

/* Patterns */

static pgptn* pg_ptn_create( pgpar* par, pgptntype type, char* name )
{
	pgptn*	ptn;

	if( *pgetstr( name ) )
		ptn = (pgptn*)plist_access(
						plist_insert( par->ptns, (plistel*)NULL,
										name, (void*)NULL ) );
	else
		ptn = (pgptn*)plist_malloc( par->ptns );

	ptn->id = plist_count( par->ptns ) - 1;
	ptn->type = type;
	ptn->name = name;

	return ptn;
}

static pgptn* pg_ptn_create_char( pgpar* par, wchar_t from, wchar_t to )
{
	pgptn*	ptn;

	if( to > from )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( par, PGPTNTYPE_CHAR, (char*)NULL );
	ptn->att.range.from = from;
	ptn->att.range.to = to;

	return ptn;
}

static pgptn* pg_ptn_create_rule( pgpar* par, char* name, pgptn* ptn )
{
	pgptn*	rule;

	rule = pg_ptn_create( par, PGPTNTYPE_RULE, name );
	rule->att.ptn = ptn;

	return rule;
}

static pgptn* pg_ptn_create_alt( pgpar* par, pgptn* chain )
{
	pgptn*	alt;

	alt = pg_ptn_create( par, PGPTNTYPE_ALT, (char*)NULL );
	alt->att.ptn = chain;

	return alt;
}

static pgptn* pg_ptn_create_ref( pgpar* par, pgptn* ref )
{
	pgptn*	ptn;

	ptn = pg_ptn_create( par, PGPTNTYPE_REF, (char*)NULL );
	ptn->att.ptn = ref;

	return ptn;
}

char* pg_ptn_get_name( pgptn* ptn )
{
	static char	idname[ 10+1 ];

	if( ptn->name )
		return ptn->name;

	sprintf( idname, "r%02d", ptn->id );
	return idname;
}

static void pg_ptn_printhier( pgptn* ptn )
{
	int		i;
	static
	int		lev;

	while( ptn )
	{
		for( i = 0; i < lev; i++ )
			printf( " " );

		printf( "%s%s %s%c %s", ptn->lrec ? "L" : " ",
								ptn->nullable ? "N" : " ",
								types[ ptn->type ],
								ptn->mod ? ptn->mod : ' ',
									pg_ptn_get_name( ptn ) );

		if( ptn->type == PGPTNTYPE_CHAR )
		{
			if( ptn->att.range.from == ptn->att.range.to )
				printf( " >%c<(%d)\n", ptn->att.range.from,
										ptn->att.range.from );
			else
				printf( " >%c<(%d) - >%c<(%d)\n",
									ptn->att.range.from,
										ptn->att.range.from,
											ptn->att.range.to,
												ptn->att.range.to );
		}
		else if( ptn->type == PGPTNTYPE_REF )
			printf( " %s\n", ptn->att.ptn->name );
		else
		{
			printf( "\n" );

			lev++;
			pg_ptn_printhier( ptn->att.ptn );
			lev--;
		}

		ptn = ptn->next;
	}
}

static void pg_ptn_printbnf( pgptn* start )
{
	pgptn*		ptn;

	if( start->type == PGPTNTYPE_CHAR
			|| start->type == PGPTNTYPE_REF )
		return;

	printf( "(%s) %s :", types[ start->type ], pg_ptn_get_name( start ) );

	/* Print sequence */
	for( ptn = start->att.ptn; ptn; ptn = ptn->next )
	{
		if( ptn->type == PGPTNTYPE_CHAR )
		{
			if( ptn->att.range.from == ptn->att.range.to )
				printf( " '%c'", ptn->att.range.from );
			else
				printf( " %c-%c", ptn->att.range.from, ptn->att.range.to );
		}
		else if( ptn->type == PGPTNTYPE_REF )
			printf( " %s", pg_ptn_get_name( ptn->att.ptn ) );
		else
			printf( " %s", pg_ptn_get_name( ptn ) );

		if( ptn->mod )
			printf( "%c", ptn->mod );

		/* ...but divide for alternation */
		if( start->type == PGPTNTYPE_ALT && ptn->next )
			printf( " |" );
	}

	printf( " ;\n" );

	/* Print subsequent rules */
	for( ptn = start->att.ptn; ptn; ptn = ptn->next )
		pg_ptn_printbnf( ptn );
}

static void pg_ptn_printc( pgptn* start )
{
	pgptn*		ptn;

	if( start->type == PGPTNTYPE_CHAR
			|| start->type == PGPTNTYPE_REF )
		return;

	printf( "BOOL %s( char* start, char** stop )\n", pg_ptn_get_name( start ) );
	printf( "{\n" );

	if( start->type == PGPTNTYPE_ALT )
	{
		for( ptn = start->att.ptn; ptn; ptn = ptn->next )
		{
			printf( "	%sif( %s( start, stop ) )\n",
							ptn != start->att.ptn ? "else " : "",
							pg_ptn_get_name( ptn ) );
			printf( "		return TRUE;\n" );
		}

		printf( "	return FALSE;\n" );
	}
	else
	{
		printf( "	char*	end		= start;\n" );

		/* Render sequence */
		for( ptn = start->att.ptn; ptn; ptn = ptn->next )
		{
			printf( "\n" );
			if( ptn->type == PGPTNTYPE_CHAR )
			{
				if( !ptn->mod
						|| ptn->mod == MOD_POSITIVE
							|| ptn->mod == MOD_OPTIONAL )
				{
					if( ptn->att.range.from == ptn->att.range.to )
						printf( "	if( *end == '%c' )\n"
								"		end++;\n", ptn->att.range.from );
					else
						printf( "	if( *end >= '%c' && *end <= '%c' )\n"
								"		end++;\n",
											ptn->att.range.from,
											ptn->att.range.to );

					if( ptn->mod != MOD_OPTIONAL )
						printf( "	else\n"
								"		return FALSE;\n" );

					if( ptn->mod == MOD_POSITIVE )
						printf( "\n" );
				}

				if( ptn->mod == MOD_KLEENE || ptn->mod == MOD_POSITIVE )
				{
					if( ptn->att.range.from == ptn->att.range.to )
						printf( "	while( *end == '%c' )\n"
								"		end++;\n",
											ptn->att.range.from );
					else
						printf( "	while( *end >= '%c' && *end <= '%c' )\n"
								"		end++;\n",
											ptn->att.range.from,
											ptn->att.range.to );
				}
			}
			else
			{
				if( ptn->mod == MOD_OPTIONAL )
				{
					if( ptn->type == PGPTNTYPE_REF )
						printf( "	%s( end, &end );\n",
									pg_ptn_get_name( ptn->att.ptn ) );
					else
						printf( "	%s( end, &end );\n",
									pg_ptn_get_name( ptn ) );
				}
				else if( !ptn->mod || ptn->mod == MOD_POSITIVE )
				{
					if( ptn->type == PGPTNTYPE_REF )
						printf( "	if( !%s( end, &end ) )\n"
								"		return FALSE;\n",
											pg_ptn_get_name( ptn->att.ptn ) );
					else
						printf( "	if( !%s( end, &end ) )\n"
								"		return FALSE;\n",
											pg_ptn_get_name( ptn ) );
				}

				if( ptn->mod == MOD_KLEENE || ptn->mod == MOD_POSITIVE )
				{
					if( ptn->type == PGPTNTYPE_REF )
						printf( "	while( %s( end, &end ) )\n"
								"		;\n",
											pg_ptn_get_name( ptn->att.ptn ) );
					else
						printf( "	while( %s( end, &end ) )\n"
								"		;\n",
											pg_ptn_get_name( ptn ) );
				}
			}
		}

		printf( "\n"
				"	*stop = end;\n"
				"	printf( \"%s matching >%%.*s<\\n\", end - start, start );\n"
				"	return TRUE;\n", pg_ptn_get_name( start ) );
	}

	printf( "}\n\n" );

	/* Print subsequent rules */
	for( ptn = start->att.ptn; ptn; ptn = ptn->next )
		pg_ptn_printc( ptn );
}

static void pg_ptn_printcproto( pgptn* start )
{
	pgptn*		ptn;

	if( start->type == PGPTNTYPE_CHAR
			|| start->type == PGPTNTYPE_REF )
		return;

	printf( "BOOL %s( char* start, char** stop );\n",
				pg_ptn_get_name( start ) );

	/* Print subsequent rules */
	for( ptn = start->att.ptn; ptn; ptn = ptn->next )
		pg_ptn_printcproto( ptn );
}

void pg_ptn_print( pgpar* par, void (*fn)( pgptn* ) )
{
	plistel*	e;
	pgptn*		p;

	plist_for( par->ptns, e )
	{
		p = (pgptn*)plist_access( e );
		if( p->name )
		{
			(*fn)( p );
			printf( "\n" );
		}
	}
}

void pg_ptn_find_lrec( pgpar* par )
{
	pgptn*		wp;
	pgptn*		p;
	pgptn*		child;
	plist*		closure;
	plist*		done;
	int			i;

	closure = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	for( i = 0; ( p = wp = (pgptn*)plist_access(
								plist_get( par->ptns, i ) ) ); i++ )
	{
		/* printf( "\n--- %s ---\n\n", pg_ptn_get_name( wp ) ); */

		while( p )
		{
			/* printf( "while( %s )\n", pg_ptn_get_name( p ) ); */

			switch( p->type )
			{
				case PGPTNTYPE_CHAR:
					break;

				case PGPTNTYPE_ALT:
				case PGPTNTYPE_RULE:
				case PGPTNTYPE_REF:
					for( child = p->att.ptn; child;
							child = child->next )
					{
						if( child == wp )
							wp->lrec = TRUE;
						else if( !plist_get_by_ptr( done, child ) )
							plist_push( closure, child );

						if( p->type != PGPTNTYPE_ALT
								&& !child->nullable )
							break;
					}
					break;
			}

			if( p->nullable )
			{
				if( !( p = p->next ) )
				{
					if( !wp->nullable )
						wp->nullable = TRUE;
				}
				else
					continue;
			}

			if( plist_pop( closure, &p ) )
				plist_push( done, p );
		}

		plist_clear( done );
	}

	plist_free( closure );
	plist_free( done );
}

pgptn* pg_ptn_free( pgptn* ptn )
{
	pgptn*	next	= ptn;

	while( next )
	{
		pfree( ptn->name );
		next = ptn->next;
		pfree( ptn );
	}

	return (pgptn*)NULL;
}

/* Parse BNF */

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

static pgptn* parse_alternatives( pgpar* par, char** def );

static pgptn* parse_factor( pgpar* par, char** def )
{
	pgptn*		first		= (pgptn*)NULL;
	pgptn*		prev;
	pgptn*		factor;
	pgptn*		ref;
	char*		start;
	char*		end;
	char*		name;
	char		stopch;

	SKIPWHITE();

	if( **def == '@' && isalpha( *( (*def) + 1 ) ) )
	{
		start = end = ++(*def);

		while( isalnum( *end ) || *end == '_' )
			end++;

		name = pstrndup( start, end - start );
		*def = end;

		if( !( ref = (pgptn*)plist_access(
								plist_get_by_key( par->ptns, name ) ) ) )
			ref = pg_ptn_create_rule( par, name, (pgptn*)NULL );
		else
			pfree( name );

		ref->used = TRUE;

		factor = pg_ptn_create_ref( par, ref );
	}
	else if( **def == '(' )
	{
		(*def)++;

		factor = pg_ptn_create_rule( par, (char*)NULL,
					parse_alternatives( par, def ) );

		if( !read_char( def, ')' ) )
		{
			pg_ptn_free( factor );
			return (pgptn*)NULL;
		}
	}
	else if( **def == '\'' || **def == '\"' )
	{
		stopch = *( (*def)++ );

		while( **def != stopch )
		{
			if( **def == '\\' )
				(*def)++;

			factor = pg_ptn_create_char( par, **def, **def );
			(*def)++;

			if( !first )
				first = factor;
			else
				prev->next = factor;

			prev = factor;
		}

		(*def)++;

		if( first->next )
			factor = pg_ptn_create_rule( par, (char*)NULL, first );
	}
	else if( isalnum( **def ) )
	{
		factor = pg_ptn_create_char( par, **def, **def );
		(*def)++;

		/* is range? */
		if( **def == '-' && isalnum( *( *def + 1 ) ) )
		{
			(*def)++;
			factor->att.range.to = **def;
			(*def)++;
		}

		/* Switch if wrong order */
		if( factor->att.range.to < factor->att.range.from )
		{
			stopch = factor->att.range.to;
			factor->att.range.to = factor->att.range.from;
			factor->att.range.from = stopch;
		}
	}
	else
	{
		factor = pg_ptn_create_rule( par, (char*)NULL, (pgptn*)NULL );
		factor->nullable = TRUE;
	}

	return factor;
}

static pgptn* parse_mod( pgpar* par, char** def )
{
	pgptn*		factor;

	if( !( factor = parse_factor( par, def ) ) )
		return (pgptn*)NULL;

	SKIPWHITE();
	if( **def == '*' || **def == '+' || **def == '?' )
	{
		factor->mod = **def;

		if( **def == '*' || **def == '?' )
			factor->nullable = TRUE;

		(*def)++;
	}

	return factor;
}

static pgptn* parse_sequence( pgpar* par, char** def )
{
	pgptn*		mod;
	pgptn*		first	= (pgptn*)NULL;
	pgptn*		prev;

	while( ( mod = parse_mod( par, def ) ) )
	{
		if( !first )
			first = mod;
		else
			prev->next = mod;

		prev = mod;

		SKIPWHITE();
		if( **def == ';' || **def == '|' || **def == ')' )
			break;
	}

	return first;
}

static pgptn* parse_alternatives( pgpar* par, char** def )
{
	pgptn*		seq;
	pgptn*		fseq	= (pgptn*)NULL;
	pgptn*		first	= (pgptn*)NULL;
	pgptn*		rule;

	while( TRUE )
	{
		seq = parse_sequence( par, def );

		if( fseq && seq )
		{
			if( !first )
				first = rule = pg_ptn_create_rule( par, (char*)NULL, fseq );

			rule->next = pg_ptn_create_rule( par, (char*)NULL, seq );
			rule = rule->next;
		}
		else
			fseq = seq;

		SKIPWHITE();

		if( **def != '|' )
			break;
		(*def)++;
	}

	if( !first )
		return fseq;

	return pg_ptn_create_alt( par, first );
}

static int parse_grammar( pgpar* par, char** def )
{
	char*		start;
	char*		end;
	char*		name;
	pgptn*		rule;
	pgptn*		next;
	int			defcount	= 0;

	while( *def )
	{
		SKIPWHITE();

		start = end = *def;

		if( ! *start )
			break;

		if( isalpha( *start ) || *start == '_' )
		{
			while( isalnum( *end ) || *end == '_' )
				end++;

			*def = end;
			name = pstrndup( start, end - start );

			if( !( rule = (pgptn*)plist_access(
									plist_get_by_key( par->ptns, name ) ) ) )
				rule = pg_ptn_create_rule( par, name, (pgptn*)NULL );
			else
				pfree( name );

			rule->defined = TRUE;

			if( !par->start )
				par->start = rule;

			SKIPWHITE();
			if( !read_char( def, ':' ) )
				return -1;

			if( !( rule->att.ptn = parse_alternatives( par, def ) ) )
				return -1;

			defcount++;

			if( !read_char( def, ';' ) )
				return -1;
		}
		else
		{
			PARSEERROR( def, "pattern definition" );
			return -1;
		}
	}

	return defcount;
}

/* Init */

pgpar* pg_par_create( char* def )
{
	pgpar*		par;

	if( !( def && *def ) )
	{
		WRONGPARAM;
		return (pgpar*)NULL;
	}

	par = (pgpar*)pmalloc( sizeof( pgpar ) );
	par->ptns = plist_create( sizeof( pgptn ),
					PLIST_MOD_RECYCLE | PLIST_MOD_EXTKEYS );

	parse_grammar( par, &def );

	return par;
}

/* Run */
static pgmatch* find_match( plist* cache, pgptn* ptn, char* start )
{
	plistel*	e;
	pgmatch*	m;

	for( e = plist_last( cache ); e; e = plist_prev( e ) )
	{
		m = (pgmatch*)plist_access( e );

		if( m->ptn == ptn && m->start == start )
			return m;
	}

	return (pgmatch*)NULL;
}

static pboolean pg_par_exec( pgptn* ptn, plist* cache,
								char* start, char** stop )
{
	static int	lev		= -1;
	pgptn*		base	= ptn;
	int			i;
	pboolean	ok;
	pboolean	accept	= TRUE;
	pboolean	loop;
	char*		end		= start;
	char		gap		[ 80 + 1 ];
	pgmatch*	handle	= (pgmatch*)NULL;
	int			count	= 0;

	sprintf( gap, "%-10s", pg_ptn_get_name( base ) );

	lev++;
	for( i = 10; i < 10 + lev; i++ )
		gap[ i ] = ' ';
	gap[ i ] = '\0';

	if( base->name )
	{
		if( ( handle = find_match( cache, base, start ) ) )
		{
			if( !handle->end )
			{
				printf( "%shandle on %s unfinished\n",
							gap, base->name );
				lev--;
				return FALSE;
			}

			printf( "%shandle on %s exists, accepting\n",
						gap, base->name );

			*stop = handle->end;
			lev--;
			return TRUE;
		}

		handle = (pgmatch*)plist_malloc( cache );
		handle->ptn = base;
		handle->start = start;
		handle->end = (char*)NULL;

		printf( "%sopening handle for %s\n", gap, base->name );
	}

again:

	for( ; accept && ptn; ptn = ptn->next )
	{
		accept = FALSE;

		do
		{
			loop = FALSE;
			ok = FALSE;

			printf( "%s%s %s >%s<\n", gap, types[ ptn->type ],
										pg_ptn_get_name( ptn ), end );

			switch( ptn->type )
			{
				case PGPTNTYPE_CHAR:
					if( *end >= ptn->att.range.from
						&& *end <= ptn->att.range.to )
					{
						printf( "%sread %c\n", gap, *end );
						ok = TRUE;
						end++;
					}
					break;

				case PGPTNTYPE_RULE:
					ok = pg_par_exec( ptn->att.ptn, cache, end, &end );
					break;

				case PGPTNTYPE_REF:
					ok = pg_par_exec( ptn->att.ptn, cache, end, &end );
					break;

				case PGPTNTYPE_ALT:
				{
					pgptn*		alt;

					for( alt = ptn->att.ptn; !ok && alt; alt = alt->next )
					{
						printf( "%strying alt %s at >%s< count %d\n",
							gap, pg_ptn_get_name( alt ), end, count );

						ok = pg_par_exec( alt->att.ptn, cache, end, &end );
					}

					break;
				}
			}

			switch( ptn->mod )
			{
				case MOD_POSITIVE:
					if( ok )
						loop = TRUE;
					break;

				case MOD_KLEENE:
					if( ok )
						loop = TRUE;

				case MOD_OPTIONAL:
					ok = TRUE;
					break;

			}

			accept |= ok;
		}
		while( loop );
	}

	if( accept )
	{
		if( handle )
		{
			printf( "%sclosing handle for %s\n", gap, base->name );

			if( base->lrec && ( !handle->end || end > handle->end ) )
			{
				printf( "%saccepting >%.*s<, lrec-loop\n",
							gap, *stop - start, start );

				*stop = handle->end = end;
				ptn = base;
				end = start;

				count++;
				goto again;
			}
			else if( !base->lrec )
				*stop = end;

			printf( "%saccepting >%.*s<, final match\n",
						gap, *stop - start, start );
		}
		else
		{
			printf( "%supdating length %d\n", gap, end - start );
			*stop = end;
		}
	}
	else if( count > 0 )
		accept = TRUE;

	lev--;

	return accept;
}

size_t pg_par_run( pgpar* par, char* src )
{
	char*	end;
	plist*	cache;

	cache = plist_create( sizeof( pgmatch ), PLIST_MOD_RECYCLE );

	if( pg_par_exec( par->start, cache, src, &end ) )
	{
		printf( "Parsed >%.*s<\n", end - src, src );
		return end - src;
	}

	printf( "Parse error\n" );
	return 0;
}

/* Emit a C program for parser */

pboolean pg_par_to_c( pgpar* par )
{
	if( !par )
	{
		WRONGPARAM;
		return FALSE;
	}

	printf(
		"#include <stdlib.h>\n"
		"#include <stdio.h>\n\n"

		"#define BOOL short\n"
		"#define TRUE 1\n"
		"#define FALSE 0\n\n" );

	pg_ptn_print( par, pg_ptn_printcproto );
	pg_ptn_print( par, pg_ptn_printc );

	printf(
		"int main( int argc, char** argv )\n"
		"{\n"
		"	char*	s;\n"
		"	char*	e;\n\n"
		"	if( argc < 2 )\n"
		"		return 1;\n\n"
		"	s = argv[1];\n"
		"	if( expr( s, &e ) )\n"
		"		printf( \">%%.*s<\\n\", e - s, s );\n\n"
		"	return 0;\n"
		"}\n\n" );

	return TRUE;
}

/* Main */

int main( int argc, char** argv )
{
	int		l;
	pgpar*	par;

#if 0
	par = pg_par_create(
		"expr : @term ( '+' @term | '-' @term )* ;"
		"term : @factor ( '*' @factor | '/' @factor )* ;"
		"factor: 0-9+ | '(' @expr ')' ;" );
#else
	par = pg_par_create(
		"expr : @expr '+' @term | @expr '-' @term | @term;"
		"term : @term '*' @factor | @term '/' @factor | @factor;"
		"factor: 0-9+ | '(' @expr ')' ;" );
#endif

	pg_ptn_find_lrec( par );
	pg_ptn_print( par, pg_ptn_printhier );
	pg_ptn_print( par, pg_ptn_printbnf );

	/* pg_par_to_c( par ); */

	//pg_par_run( par, "123+456*3+699" );
	if( argc > 1 )
		pg_par_run( par, argv[1] );

	return 0;
}

