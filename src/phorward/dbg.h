/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: Base Library
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	dbg.h
Usage:	Program Trace Facilities
		These facilities require to switch __WITH_TRACE on before including
		this file.
----------------------------------------------------------------------------- */

#ifndef _DBG_H
#define _DBG_H

/* ----------------------- */
/* --- DEBUG VARIABLES --- */
/* ----------------------- */
#ifndef __DBG_LOCAL_COMPILE
	#ifdef __WITH_TRACE
		extern int _dbg_indent;
		extern FILE* _dbg_tracefile;
	#endif
#endif

/* -------------- */
/* --- MACROS --- */
/* -------------- */

/* Macro: MAIN */
#ifdef __WITH_TRACE
	#define MAIN( tracefile ) \
		char*	_dbg_proc_name	= "main"; \
		do { \
		_dbg_tracefile = fopen( tracefile, "at" ); \
		_dbg_indent++; \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "ENTRY ", "" ); \
		} \
		while( 0 )
#else
	#define MAIN( tracefile )
#endif


/* Macro: PROC */
/* -FUNCTION--------------------------------------------------------------------
	Function:		PROC()

	Author:			Jan Max Meyer
	
	Usage:			Mark begin of function. PROC must be given behind all
					local variable declarations and before the first line of
					code.

	Parameters:		uchar*	name			The name of the procedure

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#ifdef __WITH_TRACE
	#define PROC( name ) \
		char*	_dbg_proc_name	= name; \
		do { \
		_dbg_indent++; \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "ENTRY ", "" ); \
		} \
		while( 0 )
#else
	#define PROC( name )
#endif

/* Macro: RETURN */
/* -FUNCTION--------------------------------------------------------------------
	Function:		RETURN()

	Author:			Jan Max Meyer
	
	Usage:			Return a value from a function, and close function trace
					level. RETURN must be used whenever a function begins with
					PROC(). If you forget it, it will result in confusing
					trace output.

	Parameters:		<func_type>	ret			The return value of the same type
											of the function iself.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#ifdef __WITH_TRACE
	#define RETURN( val ) \
		do \
		{ _dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "RETURN", "", val );\
		_dbg_indent--; \
		return val; \
		} \
		while( 0 )
#else
	#define RETURN( val ) \
		return val
#endif

/* Macro: VOIDRET */
/* -FUNCTION--------------------------------------------------------------------
	Function:		VOIDRET

	Author:			Jan Max Meyer
	
	Usage:			Same as RETURN(), but for void-Functions.

	Parameters:		

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#ifdef __WITH_TRACE
	#define VOIDRET \
		do \
		{ \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "RETURN", "(void)" ); \
		_dbg_indent--; \
		return; \
		} \
		while( 0 )
#else
	#define VOIDRET \
		return
#endif

/* Macro: MAINRET */
#ifdef __WITH_TRACE
	#define MAINRET( val ) \
		do \
		{ \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "RETURN", ">%d<", val ); \
		_dbg_indent--; \
		fclose( _dbg_tracefile ); \
		return val; \
		} \
		while( 0 )
#else
	#define MAINRET( val )\
		return val
#endif

/* Macro: MSG */
/* -FUNCTION--------------------------------------------------------------------
	Function:		MSG()

	Author:			Jan Max Meyer
	
	Usage:			Print trace message into trace file stream.

	Parameters:		uchar*	text			A string to be printed.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#ifdef __WITH_TRACE
	#define MSG( text ) \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "MSG   ", "%s", text )
#else
	#define MSG( text )
#endif

/* Macro: VARS */
/* -FUNCTION--------------------------------------------------------------------
	Function:		VARS()

	Author:			Jan Max Meyer
	
	Usage:			Show variable content with a desired formatting value.

	Parameters:		uchar*	name			String marking the name of the
											variable.
					uchar*	format			printf-Format string for the
											variable value, e.g. "%s"
					<var>	var				The variable itself.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#ifdef __WITH_TRACE
	#define VARS( name, format, val ) \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "VARS  ", "%s = >" format "<", name, val )
#else
	#define VARS( name, format, val )
#endif

/* Macro: PARMS */
/* -FUNCTION--------------------------------------------------------------------
	Function:		PARMS()

	Author:			Jan Max Meyer
	
	Usage:			Show parameter variable content with a desired formatting
					value. It is used same as VARS(), and has its only purpose
					in differing between variables and parameters.

	Parameters:		uchar*	name			String marking the name of the
											variable.
					uchar*	format			printf-Format string for the
											variable value, e.g. "%s"
					<var>	var				The variable itself.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#ifdef __WITH_TRACE
	#define PARMS( name, format, val ) \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name, "PARMS ", "%s = >" format "<", name, val )
#else
	#define PARMS( name, format, val )
#endif

/* Macro: TIME */
#ifdef __WITH_TRACE
	#define TIMEDUMP \
		_dbg_time( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, \
			_dbg_proc_name )
#else
	#define TIMEDUMP
#endif

#endif
