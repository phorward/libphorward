/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	rd.c
Usage:	Recursive Descent Parser Generator Test
		This is a test implementation for research and better understanding.
---------------------------------------------------------------------------- */

#include "phorward.h"

#define PARSEFUNC	"parse_"

typedef enum
{
	PGPTNTYPE_RULE,
	PGPTNTYPE_CCL,
	PGPTNTYPE_STRING,
	PGPTNTYPE_REF,
	PGPTNTYPE_ALT
} pgptntype;

static char	types[][6+1]	= { "rule ", "char ", "string", "ref  ", "alt  " };

typedef struct _pgptn	pgptn;

struct _pgptn
{
	pgptn*		parent;			/* Link to named parent */
	pgptntype	type;

	int			id;
	char*		name;
	pboolean	used;
	pboolean	defined;
	pboolean	emit;

	char		mod;
#define MOD_NONE		'\0'
#define MOD_OPTIONAL	'?'
#define MOD_KLEENE		'*'
#define MOD_POSITIVE	'+'

	pboolean	lrec;
	pboolean	nullable;

	union
	{
		pccl*		ccl;
		char*		str;
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
	enum
	{
		PGMATCH_BEGIN,
		PGMATCH_END
	} 			type;

	pgptn*		ptn;

	char*		start;
	char*		end;
} pgmatch;

/* Patterns */

static pgptn* pg_ptn_create( pgpar* par, pgptntype type, char* name )
{
	pgptn*		ptn;
	plistel*	e;
	pgptn*		parent;

	if( *pgetstr( name ) )
		ptn = (pgptn*)plist_access(
						plist_insert( par->ptns, (plistel*)NULL,
										name, (void*)NULL ) );
	else
	{
		ptn = (pgptn*)plist_malloc( par->ptns );

		for( e = plist_last( par->ptns ); e; e = plist_prev( e ) )
		{
			parent = (pgptn*)plist_access( e );

			if( parent->name )
			{
				ptn->parent = parent;
				break;
			}
		}
	}

	ptn->id = plist_count( par->ptns ) - 1;
	ptn->type = type;
	ptn->name = name;

	return ptn;
}

static pgptn* pg_ptn_create_char( pgpar* par, char* str )
{
	pgptn*	ptn;

	if( !( str && *str ) )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( par, PGPTNTYPE_CCL, (char*)NULL );

	if( *str == '^' )
	{
		ptn->att.ccl = p_ccl_create( 0, 255, str + 1 );
		p_ccl_negate( ptn->att.ccl );
	}
	else
		ptn->att.ccl = p_ccl_create( 0, 255, str );

	return ptn;
}

static pgptn* pg_ptn_create_string( pgpar* par, char* str )
{
	pgptn*	ptn;

	if( !( str && *str ) )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( par, PGPTNTYPE_STRING, (char*)NULL );
	ptn->att.str = str;

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

	sprintf( idname, "rule_%02d", ptn->id );
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

		if( ptn->type == PGPTNTYPE_CCL )
		{
			printf( " \'%s\'\n", p_ccl_to_str( ptn->att.ccl, TRUE ) );
		}
		else if( ptn->type == PGPTNTYPE_STRING )
			printf( " \"%s\"\n", ptn->att.str );
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

	if( start->type == PGPTNTYPE_CCL
			|| start->type == PGPTNTYPE_STRING
				|| start->type == PGPTNTYPE_REF )
		return;

	printf( "(%s) %s :", types[ start->type ], pg_ptn_get_name( start ) );

	/* Print sequence */
	for( ptn = start->att.ptn; ptn; ptn = ptn->next )
	{
		switch( ptn->type )
		{
			case PGPTNTYPE_CCL:
				printf( " '%c'", p_ccl_to_str( ptn->att.ccl, TRUE ) );
				break;

			case PGPTNTYPE_STRING:
				printf( " \"%s\"", ptn->att.str );
				break;

			case PGPTNTYPE_REF:
				printf( " %s", pg_ptn_get_name( ptn->att.ptn ) );
				break;

			default:
				printf( " %s", pg_ptn_get_name( ptn ) );
				break;
		}

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
	pgptn*		fp;
	pgptn*		child;
	plist*		closure;
	plist*		done;
	int			i;

	closure = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	for( i = 0;
			( p = wp = (pgptn*)plist_access( plist_get( par->ptns, i ) ) );
				i++ )
	{
		/* printf( "\n--- %s ---\n\n", pg_ptn_get_name( wp ) ); */
		while( p )
		{
			/* printf( "while( %s )\n", pg_ptn_get_name( p ) ); */

			switch( p->type )
			{
				case PGPTNTYPE_CCL:
				case PGPTNTYPE_STRING:
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

	if( isalpha( **def ) )
	{
		start = end = *def;

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
	else if( **def == '\"' )
	{
		start = end = ++(*def);

		while( *end != '\"' )
		{
			if( *end == '\\' )
				end++;

			end++;
		}

		*def = end + 1;

		factor = pg_ptn_create_string( par, pstrndup( start, end - start ) );
	}
	else if( **def == '\'' )
	{
		start = end = ++(*def);

		while( *end != '\'' )
		{
			if( *end == '\\' )
				end++;

			end++;
		}

		*def = end + 1;

		factor = pg_ptn_create_char( par, pstrndup( start, end - start ) );
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

static pboolean is_lexem( pgptn* ptn )
{
	if( !ptn->name )
		return FALSE;

	if( strcmp( ptn->name, "ident" ) == 0
		|| strcmp( ptn->name, "constant" ) == 0
		|| strcmp( ptn->name, "string" ) == 0 )
		return TRUE;

	return FALSE;
}

static void pg_print_ast( plist* ast )
{
	plistel*	e;
	pgmatch*	match;
	char		gap		[ 2048 + 1 ];

	*gap = '\0';

	plist_for( ast, e )
	{
		match = (pgmatch*)plist_access( e );

		if( match->type == PGMATCH_END )
			gap[ strlen( gap ) - 1 ] = '\0';

		printf( "%s%s %-10s",
				gap,
				( match->type == PGMATCH_BEGIN ? "[" : "]" ),
				pg_ptn_get_name( match->ptn ) );

		if( is_lexem( match->ptn ) )
			printf( " >%.*s<\n", match->end - match->start, match->start );
		else
			printf( "\n" );

		if( match->type == PGMATCH_BEGIN )
			strcat( gap, " " );
	}
}

static pboolean pg_par_exec( pgptn* ptn, plist* cache, plist* ast,
								char* start, char** stop )
{
	static int	lev		= -1;
	pgptn*		base	= ptn;
	int			i;
	pboolean	ok;
	pboolean	accept	= TRUE;
	pboolean	loop;
	char*		end		= start;
	char		gap		[ 2048 + 1 ];
	pgmatch*	handle	= (pgmatch*)NULL;
	plistel*	pbegin	= (plistel*)NULL;
	plistel*	ploop	= (plistel*)NULL;
	pgmatch*	mbegin	= (pgmatch*)NULL;
	pgmatch*	mloop;
	pgmatch*	mend;
	int			count	= 0;
	plistel*	e;
	plistel*	ne;

	lev++;

	if( !base )
		return TRUE;

#if 0
	sprintf( gap, "%2d %-7s", lev, pg_ptn_get_name( base ) );

	for( i = 10; i < 10 + lev; i++ )
		gap[ i ] = ' ';
	gap[ i ] = '\0';
#endif
	gap[ 0 ] = '\0';

	printf( "BEGIN >%.30s<\n", end );

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
		handle->start = end;
		handle->end = (char*)NULL;

		printf( "%sopening handle for %s\n",
			gap, base->name );
	}

	if( base->name )
	{
		pbegin = plist_insert( ast, (plistel*)NULL, (char*)NULL, (void*)NULL );
		mbegin = (pgmatch*)plist_access( pbegin );

		mbegin->type = PGMATCH_BEGIN;
		mbegin->ptn = base;
		mbegin->start = start;
	}

again:

	for( ; accept && ptn; ptn = ptn->next )
	{
		accept = FALSE;

		do
		{
			loop = FALSE;
			ok = FALSE;

			if( ptn->type == PGPTNTYPE_CCL )
				printf( "%s%s %s '%s' >%.3s<\n",
					gap, types[ ptn->type ], pg_ptn_get_name( ptn ),
						p_ccl_to_str( ptn->att.ccl, TRUE ), end );
			else if( ptn->type == PGPTNTYPE_STRING )
				printf( "%s%s %s \"%s\" >%.*s<\n",
					gap, types[ ptn->type ], pg_ptn_get_name( ptn ),
						ptn->att.str, strlen( ptn->att.str ), end );
			else
				printf( "%s%s %s >%.3s<\n",
					gap, types[ ptn->type ], pg_ptn_get_name( ptn ), end );


			switch( ptn->type )
			{
				case PGPTNTYPE_CCL:
					if( p_ccl_test( ptn->att.ccl, *end ) )
					{
						printf( "%sREAD --- >%c< ---\n", gap, *end );
						ok = TRUE;
						end++;
					}
					break;

				case PGPTNTYPE_STRING:
					if( !strncmp( ptn->att.str, end, strlen( ptn->att.str ) ) )
					{
						ok = TRUE;
						end += strlen( ptn->att.str );
					}
					break;

				case PGPTNTYPE_RULE:
					ok = pg_par_exec( ptn->att.ptn, cache, ast, end, &end );
					break;

				case PGPTNTYPE_REF:
					while( isspace( *end ) )
						end++;

					ok = pg_par_exec( ptn->att.ptn, cache, ast, end, &end );
					break;

				case PGPTNTYPE_ALT:
				{
					pgptn*		alt;

					for( alt = ptn->att.ptn; !ok && alt; alt = alt->next )
					{
						printf( "%strying alt %s\n",
							gap, pg_ptn_get_name( alt ) );

						ok = pg_par_exec( alt->att.ptn, cache, ast, end, &end );
						printf( "%salt %s is %s\n",
							gap, pg_ptn_get_name( alt ), BOOLEAN_STR( ok ) );
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
				*stop = handle->end = end;
				ptn = base;
				end = start;

				printf( "%saccepting >%.*s<, lrec-loop\n",
							gap, *stop - start, start );

				if( pbegin )
				{
					if( !ploop )
						ploop = pbegin;

					ploop = plist_insert( ast, pbegin,
								(char*)NULL, (void*)NULL );
					mloop = (pgmatch*)plist_access( ploop );

					mloop->type = PGMATCH_BEGIN;
					mloop->ptn = base;
					mloop->start = start;
					mloop->end = *stop;

					mend = (pgmatch*)plist_malloc( ast );
					mend->type = PGMATCH_END;
					mend->ptn = base;
					mend->start = start;
					mend->end = *stop;
				}

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

	if( count > 0 )
	{
		if( pbegin )
		{
			plist_remove( ast, pbegin );
			mbegin = (pgmatch*)NULL;
		}

		accept = TRUE;
	}

	if( !accept && pbegin )
	{
		for( e = pbegin; e; e = ne )
		{
			ne = plist_next( e );
			plist_remove( ast, e );
		}

		mbegin = (pgmatch*)NULL;
	}

	if( accept && mbegin )
	{
		mbegin->end = *stop;

		mend = (pgmatch*)plist_malloc( ast );
		mend->type = PGMATCH_END;
		mend->ptn = base;
		mend->start = start;
		mend->end = *stop;
	}

	lev--;

	return accept;
}

size_t pg_par_run( pgpar* par, char* src )
{
	char*		end;
	plist*		cache;
	plist*		ast;

	cache = plist_create( sizeof( pgmatch ), PLIST_MOD_RECYCLE );
	ast = plist_create( sizeof( pgmatch ), PLIST_MOD_RECYCLE );

	if( pg_par_exec( par->start, cache, ast, src, &end ) )
	{
		printf( "Parsed >%.*s<\n", end - src, src );

		pg_print_ast( ast );
		return end - src;
	}

	printf( "Parse error\n" );
	return 0;
}

/* Main */
#if 0
int main( int argc, char** argv )
{
	int		l;
	pgpar*	par;
	char*	grammar;
	char*	input;

	if( argc < 2 )
	{
		printf( "usage: %s GRAMMAR INPUT\n", *argv );
		return 1;
	}

	if( map_file( &grammar, argv[ 1 ] ) != ERR_OK )
	{
		fprintf( stderr, "Can't read '%s'\n", argv[ 1 ] );
		return 1;
	}

	par = pg_par_create( grammar );
	pg_ptn_find_lrec( par );

	pg_ptn_print( par, pg_ptn_printhier );
	/*
	pg_ptn_print( par, pg_ptn_printbnf );
	pg_par_to_c( par );
	*/

	if( map_file( &input, argv[ 2 ] ) != ERR_OK )
	{
		fprintf( stderr, "Can't read '%s'\n", argv[ 2 ] );
		return 1;
	}

	pg_par_run( par, input );
	return 0;
}
#endif

#if 1
int main( int argc, char** argv )
{
	int		l;
	pgpar*	par;

#if 0
	par = pg_par_create(
		"expr : term ( '+' term | '-' term )* ;"
		"term : factor ( '*' factor | '/' factor )* ;"
		"factor: '0-9'+ | '(' expr ')' ;" );
#else
	/*
	par = pg_par_create(
		"expr : a? expr '+' term | expr '-' term | term;"
		"term : term '*' factor | term '/' factor | factor;"
		"factor: '0-9'+ | '(' @expr ')' ;" );
	*/
	par = pg_par_create(
		"e : e '+' t | t;"
		"t : t '*' f | f;"
		"f: '0-9'+ | '(' e ')' ;" );

#endif

	pg_ptn_find_lrec( par );

	if( argc > 1 )
		pg_par_run( par, argv[1] );

	return 0;
}
#endif
