/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	string.c
Author:	Jan Max Meyer
Usage:	Some extended functions for zero-terminated byte- and wide-character
		strings that can be universally used. The library was build from
		several UniCC and RapidBATCH 6 functions, and is now assembled into
		the pbasis-library now.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pbasis.h"

/*
 * Defines
 */
#define MAX_SIZE		64
#define MALLOC_STEP		32

/*
 * Global variables
 */

/*
 * Functions
 */

/*******************************************************************************
 * WARNING: MOST OF THESE FUNCTIONS HANDLE BYTE-STRINGS ONLY!                  *
 *          BYTE-STRINGS ARE OF TYPE UCHAR*, AND ARE NOT PART OF THE           *
 *          PSTRING FUNCTIONS PROVIDED BY THE PSTRING LIBRARY, ALTOUGHT        *
 *          FUNCTIONS IN HERE DO BEGIN WITH THE "PSTR"-PREFIX!                 *
 ******************************************************************************/

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_append_char()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends a character to a dynamic string.
					
	Parameters:		uchar*		str				Pointer to a string to be
												appended.
												If this is (uchar*)NULL, the
												string is newly allocated.
					wchar		chr				The character to be appended to
												str.
	
	Returns:		uchar*						Pointer to (possibly re-)
												allocated and appended string.
												(uchar*)NULL is returned if no
												memory could be (re)allocated.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstr_append_char( uchar* str, wchar chr )
{
	PROC( "pstr_append_char" );
	PARMS( "str", "%p", str );
	PARMS( "chr", "%d", chr );

	if( !str )
	{
		MSG( "Allocating new string" );
		str = (uchar*)pmalloc( ( 1 + 1 ) * sizeof( uchar ) );
		
		if( str )
			*str = '\0';
	}
	else
	{
		MSG( "Reallocating existing string" );
		str = (uchar*)prealloc( (uchar*)str,
				( pstrlen( str ) + 1 + 1) * sizeof( uchar ) );
	}
	
	VARS( "str", "%p", str );
	if( !str )
	{
		MSG( "Pointer is null, critical error" );
		exit( 1 );
	}

	sprintf( str + pstrlen( str ), "%c", (int)chr );
	
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_append_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends a (possibly dynamic) string to a dynamic string.
					
	Parameters:		uchar*		str				Pointer to a string to be
												appended.
												If this is (uchar*)NULL, the
												string is newly allocated.
					uchar*		append			The string to be appended.
					boolean		release_append	If true, append is free'd
												automatically by this function.
	
	Returns:		uchar*						Pointer to (possibly re-)
												allocated and appended string.
												(uchar*)NULL is returned if no
												memory could be (re)allocated,
												or both strings where NULL.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstr_append_str( uchar* dest, uchar* src, boolean freesrc )
{
	PROC( "pstr_append_str" );
	PARMS( "dest", "%p", dest );
	PARMS( "src", "%p", src );
	PARMS( "freesrc", "%d", freesrc );

	if( src )
	{
		if( !dest )
		{
			if( freesrc )
			{
				dest = src;
				freesrc = FALSE;
			}
			else
				dest = pstrdup( src );
		}
		else
		{
			dest = (uchar*)prealloc( (uchar*)dest,
					( pstrlen( dest ) + pstrlen( src ) + 1 )
						* sizeof( uchar ) );
			strcat( dest, src );
		}
	
		if( freesrc )
			pfree( src );
	}

	RETURN( dest );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_append_nchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends a number of N characters from one string
					to a dynamic string.
					
	Parameters:		uchar*		str				Pointer to a string to be
												appended.
												If this is (uchar*)NULL, the
												string is newly allocated.
					uchar*		append			Begin of character sequence
												to be appended.
					psize		num				Number of characters to be
												appended to str.
	
	Returns:		uchar*						Pointer to (possibly re-)
												allocated and appended string.
												(uchar*)NULL is returned if no
												memory could be (re)allocated,
												or both strings where NULL.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstr_append_nchar( uchar* str, uchar* append, psize num )
{
	psize	len		= 0;

	PROC( "pstr_append_nchar" );
	PARMS( "str", "%p", str );
	PARMS( "append", "%p", append );
	PARMS( "num", "%d", num );
	
	if( append )
	{
		if( !str )
		{
			if( !( str = (uchar*)pmalloc( ( num + 1 ) * sizeof( uchar ) ) ) )
				RETURN( (uchar*)NULL );
		}
		else
		{
			len = pstrlen( str );

			if( !( str = (uchar*)prealloc( (uchar*)str,
					( len + num + 1 ) * sizeof( uchar ) ) ) )
				RETURN( (uchar*)NULL );
		}

		strncpy( str + len, append, num );
		str[ len + num ] = '\0';
	}
	
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_replace()
	
	Author:			Jan Max Meyer
	
	Usage:			Replace a substring sequence within a string.
					
	Parameters:		uchar*		str					String to be replaced in
					uchar*		find				The string to be found
					uchar*		replace				The string to be inserted
													for each string 'find'

	Returns:		uchar*							An allocated string which
													is the resulting source.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstr_replace( uchar* str, uchar* find, uchar* replace )
{
	uchar*			match;
	uchar*			str_ptr			= str;
	uchar*			result			= (uchar*)NULL;
	uchar*			result_end		= (uchar*)NULL;
	unsigned long	len;
	unsigned long	rlen;
	unsigned long	size			= 0L;
	
	PROC( "pstr_replace" );
	PARMS( "str", "%s", str );
	PARMS( "find", "%s", find );
	PARMS( "replace", "%s", replace );

	len = pstrlen( find );
	rlen = pstrlen( replace );

	while( 1 )
	{
		VARS( "str_ptr", "%s", str_ptr );
		if( !( match = pstrstr( str_ptr, find ) ) )
		{
			size = 0;
			match = str_ptr + pstrlen( str_ptr );
		}
		else
			size = rlen;
			
		size += (unsigned long)( match - str_ptr );
		
		VARS( "size", "%ld", size );
		VARS( "match", "%s", match );

		if( !result )
			result = result_end = (uchar*)pmalloc(
				( size + 1 ) * sizeof( uchar ) );
		else
		{
			result = (uchar*)prealloc( (uchar*)result,
				( result_end - result + size + 1 ) * sizeof( uchar ) );
			result_end = result + pstrlen( result );
		}

		if( !result )
		{
			MSG( "Ran out of memory!" );
			exit( 1 );
		}
		
		pstrncpy( result_end, str_ptr, match - str_ptr );
		result_end += match - str_ptr;
		*result_end = '\0';
		
		VARS( "result", "%s", result );
		
		if( !*match )
			break;

		strcat( result_end, replace );
		VARS( "result", "%s", result );
		
		result_end += rlen;
		str_ptr = match + len;
	}

	RETURN( result );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrzero()
	
	Author:			Jan Max Meyer
	
	Usage:			Check string for NULL.
					
	Parameters:		uchar*		str					Parameter string to be
													tested.
	
	Returns:		uchar*							Returns an empty string,
													if parameter is
													(uchar*)NULL, else the
													parameter.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstrzero( uchar* str )
{
	if( !str )
		return "";
		
	return str;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrdup()
	
	Author:			Jan Max Meyer
	
	Usage:			Saver strdup replacement.
					
	Parameters:		uchar*		str					Parameter string to be
													dupped. If (uchar*)NULL,
													the function returns
													(uchar*)NULL.
	
	Returns:		uchar*							Same as strdup, (uchar*)
													NULL if NULL-parameter.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstrdup( uchar* str )
{
	if( !str )
		return (uchar*)NULL;
		
	return (uchar*)memdup( str, ( pstrlen( str ) + 1 ) * sizeof( uchar ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrlen()
	
	Author:			Jan Max Meyer
	
	Usage:			Saver strlen replacement.
					
	Parameters:		uchar*		str					Parameter string to be
													evaluated. If (uchar*)NULL,
													the function returns
													(uchar*)NULL.
	
	Returns:		psize							Same as strlen, 0 in case
													of (uchar*)NULL.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
psize pstrlen( uchar* str )
{
	if( !str )
		return 0;
		
	return strlen( (char*)str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		psprintf()

	Author:			Jan Max Meyer

	Usage:			Replacement for sprintf.

	Parameters:		uchar*		res					Result string
					uchar*		fmt					Format string
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		int								Returns the number of
													characters written, -1
													in error case.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int psprintf( uchar* res, uchar* fmt, ... )
{
	int ret;
	va_list	args;

	PROC( "psprintf" );
	PARMS( "res", "%p", res );
	PARMS( "fmt", "%s", fmt );

	if( !( res && fmt ) )
		RETURN( -1 );

	va_start( args, fmt );
	
	ret = vsprintf( res, fmt, args );
	
	va_end( args );
	
	VARS( "ret", "%d", ret );
	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		patol()
	
	Author:			Jan Max Meyer
	
	Usage:			A safer atol() replacement.
					
	Parameters:		uchar*		str					Parameter string to be
													converted to long.
													
	Returns:		long							Same as atol, 0L in case
													of (uchar*)NULL.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
long patol( uchar* str )
{
	if( !str )
		return 0L;
		
	return atol( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		patof()
	
	Author:			Jan Max Meyer
	
	Usage:			A safer atof() replacement.
					
	Parameters:		uchar*		str					Parameter string to be
													converted to double.
													
	Returns:		double							Same as atol, 0.0 in case
													of (uchar*)NULL.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
double patof( uchar* str )
{
	if( !str )
		return (double)0.0;
		
	return atof( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		patoi()
	
	Author:			Jan Max Meyer
	
	Usage:			A safer atoi() replacement.
					
	Parameters:		uchar*		str					Parameter string to be
													converted to int.
													
	Returns:		double							Same as atoi, 0 in case
													of (uchar*)NULL.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int patoi( uchar* str )
{
	if( !str )
		return 0;
		
	return atoi( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_render()
	
	Author:			Jan Max Meyer
	
	Usage:			Inserts values dynamically into the according wildcards
					positions of a template string
					
	Parameters:		uchar*		tpl				The template to be
												rendered with values.
					...							Set of values to be inserted
												into the desired position;
												These must be always consist
												of three values:

												- Wildcard-Name
													(const uchar*)
												- Value to be inserted
													(uchar*)
												- free-flag (boolean int)
												  if the value is freed by
												  this function (for recur-
												  sive code generation!)

											    Put a (uchar*)NULL as end
												of parameter signal.

	Returns:		uchar*						An allocated string which
												is the resulting source.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstr_render( uchar* tpl, ... )
{
	struct
	{
		uchar*	wildcard;
		uchar*	value;
		BOOLEAN	clear;
		uchar*	_match;
	} values[MAX_SIZE];

	va_list	args;
	int		i;
	int		vcount;
	int		match;
	uchar*	tpl_ptr			= tpl;
	uchar*	result			= (uchar*)NULL;
	long	copy_size;
	long	prev_size;
	long	size			= 0L;

	if( !tpl )
		return (uchar*)NULL;

	va_start( args, tpl );

	for( vcount = 0; vcount < MAX_SIZE; vcount++ )
	{
		if( !( values[vcount].wildcard = va_arg( args, uchar* ) ) )
			break;

		values[vcount].value = va_arg( args, uchar* );
		values[vcount].clear = va_arg( args, BOOLEAN );

		if( !values[vcount].value )
		{
			values[vcount].value = pstrdup( "" );
			values[vcount].clear = TRUE;
		}
	}

	do
	{
		for( i = 0; i < vcount; i++ )
			values[i]._match = pstrstr( tpl_ptr, values[i].wildcard );

		match = vcount;
		for( i = 0; i < vcount; i++ )
		{
			if( values[i]._match != (uchar*)NULL )
			{
				if( match == vcount || values[match]._match > values[i]._match )
					match = i;
			}
		}

		prev_size = size;
		if( match < vcount )
		{
			copy_size = (long)( values[match]._match - tpl_ptr );
			size += (long)strlen( values[match].value );
		}
		else
			copy_size = (long)strlen( tpl_ptr );

		size += copy_size;

		if( result )
			result = (uchar*)prealloc( (uchar*)result, 
				( size + 1 ) * sizeof( uchar ) );
		else
			result = (uchar*)pmalloc( ( size + 1 ) * sizeof( uchar ) );

		memcpy( result + prev_size, tpl_ptr, copy_size * sizeof( uchar ) );

		if( match < vcount )
			memcpy( result + prev_size + copy_size, values[match].value,
				strlen( values[match].value ) * sizeof( uchar ) );

		result[size] = '\0';

		if( match < vcount )
			tpl_ptr += copy_size + strlen( values[match].wildcard );
	}
	while( match < vcount );


	for( i = 0; i < vcount; i++ )
		if( values[i].clear )
			free( values[i].value );

	va_end( args );

	return result;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_strip()
	
	Author:			Jan Max Meyer
	
	Usage:			Removes trailing and ending whitespaces (tabs, space) from
					a string.

	Parameters:		uchar*		str				String to be trimmed.
					
	Returns:		uchar*						Trimmed string, zero terminated.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstr_strip( uchar* str )
{
	uchar*		ptr;
	uchar*		start;
	uchar*		end;

	PROC( "pstr_strip" );
	PARMS( "str", "%s", str );
	
	if( !( str && *str ) )
		return str;

	for( start = str; *start; start++ )
		if( *start != ' ' && *start != '\t' )
			break;

	for( end = str + pstrlen( str ) - 1; end > start; end-- )
		if( *end != ' ' && *end != '\t' )
			break;
	*( end + 1 ) = '\0';

	if( start > str )
	{
		for( ptr = str; *start; start++, ptr++ )
			*ptr = *start;

		*ptr = '\0';
	}
	
	VARS( "str", "%s", str );
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_tok()

	Author:			Jan Max Meyer

	Usage:			Tokenizes a string and returns an allocated array of
					token reference pointers.

	Parameters:		uchar***		tokens			An allocated array of to-
													kenized array values.
													Requires a pointer to
													uchar**.
					uchar*			str				Input string to be tokenized
					uchar*			sep				Token separation string
					int				limit			Token limit; If set to 0,
													there is no token limit
													available.
					

	Returns:		int								Returns the number of
													separated tokens, RB_ERR-
													defines in error-case.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstr_tok( uchar*** tokens, uchar* str, uchar* sep, int limit )
{
	uchar*	next;
	uchar*	tok		= str;
	int		cnt		= 0;
	
	PROC( "pstr_tok" );
	PARMS( "tokens", "%p", tokens );
	PARMS( "str", "%s", str );
	PARMS( "sep", "%p", sep );
	PARMS( "limit", "%d", limit );
	
	if( !( tokens && str && sep && *sep ) )
		RETURN( ERR_PARMS );

	if( !( *tokens = (uchar**)pmalloc( MALLOC_STEP * sizeof( uchar* ) ) ) )
		RETURN( ERR_MEM );

	VARS( "cnt", "%d", cnt );
	VARS( "tok", "%s", tok );
	(*tokens)[ cnt++ ] = tok;
		
	while( ( next = pstrstr( tok, sep ) ) 
			&& ( ( limit > 0 ) ? cnt < limit : 1 ) )
	{
		tok = next + pstrlen( sep );
		VARS( "next", "%s", next );
		*next = '\0';

		if( ( cnt % MAX_SIZE ) == 0 )
		{
			MSG( "realloc required!" );
			if( !( *tokens = (uchar**)prealloc( (uchar**)*tokens,
					( cnt + MALLOC_STEP ) * sizeof( uchar* ) ) ) )
				RETURN( ERR_MEM );
		}

		VARS( "cnt", "%d", cnt );
		VARS( "tok", "%s", tok );
		(*tokens)[ cnt++ ] = tok;
	}

	if( limit > 0 )
		*next = '\0';
	
	RETURN( cnt );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrupr()
	
	Author:			Jan Max Meyer
	
	Usage:			Serves a platform-independent strupr-function.

	Parameters:		uchar*	str				Acts both as input and
											output-string.
					
	Returns:		Pointer to the input string.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstrupr( uchar* str )
{
	uchar*	ptr;

	if( !str )
		return (uchar*)NULL;

	for( ptr = str; *ptr; ptr++ )
		if( pislower( *ptr ) )
			*ptr = ptoupper( *ptr );

	return str;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrlwr()
	
	Author:			Jan Max Meyer
	
	Usage:			Serves a platform-independent strlwr-function.

	Parameters:		uchar*	str				Acts both as input and
											output-string.
					
	Returns:		Pointer to the input string.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstrlwr( uchar* str )
{
	uchar*	ptr;

	if( !str )
		return (uchar*)NULL;

	for( ptr = str; *ptr; ptr++ )
		if( pisupper( *ptr ) )
			*ptr = ptolower( *ptr );

	return str;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstr_char()
	
	Author:			Jan Max Meyer
	
	Usage:			Reads a character from a byte stream. The character may
					exist of one single character or it may be made up of an
					escape sequence or UTF-8 character. The function returns
					the number of bytes read.

	Parameters:		wchar*		retc		Return pointer for the character
											code of the escaped string.
					uchar*		str			Begin pointer where string
					pboolean	escapeseq	TRUE: Regards escape sequences
											FALSE: Ignores escape sequences
					
	Returns:		int						The number of bytes that had been
											read for the character.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */int pstr_char( wchar* retc, uchar *str, pboolean escapeseq )
{
	wchar	ch;
    uchar 	digs[9]		=	"\0\0\0\0\0\0\0\0";
    int		dno 		= 0;
	uchar*	p			= str;

	PROC( "pstr_char" );
	PARMS( "ch", "%p", ch );
	PARMS( "str", "%s", str );
	PARMS( "escapeseq", "%s", BOOLEAN_STR( escapeseq ) );

	if( escapeseq && *p == '\\' )
	{
		p++;

		switch( *p )
		{
			case 'n':
				ch = '\n';
				p++;
				break;
			case 't':
				ch = '\t';
				p++;
				break;
			case 'r':
				ch = '\r';
				p++;
				break;
			case 'b':
				ch = '\b';
				p++;
				break;
			case 'f':
				ch = '\f';
				p++;
				break;
			case 'v':
				ch = '\v';
				p++;
				break;
			case 'a':
				ch = '\a';
				p++;
				break;

			default:
				if( octal_digit( *p ) )
				{
					do
						digs[dno++] = *( p++ );
					while( octal_digit( *p ) && dno < 3 );
					ch = strtol( digs, (uchar**)NULL, 8 );
				}
				else if( *p == 'x' )
				{
					p++;
					while( hex_digit( *p ) && dno < 2 )
						digs[ dno++ ] = *( p++ );

					if (dno > 0)
						ch = strtol( digs, (uchar**)NULL, 16 );
				}
#ifdef UTF8
				else if( *p == 'u' )
				{
					p++;
					while( hex_digit( *p ) && dno < 4 )
						digs[dno++] = *( p++ );

					if( dno > 0 )
						ch = strtol( digs, (uchar**)NULL, 16 );
				}
				else if( *p == 'U' )
				{
					p++;
					while( hex_digit( *p ) && dno < 8 )
						digs[dno++] = *( p++ );

					if( dno > 0 )
						ch = strtol( digs, (uchar**)NULL, 16 );
				}
#endif
				else
				{
#ifdef UTF8
					ch = u8_char( p );
					p += u8_seqlen( p );
#else
					ch = *( p );
#endif
				}
				break;
		}
	}
	else
	{
#ifdef UTF8
		ch = u8_char( p );
		p += u8_seqlen( p );
#else
		ch = *( p++ );
#endif
	}

	VARS( "ch", "%d", ch );
	VARS( "len", "%d", (int)( p - str ) );
	*retc = ch;
    RETURN( (int)( p - str ) );
}

#if 0
/* NO_DOC */

/* -FUNCTION--------------------------------------------------------------------
	Function:		p_int_to_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns an allocated string which contains the
					string-representation of an int value, for code
					generation purposes.

	Parameters:		int		val					Value to be converted
					
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_int_to_str( int val )
{
	uchar*	ret;

	ret = (uchar*)pmalloc( MAX_SIZE * sizeof( uchar ) );
	sprintf( ret, "%d", val );

	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		p_chr_to_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns an allocated string which contains the character-
					representation of an int value, for code generation purposes.

	Parameters:		int		val					Value to be converted
					
	Returns:		uchar*						Pointer to allocated string
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_chr_to_str( int val )
{
	uchar*	ret;

	ret = (uchar*)p_malloc( (1+1) * sizeof( uchar ) );
	sprintf( ret, "%c", val );

	return ret;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		p_long_to_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns an allocated string which contains the string-repre-
					sentation of a long value, for code generation purposes.

	Parameters:		long	val					Value to be converted
					
	Returns:		uchar*						Pointer to allocated string
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_long_to_str( long val )
{
	uchar*	ret;

	ret = (uchar*)p_malloc( 128 * sizeof( uchar ) );
	sprintf( ret, "%ld", val );

	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		p_str_to_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns an allocated string which contains the string-repre-
					sentation of a string or empty string if it is (uchar*)NULL.

	Parameters:		uchar*		val				The string that should be
												validated.
					
	Returns:		uchar*						Pointer to allocated string
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_str_to_str( uchar* val )
{
	uchar*	ret;

	if( val )
		ret = p_strdup( val );
	else
		ret = p_strdup( "" );

	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		p_str_to_xml()
	
	Author:			Jan Max Meyer
	
	Usage:			Modifies a string for XML-compliance. Frees the string
					parameter it gets and returns a allocated pointer that
					has to be freed manually later.

	Parameters:		uchar*		str				The string to be made
												XML compliant. Will be freed
												by p_str_to_xml().
					
	Returns:		uchar*						Pointer to allocated string
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_str_to_xml( uchar* str )
{
	uchar*	ret;

	if( !str )
		return p_strdup( "" );

	ret = p_tpl_insert( str, 7,
				"<", "&lt;", FALSE,
				">", "&gt;", FALSE,
				"&", "&amp;", FALSE,
				"\"", "&quot;", FALSE,
				"\n", "&#x0A;", FALSE,
				"\r", "&#x0D;", FALSE,
				"\t", "&#x09;", FALSE
			);

	p_free( str );

	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		p_unescape_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Unescapes a string.

	Parameters:		uchar*	str				Acts both as input and output-string.
					
	Returns:		Pointer to the input string.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_unescape_str( uchar* str )
{
	uchar*	ptr		= str;
	uchar*	start	= str;

	while( *str != '\0' )
	{
		if( *str == '\\' )
		{
			switch( *(str+1) )
			{
				case 'n':
					*ptr++ = '\n';
					break;
				case 'r':					
					*ptr++ = '\r';
					break;
				case 't':
					*ptr++ = '\t';
					break;
				case 'b':
					*ptr++ = '\b';
					break;
				case '0':
					*ptr++ = '\0';
					break;

				default:
					break;
			}
			str += 2;
		}
		else
			*ptr++ = *str++;
	}

	*ptr = '\0';

	return start;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		p_str_no_whitespace()
	
	Author:			Jan Max Meyer
	
	Usage:			Removes all whitespaces from a string (including inline ones!)
					and returns the resulting string.

	Parameters:		uchar*	str				Acts both as input and output-string.
					
	Returns:		Pointer to the input string.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* p_str_no_whitespace( uchar* str )
{
	uchar*	ptr		= str;
	uchar*	start	= str;

	while( *str != '\0' )
		if( *str == ' ' || *str == '\t' )
			str++;
		else
			*ptr++ = *str++;

	*ptr = '\0';

	return start;
}
/* COD_ON */
#endif

/******************************************************************************
 * FUNCTIONS FOR UNICODE PROCESSING (pchar)                                   *
 ******************************************************************************/

#ifdef UNICODE

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrdup()

	Author:			Jan Max Meyer

	Usage:			Character-set independent strdup replacement.

	Parameters:		pchar*		str					String to be copied in 
														memory.

	Returns:		pchar*							Returns a copy of 'str'.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstrdup( pchar* str )
{	
	if( !str )
		return (pchar*)NULL;
		
	return (pchar*)memdup( str, ( Pstrlen( str ) + 1 ) * sizeof( pchar ) );
		return (pchar*)NULL;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Psprintf()

	Author:			Jan Max Meyer

	Usage:			Psprintf() is a repleacement for a parameter-compatible
					sprintf-behavior function for wide-character.

					Yes, it's unsafe, but the same behavior as sprintf().

	Parameters:		pchar*		res					Result string
					pchar*		fmt					Format string
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		int								Returns the number of
													characters written, -1
													in error case.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int Psprintf( pchar* res, pchar* fmt, ... )
{
	int ret;
	va_list	args;

	PROC( "psprintf" );
	PARMS( "res", "%p", res );
	PARMS( "fmt", "%ls", fmt );

	if( !( res && fmt ) )
		RETURN( -1 );

	va_start( args, fmt );

	ret = vswprintf( res, 256 * 1024 * 1024, fmt, args );
	
	va_end( args );
	
	VARS( "ret", "%d", ret );
	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstr_append_char()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends a character to a dynamic wide-character string.
					
	Parameters:		pchar*		str				Pointer to a pchar-string to be
												appended.
												If this is (pchar*)NULL, the
												string is newly allocated.
					wchar		chr				The character to be appended to
												str.
	
	Returns:		pchar*						Pointer to (possibly re-)allo-
												cated and appended string.
												(pchar*)NULL is returned if no
												memory could be (re)allocated.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstr_append_char( pchar* str, wchar chr )
{
	PROC( "Pstr_append_char" );
	PARMS( "str", "%p", str );
	PARMS( "chr", "%d", chr );

	if( !str )
	{
		MSG( "Allocating new string" );
		str = (pchar*)pmalloc( ( 1 + 1 ) * sizeof( pchar ) );
		
		if( str )
			*str = L'\0';
	}
	else
	{
		MSG( "Reallocating existing string" );
		str = (pchar*)prealloc( (pchar*)str,
				( Pstrlen( str ) + 1 + 1) * sizeof( pchar ) );
	}
	
	VARS( "str", "%p", str );
	if( !str )
	{
		MSG( "Pointer is null, critical error" );
		exit( 1 );
	}

	Psprintf( str + Pstrlen( str ), L"%lc", chr );
	
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstr_append_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends a (possibly dynamic) wide-character string to a
					dynamic wide-character string.
					
	Parameters:		pchar*		str				Pointer to a pchar-string to be
												appended.
												If this is (pchar*)NULL, the
												string is newly allocated.
					pchar*		append			The string to be appended.
					boolean		release_append	If true, append is free'd
												automatically by this function.
	
	Returns:		pchar*						Pointer to (possibly re-)allo-
												cated and appended string.
												(pchar*)NULL is returned if no
												memory could be (re)allocated,
												or both strings where NULL.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstr_append_str( pchar* dest, pchar* src, boolean freesrc )
{
	PROC( "Pstr_append_str" );
	PARMS( "dest", "%p", dest );
	PARMS( "src", "%p", src );
	PARMS( "freesrc", "%d", freesrc );

	if( src )
	{
		if( !dest )
		{
			if( freesrc )
			{
				dest = src;
				freesrc = FALSE;
			}
			else
				dest = Pstrdup( src );
		}
		else
		{
			dest = (pchar*)prealloc( (pchar*)dest,
					( Pstrlen( dest ) + Pstrlen( src ) + 1 )
						* sizeof( pchar ) );
			Pstrcat( dest, src );
		}
	
		if( freesrc )
			pfree( src );
	}

	RETURN( dest );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstr_append_nchar()
	
	Author:			Jan Max Meyer
	
	Usage:			Appends a number of N characters from one wide-character
					string to a dynamic string.
					
	Parameters:		pchar*		str				Pointer to a pchar-string to be
												appended.
												If this is (pchar*)NULL, the
												string is newly allocated.
					pchar*		append			Begin of character sequence
												to be appended.
					psize		num				Number of characters to be
												appended to str.
	
	Returns:		pchar*						Pointer to (possibly re-)allo-
												cated and appended string.
												(pchar*)NULL is returned if no
												memory could be (re)allocated,
												or both strings where NULL.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstr_append_nchar( pchar* str, pchar* append, psize num )
{
	psize	len		= 0;

	PROC( "Pstr_append_nchar" );
	PARMS( "str", "%p", str );
	PARMS( "append", "%p", append );
	PARMS( "num", "%d", num );
	
	if( append )
	{
		if( !str )
		{
			if( !( str = (pchar*)pmalloc( ( num + 1 ) * sizeof( pchar ) ) ) )
				RETURN( (pchar*)NULL );
		}
		else
		{
			len = Pstrlen( str );

			if( !( str = (pchar*)prealloc( (pchar*)str,
					( len + num + 1 ) * sizeof( pchar ) ) ) )
				RETURN( (pchar*)NULL );
		}

		Pstrncpy( str + len, append, num );
		str[ len + num ] = L'\0';
	}
	
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrlen()
	
	Author:			Jan Max Meyer
	
	Usage:			Saver strlen replacement for wide-character.
					
	Parameters:		pchar*		str					Parameter string to be
													evaluated. If (pchar*)NULL,
													the function returns 0.
	
	Returns:		psize							Same as strlen, 0 in case
													of (pchar*)NULL.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
psize Pstrlen( pchar* str )
{
	if( !str )
		return 0;
		
	return wcslen( str );
}

#endif
