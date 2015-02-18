/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ppgram2c.c
Usage:	ppgram to C-code generator
----------------------------------------------------------------------------- */

#include "phorward.h"

char*	emptystr	= "";
char*	gname		= "g";
char*	symname		= "sym";
char*	indent;

void help( char** argv )
{
	printf( "usage: %s OPTIONS grammar\n\n"

	"   -h  --help              Show this help, and exit.\n"
	"   -i  --indent NUM        Indent generated code by NUM tabs\n"
	"   -o  --output FILE       Output to FILE; Default is stdout.\n", *argv );
}

char* cident( char* ident )
{
	return ident;
}

void gen_decl( FILE* f, ppgram* g )
{
	/*
	fprintf( f, "%sppgram*	%s;\n", indent, gname );
	*/
	fprintf( f, "%sppsym*	%s[ %d ];\n\n",
		indent, symname, plist_count( g->symbols ) );

	/*
	fprintf( f, "%s%s = pp_gram_create( (char*)NULL );\n\n", indent, gname );
	*/
}

void gen_sym( FILE* f, ppgram* g )
{
	int		i;
	ppsym*	sym;
	char*	def;
	char*	name;
	char*	symtypes[]	= { "PPSYMTYPE_NONTERM",
							"PPSYMTYPE_CCL",
							"PPSYMTYPE_STRING",
							"PPSYMTYPE_REGEX",
							"PPSYMTYPE_SPECIAL" };

	fprintf( f, "%s/* Symbols */\n\n", indent );

	for( i = 0; sym = pp_sym_get( g, i ); i++ )
	{
		switch( sym->type )
		{
			case PPSYMTYPE_SPECIAL:
				/* Ignore special symbols */
				continue;

			case PPSYMTYPE_CCL:
				def = p_ccl_to_str( sym->ccl, TRUE );
				break;

			case PPSYMTYPE_STRING:
				def = sym->str;
				break;

			case PPSYMTYPE_REGEX:
				def = pregex_ptn_to_regex( sym->re->ptn );
				break;

			default:
				def = (char*)NULL;
				break;
		}

		if( ( name = sym->name ) )
			name = pregex_qreplace( "([\\\\\"])", sym->name, "\\$1", 0 );

		if( def )
			def = pregex_qreplace( "([\\\\\"])", def, "\\$1", 0 );

		fprintf( f,
			"%s%s[ %d ] = pp_sym_create( %s, %s, %s%s%s, %s%s%s );\n",
			indent,
			symname, sym->id,
			gname, symtypes[ sym->type ],

			name && !( sym->flags & PPFLAG_NAMELESS ) ? "\"" : "",
			name && !( sym->flags & PPFLAG_NAMELESS ) ? name : "(char*)NULL",
			name && !( sym->flags & PPFLAG_NAMELESS ) ? "\"" : "",

			def ? "\"" : "",
			def ? def : "(char*)NULL",
			def ? "\"" : "" );

		pfree( name );
		pfree( def );

		/* Has emit id? */
		if( sym->emit )
			fprintf( f, "%s%s[ %d ]->emit = %d;\n",
				indent, symname, sym->id, sym->emit );

		/* Set flags? */
		if( sym->flags & PPFLAG_LEXEM )
			fprintf( f, "%s%s[ %d ]->flags |= PPFLAG_LEXEM;\n",
				indent, symname, sym->id );
		else if( sym->flags & PPFLAG_WHITESPACE )
			fprintf( f, "%s%s[ %d ]->flags |= PPFLAG_WHITESPACE;\n",
				indent, symname, sym->id );

		/* Is this the goal? */
		if( g->goal == sym )
			fprintf( f, "%s%s->goal = %s[ %d ];\n",
				indent, gname, symname, sym->id );

		fprintf( f, "\n" );
	}

	fprintf( f, "\n" );
}

void gen_prods( FILE* f, ppgram* g )
{
	int		i;
	int		j;
	ppprod*	prod;
	ppsym*	sym;
	char*	name;
	char*	def;
	char*	s;
	char	pat		[ 10 + 1 ];

	fprintf( f, "%s/* Productions */\n\n", indent );

	for( i = 0; prod = pp_prod_get( g, i ); i++ )
	{
		fprintf( f, "%spp_prod_create( %s, %s[ %d ] /* %s */,",
							indent, gname, symname, prod->lhs->id,
								prod->lhs->name );

		for( j = 0; sym = pp_prod_getfromrhs( prod, j ); j++ )
		{
			switch( sym->type )
			{
				case PPSYMTYPE_CCL:
					def = p_ccl_to_str( sym->ccl, TRUE );
					s = "\'";
					break;

				case PPSYMTYPE_STRING:
					def = sym->str;
					s = "\"";
					break;

				case PPSYMTYPE_REGEX:
					def = pregex_ptn_to_regex( sym->re->ptn );
					s = "/";
					break;

				default:
					name = sym->name;
					def = (char*)NULL;
					s = "";
					break;
			}

			if( *s && def )
			{
				sprintf( pat, "([\\\\%s])", s );
				def = pregex_qreplace( pat, def, "\\$1", 0 );

				name = pmalloc( ( strlen( def ) + 2 * strlen( s ) + 1 )
									* sizeof( char ) );
				sprintf( name, "%s%s%s", s, def, s );

				pfree( def );
				def = name;
			}

			if( strstr( name, "*/" ) )
			{
				name = pstrreplace( name, "*/", "*\\/" );
				pfree( def );
				def = name;
			}

			fprintf( f, "\n%s\t%s[ %d ], /* %s */",
				indent, symname, sym->id, name );

			pfree( def );
		}

		fprintf( f, "\n%s\t", indent );
		fprintf( f, "(ppsym*)NULL );\n\n" );
	}

	fprintf( f, "\n" );
}

int main( int argc, char** argv )
{
	char*		e;
	char*		s;
	ppgram*		g;
	int			i;
	int			j;
	int			rc;
	int			next;
	char		opt		[ 20 + 1 ];
	char*		param;
	char*		in		= (char*)NULL;
	char*		gram;
	char*		out		= (char*)NULL;
	FILE*		f	= stdout;

	for( i = 0; ( rc = pgetopt( opt, &param, &next, argc, argv,
						"hi:o:",
						"help indent: output:", i ) ) == 0; i++ )
	{
		if( !strcmp( opt, "help" ) || *opt == 'h' )
		{
			help( argv );
			return 0;
		}
		else if( !strcmp( opt, "output" ) || *opt == 'o' )
			out = param;
		else if( !strcmp( opt, "indent" ) || *opt == 'i' )
		{
			pfree( indent );

			if( ( j = atoi( param ) ) > 0 )
			{
				indent = pmalloc( ( j + 1 ) * sizeof( char ) );

				while( j-- )
					indent[ j ] = '\t';
			}
		}
	}

	if( !indent )
		indent = emptystr;

	if( rc == 1 )
		in = param;

	if( !in )
	{
		help( argv );
		return 1;
	}

	/* Read grammar from input */
	if( !pfiletostr( &gram, in ) )
	{
		fprintf( stderr, "Can't read from '%s'\n", in );
		return 1;
	}

	/* Generate & parse grammar */
	if( !( ( g = pp_gram_create() )
				&& pp_gram_from_bnf( g, gram ) ) )
	{
		fprintf( stderr, "Parse error\n" );
		return 1;
	}

	pp_gram_prepare( g );

	pfree( gram );

	/* Open output file */
	if( out )
	{
		if( !( f = fopen( out, "rb" ) ) )
		{
			fprintf( stderr, "Can't open file '%s' for write\n", out );
			return 1;
		}
	}

	/* Generate C code */
	gen_decl( f, g );
	gen_sym( f, g );
	gen_prods( f, g );

	/* Clear mem */
	pp_gram_free( g );

	if( indent != emptystr )
		pfree( indent );

	return 0;
}
