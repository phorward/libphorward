/*
	Recursive Descent Parser Generator Test
	This is a test source for some better understanding.

	Compile from here with
	cc -g -o rd -I ../src/ rd.c ../src/libphorward.a 
*/


#include "phorward.h"

typedef enum
{
	PGPTNTYPE_NULL,
	PGPTNTYPE_CHAR,
	PGPTNTYPE_VAR,
	PGPTNTYPE_KLEENE,
	PGPTNTYPE_POSITIVE,
	PGPTNTYPE_OPTIONAL,
	PGPTNTYPE_SEQ,
	PGPTNTYPE_ALT
} pgptntype;

typedef struct _pgptn	pgptn;
typedef struct _pgvar	pgvar;
typedef struct _pgpar	pgpar;

struct _pgptn
{
	pgptntype	type;
	pboolean	lrec;

	union
	{
		struct
		{
			wchar_t	from;
			wchar_t	to;
		} range;

		pgvar*		var;
	}			att;

	pgptn*		child;
	pgptn*		next;
};

struct _pgvar
{
	char*		name;
	pgptn*		def;

	pboolean	used;
	pboolean	defined;
};

struct _pgpar
{
	plist*		vars;
	pgvar*		start;
};

/* Patterns */

static pgptn* pg_ptn_create( pgptntype type )
{
	pgptn*	ptn;

	ptn = (pgptn*)pmalloc( sizeof( pgptn ) );
	ptn->type = type;

	return ptn;
}

static pgptn* pg_ptn_create_char( wchar_t from, wchar_t to )
{
	pgptn*	ptn;

	if( to > from )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_CHAR );
	ptn->att.range.from = from;
	ptn->att.range.to = to;

	return ptn;
}

static pgptn* pg_ptn_create_var( pgvar* var )
{
	pgptn*	ptn;

	if( !var )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_VAR );
	ptn->att.var = var;

	return ptn;
}

static pgptn* pg_ptn_create_kleene( pgptn* kptn )
{
	pgptn*	ptn;

	if( !kptn )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_KLEENE );
	ptn->child = kptn;

	return ptn;
}

static pgptn* pg_ptn_create_positive( pgptn* pptn )
{
	pgptn*	ptn;

	if( !pptn )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_POSITIVE );
	ptn->child = pptn;

	return ptn;
}

static pgptn* pg_ptn_create_optional( pgptn* optn )
{
	pgptn*	ptn;

	if( !optn )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_OPTIONAL );
	ptn->child = optn;

	return ptn;
}

static pgptn* pg_ptn_create_seq( pgptn* sptn, ... )
{
	pgptn*	ptn;
	pgptn*	prev	= sptn;
	pgptn*	next;
	va_list	args;

	if( !sptn )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_SEQ );
	ptn->child = sptn;

	va_start( args, sptn );

	while( ( next = va_arg( args, pgptn* ) ) )
	{
		prev->next = next;
		prev = next;
	}

	va_end( args );

	return ptn;
}

static pgptn* pg_ptn_create_alt( pgptn* aptn, ... )
{
	pgptn*	ptn;
	pgptn*	prev;
	va_list	args;

	if( !aptn )
	{
		WRONGPARAM;
		return (pgptn*)NULL;
	}

	ptn = pg_ptn_create( PGPTNTYPE_ALT );
	ptn->child = aptn;

	va_start( args, aptn );

	while( aptn )
	{
		if( aptn->next )
			aptn = pg_ptn_create_seq( aptn );

		if( ptn->child != aptn )
			prev->next = aptn;

		prev = aptn;
		aptn = va_arg( args, pgptn* );
	}

	va_end( args );

	return ptn;
}

static void pg_ptn_print( pgptn* ptn, int lev )
{
	int		i;

	static
	char	types[][4+1]	= { "null", "char", "var",
								"kle", "pos", "opt", "seq", "alt" };

	while( ptn )
	{
		for( i = 0; i < lev; i++ )
			printf( " " );

		printf( "%d ", ptn->lrec );

		if( ptn->type == PGPTNTYPE_CHAR )
		{
			if( ptn->att.range.from == ptn->att.range.to )
				printf( "%s >%c<(%d)\n",
							types[ ptn->type ], ptn->att.range.from,
													ptn->att.range.from );
			else
				printf( "%s >%c<(%d) - >%c<(%d)\n",
							types[ ptn->type ], ptn->att.range.from,
													ptn->att.range.from,
														ptn->att.range.to,
															ptn->att.range.to );
		}
		else if( ptn->type == PGPTNTYPE_VAR )
			printf( "%s %s\n",
				types[ ptn->type ], ptn->att.var->name );
		else
			printf( "%s\n", types[ ptn->type ] );

		pg_ptn_print( ptn->child, lev + 1 );
		ptn = ptn->next;
	}
}

pgptn* pg_ptn_free( pgptn* ptn )
{
	while( ptn )
	{
		pfree( ptn );

		pg_ptn_free( ptn->child );
		ptn = ptn->next;
	}

	return (pgptn*)NULL;
}

/* Find left-recursions */

static pboolean find_lrec( pgvar* var, pgptn* ptn )
{
	pgptn*	alt;

	switch( ptn->type )
	{
		case PGPTNTYPE_CHAR:
			break;

		case PGPTNTYPE_VAR:
			if( ptn->att.var == var )
			{
				ptn->lrec = TRUE;
				return TRUE;
			}
			break;

		case PGPTNTYPE_POSITIVE:
		case PGPTNTYPE_KLEENE:
		case PGPTNTYPE_OPTIONAL:
		case PGPTNTYPE_SEQ:
			if( ( ptn->lrec = find_lrec( var, ptn->child ) ) )
				return TRUE;

			break;

		case PGPTNTYPE_ALT:
			ptn->lrec = TRUE;

			for( alt = ptn->child; alt; alt = alt->next )
				if( !find_lrec( var, alt ) )
					ptn->lrec = FALSE;

			return ptn->lrec;

		default:
			MISSINGCASE;
			break;
	}

	return FALSE;
}

void find_lrecs( pgpar* par )
{
	plistel*	e;
	pgvar*		v;

	plist_for( par->vars, e )
	{
		v = (pgvar*)plist_access( e );
		find_lrec( v, v->def );
	}
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
	pgvar*		var;
	char*		start;
	char*		end;
	char*		name;
	char		stopch;

	SKIPWHITE();

	if( **def == '%' && isalpha( *( (*def) + 1 ) ) )
	{
		start = end = ++(*def);

		while( isalnum( *end ) || *end == '_' )
			end++;

		name = pstrndup( start, end - start );
		*def = end;

		if( !( var = (pgvar*)plist_access(
								plist_get_by_key( par->vars, name ) ) ) )
		{
			var = (pgvar*)plist_access( plist_insert( par->vars,
							(plistel*)NULL, name, (void*)NULL ) );

			var->name = name;
		}
		else
			pfree( name );

		var->used = TRUE;

		factor = pg_ptn_create_var( var );
	}
	else if( **def == '(' )
	{
		(*def)++;

		factor = parse_alternatives( par, def );

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

			factor = pg_ptn_create_char( **def, **def );
			(*def)++;

			if( !first )
				first = factor;
			else
				prev->next = factor;

			prev = factor;
		}

		(*def)++;

		if( first->next )
		{
			factor = pg_ptn_create( PGPTNTYPE_SEQ );
			factor->child = first;
		}
	}
	else if( isalnum( **def ) )
	{
		factor = pg_ptn_create_char( **def, **def );
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
		PARSEERROR( def, "%var, '(' or character" );
		return (pgptn*)NULL;
	}

	return factor;
}

static pgptn* parse_mod( pgpar* par, char** def )
{
	pgptn*		factor;
	pgptn*		mod;

	if( !( factor = parse_factor( par, def ) ) )
		return (pgptn*)NULL;

	SKIPWHITE();
	if( **def == '*' )
	{
		mod = pg_ptn_create_kleene( factor );
		(*def)++;
	}
	else if( **def == '+' )
	{
		mod = pg_ptn_create_positive( factor );
		(*def)++;
	}
	else if( **def == '?' )
	{
		mod = pg_ptn_create_optional( factor );
		(*def)++;
	}
	else
		mod = factor;

	return mod;
}

static pgptn* parse_sequence( pgpar* par, char** def )
{
	pgptn*		seq;
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

	if( !first )
		return (pgptn*)NULL;

	if( first->next )
	{
		seq = (pgptn*)pg_ptn_create( PGPTNTYPE_SEQ );
		seq->child = first;
	}
	else
		seq = first;

	return seq;
}

static pgptn* parse_alternatives( pgpar* par, char** def )
{
	pgptn*		alt;
	pgptn*		seq;
	pgptn*		first	= (pgptn*)NULL;
	pgptn*		prev;

	while( TRUE )
	{
		if( !( seq = parse_sequence( par, def ) ) )
		{
			pg_ptn_free( first );
			return (pgptn*)NULL;
		}

		if( !first )
			first = seq;
		else
			prev->next = seq;

		prev = seq;

		SKIPWHITE();

		if( **def != '|' )
			break;

		(*def)++;
	}

	alt = pg_ptn_create( PGPTNTYPE_ALT );
	alt->child = first;

	return alt;
}

static int parse_grammar( pgpar* par, char** def )
{
	char*		start;
	char*		end;
	char*		name;
	pgvar*		var;
	pgptn*		alt;
	pgptn*		next;
	plistel*	e;
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

			if( !( var = (pgvar*)plist_access(
									plist_get_by_key( par->vars, name ) ) ) )
			{
				var = (pgvar*)plist_access( plist_insert( par->vars,
								(plistel*)NULL, name, (void*)NULL ) );

				var->name = name;
			}
			else
				pfree( name );

			var->defined = TRUE;
			defcount++;

			if( !par->start )
			{
				par->start = var;
				var->used = TRUE;
			}

			SKIPWHITE();
			if( !read_char( def, ':' ) )
				return -1;

			if( !( alt = parse_alternatives( par, def ) ) )
				return -1;

			if( !var->def )
				var->def = alt;
			else
			{
				for( next = var->def->child; next->next; next = next->next )
					;

				next->next = alt;
			}

			if( !read_char( def, ';' ) )
				return -1;
		}
		else
		{
			PARSEERROR( def, "pattern definition" );
			return -1;
		}
	}

	find_lrecs( par );

	/* Debug */
	plist_for( par->vars, e )
	{
		var = (pgvar*)plist_access( e );

		printf( "--- %s ---\n", var->name );
		pg_ptn_print( var->def, 0 );
	}

	return defcount;
}

/* Init */

pgpar* pg_par_create( char* def )
{
	pgpar*	par;

	if( !( def && *def ) )
	{
		WRONGPARAM;
		return (pgpar*)NULL;
	}

	par = (pgpar*)pmalloc( sizeof( pgpar ) );
	par->vars = plist_create( sizeof( pgvar ),
								PLIST_MOD_RECYCLE | PLIST_MOD_EXTKEYS );

	parse_grammar( par, &def );

	return par;
}

/* Run */

static pboolean pg_par_exec( pgpar* par, pgptn* ptn, char* start, size_t* mlen )
{
	size_t	len;
	char*	end	= start;

	switch( ptn->type )
	{
		case PGPTNTYPE_CHAR:
			if( ptn->att.range.from <= *start &&
					ptn->att.range.to >= *start )
			{
				*mlen = 1;
				return TRUE;
			}

			break;

		case PGPTNTYPE_VAR:
			return pg_par_exec( par, ptn->att.var->def, start, mlen );

		case PGPTNTYPE_POSITIVE:
			if( !pg_par_exec( par, ptn->child, end, &len ) )
				return FALSE;

			end += len;

		case PGPTNTYPE_KLEENE:
			while( pg_par_exec( par, ptn->child, end, &len ) )
				end += len;

			*mlen = end - start;
			return TRUE;

		case PGPTNTYPE_OPTIONAL:
			pg_par_exec( par, ptn->child, end, mlen );
			return TRUE;

		case PGPTNTYPE_SEQ:
			for( ptn = ptn->child; ptn; ptn = ptn->next )
			{
				if( !pg_par_exec( par, ptn, end, &len ) )
					return FALSE;

				end += len;
			}

			*mlen = end - start;
			return TRUE;

		case PGPTNTYPE_ALT:
			for( ptn = ptn->child; ptn; ptn = ptn->next )
			{
				if( pg_par_exec( par, ptn, end, mlen ) )
					return TRUE;
			}
			break;

		default:
			MISSINGCASE;
			break;
	}

	return FALSE;
}

size_t pg_par_run( pgpar* par, char* src )
{
	size_t	len;

	if( pg_par_exec( par, par->start->def, src, &len ) )
	{
		printf( "Parsed >%.*s<\n", len, src );
		return len;
	}

	printf( "Parse error\n" );

	return 0;
}

/* Main */

int main()
{
	pgpar*	par;

	par = pg_par_create(
		"expr : %term ( '+' %term | '-' %term )* ;"
		"term : %factor ( '*' %factor | '/' %factor )* ;"
		"factor: 0-9+ | '(' %expr ')' ;" );
	/*
	par = pg_par_create(
		"expr : %expr '+' %term | %expr '-' %term | %term;"
		"term : %term '*' %factor | %term '/' %factor | %factor;"
		"factor: 0-9+ | '(' %expr ')' ;" );
	*/

	pg_par_run( par, "123+456*3+2" );
}

