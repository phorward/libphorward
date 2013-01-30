/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	dbg.c
Author:	Jan Max Meyer
Usage:	Macros and functions for trace output.
----------------------------------------------------------------------------- */

/*
	Trace is activated in any program if the DEBUG-macro is defined.
	A function which uses trace looks like the following:

	int func( int a, uchar* b )
	{
		int i;
		
		PROC( "func" );
		PARMS( "a", "%d", a );
		PARMS( "b", "%s", b );
		
		MSG( "Performing loop..." );
		for( i = 0; i < a; i++ )
		{
			VARS( "i", "%d", i );
			( do an "i" thing ;)
		}
		
		MSG( "Ok, everything is fine! :)" );
		RETURN( i );
	}
*/

/* ---------------- */
/* --- Includes --- */
/* ---------------- */
#include <phorward.h>

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

int 		_dbg_indent;
FILE* 		_dbg_tracefile;

/* ---------------------- */
/* --- Implementation --- */
/* ---------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		PROC()

	Author:			Jan Max Meyer
	
	Usage:			Trace macro for function level indication.
					The PROC-macro introduces a new function level, if compiled
					with trace. The PROC-macro must be put behind the last local
					variable declaration and the first code line, else it won't
					compile.
					A PROC-macro must exists within a function to allow for
					other trace-macro usages.
					If PROC() is used within a function, the macros RETURN() 
					or VOIDRET, according to the function return value, must
					be used. If PROC is used without RETURN, the trace output
					will output a wrong call level depth.

	Parameters:		uchar*	func_name		Name of the function 

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		RETURN()

	Author:			Jan Max Meyer
	
	Usage:			Trace macro to replace a return-statement.
					RETURN() can only be used if PROC() is used at the beginning
					of the function. For void-functions, use VOIDRET.

	Parameters:		any		return_value	The return-value of the function.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		VOIDRET

	Author:			Jan Max Meyer
	
	Usage:			Trace macro to replace a return-statement for
					void-functions.
					VOIDRET() can only be used if PROC() is used at the
					beginning of the function. For typed functions,
					use RETURN().

	Parameters:		

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		PARMS()

	Author:			Jan Max Meyer
	
	Usage:			Trace macro to output function parameter values.
					The PARMS-macro is used to write parameter names and values
					to the program trace. PARMS() should only be used right
					behind PROC().

	Parameters:		uchar*	param_name		Name of the parameter
					uchar*	format			A printf-styled format string.
					any		parameter		The parameter itself.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		VARS()

	Author:			Jan Max Meyer
	
	Usage:			Trace macro to output variables.
					The VARS-macro is used to write variable names and values
					to the program trace.

	Parameters:		uchar*	param_name		Name of the parameter
					uchar*	format			A printf-styled format string.
					any		parameter		The parameter itself.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		MSG()

	Author:			Jan Max Meyer
	
	Usage:			Write program messages to trace.

	Parameters:		uchar*	param_name		Name of the parameter
					uchar*	format			A printf-styled format string.
					any		parameter		The parameter itself.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */

/*NO_DOC*/
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
	char*		modules;
	char*		basename;
	int			maxdepth;
	va_list		arg;

	/* Find out if this module should be traced */
	if( !( modules = getenv( "TRACEMODULE" ) ) )
		return;

	if( !( basename = strrchr( file, PPATHSEP ) ) )
		basename = file;
	else
		basename++;

	if( pstrcmp( modules, "*" ) != 0 
		&& !pstrstr( modules, basename ) )
		return;

	if( ( maxdepth = atoi( pgetstr( getenv( "TRACEDEPTH" ) ) ) ) > 0
			&& indent > maxdepth )
		return;
	
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

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void _dbg_time( FILE* f, int indent, char* file, int line, char* proc )
{
#ifdef _WIN32
	__time64_t		now;
	static BOOLEAN	tz_isset	= FALSE;
#else
	struct timespec	now;
#endif
	
	if( f == (FILE*)NULL )
		f = stderr;

	fprintf( f, "%d (%s[% 4d]) ", getpid(), file, line );

	while( indent > 0 )
	{
		fprintf( f, "." );
		indent--;
	}


#ifdef _WIN32
	if( !tz_isset )
	{
		_tzset();
		tz_isset = TRUE;
	}

	_time64( &now );

	fprintf( f, "%s() TIMEDUMP %s\n", proc, _ctime64( &now ) );
#else
	clock_gettime( CLOCK_REALTIME, &now );

	fprintf( f, "%s() TIMEDUMP %ld s %ld ns\n", proc, 
				now.tv_sec, now.tv_nsec );
#endif

	
	fflush( f );
	
} /* _dbg_time() */

/*COD_ON*/

