/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pvm.c
Usage:	A pvm/pany object demonstration suite.
----------------------------------------------------------------------------- */

#include "local.h"

/* Main */

void dump_mnemonics( pvm* vm )
{
	pvmaddr		i;

	for( i = 2; i < PVM_MAXOPS && vm->mn[ i ]; i++ )
		printf( "%s%s", vm->mn[ i ],
			i + 1 == PVM_MAXOPS || !vm->mn[ i + 1 ] ? "\n" : " " );
}

void help( char** argv )
{
	printf( "Usage: %s OPTIONS [program]\n\n"

	"   program                   Program to be compiled and executed.\n\n"

	"   -h  --help                Show this help, and exit.\n"
	"   -v  --verbose             Print processing information.\n"
	"   -V  --version             Show version info and exit.\n"

	"\n", *argv );

}


int main( int argc, char** argv )
{
	char*		prog	= (char*)NULL;
	char		line	[ 128 + 1 ];
	char		buf		[ 1024 + 1 ];
	pvm*		vm;
	pvmprog*	p;
	int			i;
	int			rc;
	int			next;
	char		opt		[ 20 + 1 ];
	char*		param;
	pany*		ret;

	pboolean	verbose	= FALSE;

	PROC( "pvm" );

	for( i = 0; ( rc = pgetopt( opt, &param, &next, argc, argv,
						"hvV",
						"help verbose version", i ) ) == 0; i++ )
	{
		if( !strcmp( opt, "help" ) || !strcmp( opt, "h" ) )
		{
			help( argv );
			RETURN( 0 );
		}
		else if( !strcmp( opt, "verbose" ) || !strcmp( opt, "v" ) )
			verbose = TRUE;
		else if( !strcmp( opt, "version" ) || !strcmp( opt, "V" ) )
		{
			version( argv, "Virtual machine assembler and runtime" );
			RETURN( 0 );
		}
	}

	if( rc == 1 && param )
	{
		prog = param;
		next++;
	}

	vm = pvm_create();

	if( verbose )
		dump_mnemonics( vm );

	if( prog )
	{
		p = pvm_prog_create( vm, prog );

		if( verbose )
			pvm_prog_dump( p );

		pvm_prog_run( &ret, p );

		if( ret )
			pany_fprint( stdout, ret );

		pvm_prog_free( p );
	}
	else
	{
		while( TRUE )
		{
			*buf = '\0';

			while( TRUE )
			{
				printf( ">" );
				fflush( stdout );

				fgets( line, 128, stdin );
				if( *line == '\n' )
					break;

				strcat( buf, line );
				strcat( buf, "\n" );
			}

			if( !*buf )
				break;

			p = pvm_prog_create( vm, buf );

			if( verbose )
				pvm_prog_dump( p );

			printf( "Prepare to run (Y/n)?" );
			fflush( stdout );
			if( toupper( getchar() ) == 'N' )
				continue;

			pvm_prog_run( (pany**)NULL, p );
			pvm_prog_free( p );
		}
	}

	RETURN( ret ? pany_get_char( ret ) : 0 );
}
