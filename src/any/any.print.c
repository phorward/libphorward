/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Dump pany object to trace. */
/*MACRO:PANY_DUMP( pany* any )*/
void _dbg_any_dump( char* file, int line, char* function,
						char* name, pany* val )
{
	switch( val->type )
	{
		case PANYTYPE_BOOL:
			_dbg_trace( file, line, "ANY", function, "%s (bool) = >%d<",
							name, pany_get_bool( val ) );
			break;

		case PANYTYPE_CHAR:
			_dbg_trace( file, line, "ANY", function, "%s (char) = >%c<",
							name, pany_get_char( val ) );
			break;

		case PANYTYPE_INT:
			_dbg_trace( file, line, "ANY", function, "%s (int) = >%d<",
							name, pany_get_int( val ) );
			break;

		case PANYTYPE_LONG:
			_dbg_trace( file, line, "ANY", function, "%s (long) = >%ld<",
							name, pany_get_long( val ) );
			break;

		case PANYTYPE_ULONG:
			_dbg_trace( file, line, "ANY", function, "%s (ulong) = >%ld<",
							name, pany_get_ulong( val ) );
			break;

		case PANYTYPE_FLOAT:
			_dbg_trace( file, line, "ANY", function, "%s (float) = >%f<",
							name, pany_get_float( val ) );
			break;

		case PANYTYPE_DOUBLE:
			_dbg_trace( file, line, "ANY", function, "%s (double) = >%lf<",
							name, pany_get_double( val ) );
			break;

		case PANYTYPE_STR:
			_dbg_trace( file, line, "ANY", function, "%s (str) = >%s<",
							name, pany_get_str( val ) );
			break;

		case PANYTYPE_WCS:
			_dbg_trace( file, line, "ANY", function, "%s (wcs) = >%ls<",
							name, pany_get_wcs( val ) );
			break;

		case PANYTYPE_PTR:
			_dbg_trace( file, line, "ANY", function, "%s (ptr) = >%p<",
							name, pany_get_ptr( val ) );
			break;


		default:
			break;
	}
}

/** Print the type and value of //val// to //stream//
without any conversion. This function shall be used for debug only.

//stream// is the stream to write to.
//val// is the pany-object to be printed.
*/
void pany_fprint( FILE* stream, pany* val )
{
	if( !val )
	{
		fprintf( stream, "%p (empty)", val );
		return;
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			fprintf( stream, "%p bool >%d<\n",
					val, pany_get_bool( val ) );
			break;

		case PANYTYPE_CHAR:
			fprintf( stream, "%p char >%c<\n",
					val, pany_get_char( val ) );
			break;

		case PANYTYPE_INT:
			fprintf( stream, "%p int >%d<\n",
					val, pany_get_int( val ) );
			break;

		case PANYTYPE_LONG:
			fprintf( stream, "%p long >%ld<\n",
					val, pany_get_long( val ) );
			break;

		case PANYTYPE_ULONG:
			fprintf( stream, "%p ulong >%ld<\n",
					val, pany_get_ulong( val ) );
			break;

		case PANYTYPE_FLOAT:
			fprintf( stream, "%p float >%f<\n",
					val, pany_get_float( val ) );
			break;

		case PANYTYPE_DOUBLE:
			fprintf( stream, "%p double >%lf<\n",
					val, pany_get_double( val ) );
			break;

		case PANYTYPE_STR:
			fprintf( stream, "%p str >%s<\n",
					val, pany_get_str( val ) );
			break;

		case PANYTYPE_WCS:
			fprintf( stream, "%p wcs >%ls<\n",
					val, pany_get_wcs( val ) );
			break;

		case PANYTYPE_PTR:
			fprintf( stream, "%p ptr >%p<\n",
					val, pany_get_ptr( val ) );
			break;


		default:
			break;
	}
}

