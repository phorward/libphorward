/* -MODULE----------------------------------------------------------------------
Trace and Debug Library
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

File:	dbg.c
Usage:	Tracefile functions
----------------------------------------------------------------------------- */

/* ---------------- */
/* --- Includes --- */
/* ---------------- */
#include "pbasis.h"

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

int 		_dbg_indent;
FILE* 		_dbg_tracefile;

/* ---------------------- */
/* --- Implementation --- */
/* ---------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		_dbg_trace()

	Author:			Jan Max Meyer
	
	Usage:			Output debug information to a stream.

	Parameters:		FILE*	f				File handle where the debug
											should be written to. If
											(FILE*)NULL, debug is written to
											stderr.
					int		indent			Number of indents to be printed.
					char*	file			Filename (__FILE__)
					int		line			Line number in file (__LINE__)
					char*	proc			Procedure name
					char*	type			Type of trace
					char*	format			printf()-like format string
					...						Format values

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void _dbg_trace( FILE* f, int indent, char* file, int line, char* proc, 
			char* type, char* format, ... )
{
	va_list			arg;
	
	va_start( arg, format );
	
	if( f == (FILE*)NULL )
		f = stderr;

	fprintf( f, "%d (%s[% 4d]) ", getpid(), file, line );

	while( indent > 0 )
	{
		fprintf( f, "." );
		indent--;
	}

	fprintf( f, "%s() %s", proc, type );
	
	if( *format != '\0' )
	{
		fprintf( f, ": " );
		vfprintf( f, format, arg );
	}
	
	fprintf( f, "\n" );

	fflush( f );
	
	va_end( arg );
} /* _dbg_trace() */

/* -FUNCTION--------------------------------------------------------------------
	Function:		_dbg_time()

	Author:			Jan Max Meyer
	
	Usage:			Output debug information to a stream.

	Parameters:		FILE*	f				File handle where the debug
											should be written to. If
											(FILE*)NULL, debug is written to
											stderr.
					int		indent			Number of indents to be printed.
					char*	file			Filename (__FILE__)
					int		line			Line number in file (__LINE__)
					char*	proc			Procedure name
					char*	type			Type of trace
					char*	format			printf()-like format string
					...						Format values

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void _dbg_time( FILE* f, int indent, char* file, int line, char* proc )
{
	struct timespec	now;
	
	if( f == (FILE*)NULL )
		f = stderr;

	clock_gettime( CLOCK_REALTIME, &now );
	fprintf( f, "%d (%s[% 4d]) ", getpid(), file, line );

	while( indent > 0 )
	{
		fprintf( f, "." );
		indent--;
	}

	fprintf( f, "%s() TIMEDUMP %ld s %ld ns\n", proc, 
				now.tv_sec, now.tv_nsec );
	
	fflush( f );
	
} /* _dbg_time() */

