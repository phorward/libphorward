/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

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

	}
}

