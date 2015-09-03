/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	any.c
Author:	Jan Max Meyer
Usage:	pany implements a universal data type object, which can hold any of
		generic C type.

		The functions in any.conv.c, any.get.c and any.set.c are generated
		by an awk-script reading its definitions from any.h, so the supported
		data types can easily be extended without writing any convert/get/set
		functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Initializes a pany-element.

//val// is the pointer to the pany-structure to be initialized. */
pboolean pany_init( pany* val )
{
	PROC( "pany_init" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( val, 0, sizeof( pany ) );
	val->type = PANYTYPE_NULL;

	RETURN( TRUE );
}

/** Creates a new pany-object.

It allows for parsing a value from //str//.

This object must be released after usage using pany_free(). */
pany* pany_create( char* str )
{
	pany*		val;

	val = (pany*)pmalloc( sizeof( pany ) );
	pany_init( val );

	if( str && *str )
		pany_parse( val, str, 0 );

	return val;
}

/** Frees all memory used by a pany-element.

All memory used by the element is freed, and the union's structure is reset
to be of type PANYTYPE_NULL.

//val// is the pointer to pany structure. */
pboolean pany_reset( pany* val )
{
	PROC( "pany_reset" );
	PARMS( "val", "%p", val );

	if( !( val ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	switch( val->type )
	{
		case PANYTYPE_STR:
			pfree( val->val.s );
			break;

		case PANYTYPE_WCS:
			pfree( val->val.ws );
			break;

		default:
			MSG( "Nothing to free" );
			break;
	}

	val->type = PANYTYPE_NULL;

	RETURN( TRUE );
}

/** Frees an allocated pany object and all its used memory.

The function always returns (pany*)NULL. */
pany* pany_free( pany* val )
{
	if( !val )
		return (pany*)NULL;

	pany_reset( val );
	pfree( val );

	return (pany*)NULL;
}

/** Parse any value from a string.

The function will check and ignore for leading and following whitespace, and
matches long integer, double values and strings.

If a string is encapsulated between C-styled string or character tokens (", '),
the content between the delimiters will be taken as a string and ran trough an
escaping function.

Any other content is taken as string. If the parameter //enforce// is set to
an desired PANY_-type, this type will be enforced, and no special recognition
is done.

This function tries to detect*/
pboolean pany_parse( pany* val, char* str, panytype enforce )
{
	static	plex*	lex;	/* Not that nice. */
	char*			sstr;
	char*			estr;
	int				match;
	size_t			len;

	if( !( val && str && *str ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !enforce )
	{
		for( sstr = str; *sstr && isspace( *sstr ); sstr++ )
			;

		if( !lex )
		{
			lex = plex_create( 0 );

			/* (1) long integer */
			plex_define( lex, "\\d+", 1, 0 );

			/* (2) double float */
			plex_define( lex, "\\d+\\.\\d*|\\d*\\.\\d+", 2, 0 );

			/* (3) string, encapsulated by '...' */
			plex_define( lex, "'(\\\\'|[^'])*'", 3, 0 );

			/* (3) string, encapsulated by "..." */
			plex_define( lex, "\"(\\\\\"|[^\"])*\"", 3, 0 );
		}

		if( ( match = plex_lex( lex, sstr, &estr ) ) )
		{
			len = estr - sstr;

			/* Check if preceded by whitespace ONLY! */
			while( *estr && isspace( *estr ) )
				estr++;

			/* Take match when estr ran to string's end */
			if( !*estr )
			{
				switch( match )
				{
					case 1: /* Integer detected */
						pany_set_long( val, atol( sstr ) );
						break;

					case 2: /* Double detected */
						pany_set_double( val, atof( sstr ) );
						break;

					case 3: /* Encapsulated String */
						pany_set_strndup( val, sstr + 1, len - 2 );
						pstrunescape( pany_get_str( val ) );
						break;

					default:
						MISSINGCASE;
						break;
				}
			}
			/* If not, then throw away any match. */
			else
				match = 0;
		}

		if( !match )
			pany_set_strdup( val, str );
	}
	else
	{
		/* Convert to string first... */
		pany_set_str( val, str );

		/* ...then into preferred type. */
		pany_convert( val, enforce );
	}

	return TRUE;
}

/*
	Get & Set
*/

/* Attribute: constant */

pboolean pany_set_constant( pany* val, pboolean constant )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( constant )
		val->flags |= PANYFLAG_CONSTANT;
	else
		val->flags &= ~PANYFLAG_CONSTANT;

	return TRUE;
}

pboolean pany_get_constant( pany* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PANYFLAG_CONSTANT );
}

/* Attribute: autoconvert */

pboolean pany_set_autoconvert( pany* val, pboolean autoconvert )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( autoconvert )
		val->flags |= PANYFLAG_AUTOCONVERT;
	else
		val->flags &= ~PANYFLAG_AUTOCONVERT;

	return TRUE;
}

pboolean pany_get_autoconvert( pany* val )
{
	if( !val )
	{
		WRONGPARAM;
		return FALSE;
	}

	return TRUEBOOLEAN( val->flags & PANYFLAG_AUTOCONVERT );
}
