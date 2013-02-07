/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	dbg.c
Author:	Jan Max Meyer
Usage:	Macros and functions for trace output.
----------------------------------------------------------------------------- */

#include <phorward.h>

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

/** Write function entry to trace.

The PROC-macro introduces a new function level, if compiled with trace.
The PROC-macro must be put behind the last local variable declaration and the
first code line, else it won't compile. A PROC-macro must exists within a
function to allow for other trace-macro usages. If PROC() is used within a
function, the macros RETURN() or VOIDRET, according to the function return
value, must be used. If PROC is used without RETURN, the trace output will
output a wrong call level depth.

The parameter //func_name// is a static string for the function name.
*/
/*MACRO:PROC( char* func_name )*/

/** Write function return to trace.

RETURN() can only be used if PROC() is used at the beginning of the function.
For void-functions, use the macro VOIDRET.

//return_value// is return-value of the function.
*/
/*MACRO:RETURN( function_type return_value )*/

/** Write void function return to trace.

VOIDRET can only be used if PROC() is used at the beginning of the function.
For typed functions, use the macro RETURN().
*/
/*MACRO:VOIDRET*/

/** Write parameter content to trace.

The PARMS-macro is used to write parameter names and values to the program
trace. PARMS() should - by definition - only be used right behind PROC().
If the logging of variable values is wanted during a function exection to
trace, the VARS()-macro shall be used.

//param_name// is the name of the parameter
//format// is a printf-styled format placeholder.
//parameter// is the parameter itself.
*/
/*MACRO:PARMS( char* param_name, char* format, param_type parameter )*/

/** Write variable content to trace.

The VARS-macro is used to write variable names and values to the program trace.
For parameters taken to functions, the PARMS()-macro shall be used.

//var_name// is the name of the variable
//format// is a printf-styled format placeholder.
//variable// is the the parameter itself.
*/
/*MACRO:VARS( char* var_name, char* format, var_type variable )*/

/** Write a message to trace.

//param_name// is the Name of the parameter
//format// is the A printf-styled format string.
//parameter// is the The parameter itself.

Returns a void Nothing
*/

/*NO_DOC*/

static int _dbg_level;

/** Print //indent// levels to //f//. */
static void _dbg_indent( void )
{
	int		i;

	for( i = 0; i < _dbg_level; i++ )
		fprintf( stderr, "." );
}

/** Output trace message to the error log.

//file// is the filename (__FILE__).
//line// is the line number in file (__LINE__).
//type// is the type of the trace message ("PROC", "RETURN", "VARS" ...).
//format// is a printf()-like format string.
//...// values according to the format string follow.
*/
void _dbg_trace( char* file, int line, char* type, char* format, ... )
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
			&& _dbg_level > maxdepth )
		return;

	if( strcmp( type, "ENTRY" ) == 0 )
	{
		fprintf( stderr, "%d (%s[% 4d]) ", getpid(), file, line );
		_dbg_indent();
		fprintf( stderr, "{\n" );

		_dbg_level++;
	}

	fprintf( stderr, "%d (%s[% 4d]) ", getpid(), file, line );
	_dbg_indent();
	fprintf( stderr, "%-6s", type );

	if( format && *format )
	{
		va_start( arg, format );
		fprintf( stderr, ": " );
		vfprintf( stderr, format, arg );
		va_end( arg );
	}

	fprintf( stderr, "\n" );

	if( strcmp( type, "RETURN" ) == 0 )
	{
		_dbg_level--;

		fprintf( stderr, "%d (%s[% 4d]) ", getpid(), file, line );
		_dbg_indent();
		fprintf( stderr, "}\n" );
	}

	fflush( stderr );
}

/** Output current time to trace output.

//file// is the filename (__FILE__).
//line// is the line number in file (__LINE__).
*/
void _dbg_time( char* file, int line )
{
#ifdef _WIN32
	__time64_t		now;
	static BOOLEAN	tz_isset	= FALSE;
#else
	struct timespec	now;
#endif

	fprintf( stderr, "%d (%s[% 4d]) ", getpid(), file, line );

	_dbg_indent();

#ifdef _WIN32
	if( !tz_isset )
	{
		_tzset();
		tz_isset = TRUE;
	}

	_time64( &now );

	fprintf( stderr, "TIMEDUMP %s\n", _ctime64( &now ) );
#else
	clock_gettime( CLOCK_REALTIME, &now );

	fprintf( stderr, "TIMEDUMP %ld s %ld ns\n", now.tv_sec, now.tv_nsec );
#endif

	fflush( stderr );
}

/*COD_ON*/

