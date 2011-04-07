/* -MODULE----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	direct.c
Author:	Jan Max Meyer
Usage:	Direct regular expression handling functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pregex.h"

/*
 * Global variables
 */

/*
 * Functions
 */

int pregex_match( uchar* regex, uchar* str, pregex_result** results )
{
	int				matches	= 0;
	int				ret;
	u_int			len;
	uchar*			pstr;
	pregex_nfa		nfa;

	PROC( "pregex_match" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "results", "%p", results );

	memset( &nfa, 0, sizeof( pregex_nfa ) );
	if( ( ret = pregex_compile_to_nfa( regex, &nfa, 0 ) ) != ERR_OK ) 
		RETURN( ret );

	for( pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );
		if( pregex_nfa_match( &nfa, pstr, &len ) == 0 )
		{
			if( !matches )
				*results = (pregex_result*)pmalloc( sizeof( pregex_result ) );
			else
				*results = (pregex_result*)prealloc(
								(pregex_result*)*results,
									( matches + 1 ) * sizeof( pregex_result ) );

			if( !*results )
				RETURN( ERR_MEM );

			(*results)[ matches ].begin = pstr;

			for( ; len > 0; len-- )
				pstr += u8_seqlen( pstr );

			(*results)[ matches ].end = pstr;
			matches++;
		}
		else
			pstr += u8_seqlen( pstr );

	}
	

	RETURN( matches );
}

