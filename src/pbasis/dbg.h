/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: Trace and Debug Library
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

File:	dbg.h
Usage:	Defines & Trace-Macros;
		__WITH_TRACE must be defined before this include!
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
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "ENTRY", "" ); \
		} \
		while( 0 )
#else
	#define MAIN( tracefile )
#endif


/* Macro: PROC */
#ifdef __WITH_TRACE
	#define PROC( name ) \
		char*	_dbg_proc_name	= name; \
		do { \
		_dbg_indent++; \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "ENTRY", "" ); \
		} \
		while( 0 )
#else
	#define PROC( name )
#endif

/* Macro: RETURN */
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
#ifdef __WITH_TRACE
	#define VOIDRET \
		do \
		{ \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "RETURN", "(void)" ); \
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
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "RETURN", ">%d<", val ); \
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
#ifdef __WITH_TRACE
	#define MSG( text ) \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "MSG", "%s", text )
#else
	#define MSG( text )
#endif

/* Macro: VARS */
#ifdef __WITH_TRACE
	#define VARS( name, format, val ) \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "VAR", "%s = >" format "<", name, val )
#else
	#define VARS( name, format, val )
#endif

/* Macro: PARMS */
#ifdef __WITH_TRACE
	#define PARMS( name, format, val ) \
		_dbg_trace( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name, "PARAM", "%s = >" format "<", name, val )
#else
	#define PARMS( name, format, val )
#endif

/* Macro: TIME */
#ifdef __WITH_TRACE
	#define TIMEDUMP \
		_dbg_time( _dbg_tracefile, _dbg_indent, __FILE__, __LINE__, _dbg_proc_name )
#else
	#define TIMEDUMP
#endif

#ifdef __WITH_TRACE
void _dbg_trace( FILE* f, int indent, char* file, int line, char* proc, 
			char* type, char* format, ... );
void _dbg_time( FILE* f, int indent, char* file, int line, char* proc );
#endif

#endif
