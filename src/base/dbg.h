/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
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
		_dbg_trace( __FILE__, __LINE__, "ENTRY", "%s", _dbg_proc_name )
#else
	#define PROC( name )
#endif

/* Macro: RETURN */
#ifdef DEBUG
	#define RETURN( val ) \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", "%s", _dbg_proc_name ); \
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
				"RETURN", "%s", _dbg_proc_name ); \
			return; \
		} \
		while( 0 )
#else
	#define VOIDRET return
#endif

/* Macro: MSG */
#ifdef DEBUG
	#define MSG( text ) \
		_dbg_trace( __FILE__, __LINE__, "MSG", "%s", text )
#else
	#define MSG( text )
#endif

/* Macro: VARS */
#ifdef DEBUG
	#define VARS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "VARS", "%s = >" format "<", name, val )
#else
	#define VARS( name, format, val )
#endif

/* Macro: PARMS */
#ifdef DEBUG
	#define PARMS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "PARMS", \
			"%s = >" format "<", name, val )
#else
	#define PARMS( name, format, val )
#endif

#endif
