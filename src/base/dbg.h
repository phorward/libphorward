/* -HEADER----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	dbg.h
Usage:	Program Trace Facilities
		These facilities require to switch DEBUG on before including
		this file.
----------------------------------------------------------------------------- */

#ifndef _DBG_H
#define _DBG_H

/* Macro: PROC */
#ifdef DEBUG
	#define PROC( name ) \
		static char*	_dbg_proc_name	= name; \
		static clock_t	_dbg_proc_clock; \
		_dbg_trace( __FILE__, __LINE__, "ENTRY", \
			_dbg_proc_name, (char*)NULL ); \
		_dbg_proc_clock = clock()
#else
	#define PROC( name )
#endif

/* Macro: RETURN */
#ifdef DEBUG
	#define RETURN( val ) \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"CLOCK", _dbg_proc_name, "%s : %lf", \
					_dbg_proc_name, \
					(double)( clock() - _dbg_proc_clock ) / CLOCKS_PER_SEC ); \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", _dbg_proc_name, (char*)NULL ); \
			return val; \
		} \
		while( 0 )
#else
	#define RETURN( val ) return val
#endif

/* Macro: VOIDRET */
#ifdef DEBUG
	#define VOIDRET \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"CLOCK", _dbg_proc_name, "%s : %lf", \
					_dbg_proc_name, \
					(double)( clock() - _dbg_proc_clock ) / CLOCKS_PER_SEC ); \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", _dbg_proc_name, (char*)NULL ); \
			return; \
		} \
		while( 0 )
#else
	#define VOIDRET return
#endif

/* Macro: MSG */
#ifdef DEBUG
	#define MSG( text ) \
		_dbg_trace( __FILE__, __LINE__, "MSG", _dbg_proc_name, "%s", text )
#else
	#define MSG( text )
#endif

/* Macro: VARS */
#ifdef DEBUG
	#define VARS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "VARS", _dbg_proc_name, \
				"%s = >" format "<", name, val )
#else
	#define VARS( name, format, val )
#endif

/* Macro: PARMS */
#ifdef DEBUG
	#define PARMS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "PARMS", _dbg_proc_name, \
			"%s = >" format "<", name, val )
#else
	#define PARMS( name, format, val )
#endif

/* Macro: LOG */
#ifdef DEBUG
	#define LOG( format, ... ) \
		_dbg_trace( __FILE__, __LINE__, "LOG", _dbg_proc_name, \
			format, __VA_ARGS__ )
#else
	#define LOG( name, ... )
#endif

#endif
