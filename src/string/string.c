/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
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
#include <phorward.h>

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
	Function:		pstrcatchar()

	Author:			Jan Max Meyer

	Usage:			Dynamically appends a character to a string.

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
uchar* pstrcatchar( uchar* str, wchar chr )
{
	PROC( "pstrcatchar" );
	PARMS( "str", "%p", str );
	PARMS( "chr", "%d", chr );

	if( !chr )
		return str;

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
	Function:		pstrcatstr()

	Author:			Jan Max Meyer

	Usage:			Dynamically appends a zero-terminated string to a
					dynamic string.

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
uchar* pstrcatstr( uchar* dest, uchar* src, boolean freesrc )
{
	PROC( "pstrcatstr" );
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
	Function:		pstrncatstr()

	Author:			Jan Max Meyer

	Usage:			Dynamicaly appends a number of N characters from one string
					to a string.

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
uchar* pstrncatstr( uchar* str, uchar* append, psize n )
{
	psize	len		= 0;

	PROC( "pstrncatstr" );
	PARMS( "str", "%p", str );
	PARMS( "append", "%p", append );
	PARMS( "n", "%d", n );

	if( append )
	{
		if( !str )
		{
			if( !( str = (uchar*)pmalloc( ( n + 1 ) * sizeof( uchar ) ) ) )
				RETURN( (uchar*)NULL );
		}
		else
		{
			len = pstrlen( str );

			if( !( str = (uchar*)prealloc( (uchar*)str,
					( len + n + 1 ) * sizeof( uchar ) ) ) )
				RETURN( (uchar*)NULL );
		}

		strncpy( str + len, append, n );
		str[ len + n ] = '\0';
	}

	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrreplace()

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
uchar* pstrreplace( uchar* str, uchar* find, uchar* replace )
{
	uchar*			match;
	uchar*			str_ptr			= str;
	uchar*			result			= (uchar*)NULL;
	uchar*			result_end		= (uchar*)NULL;
	unsigned long	len;
	unsigned long	rlen;
	unsigned long	size			= 0L;

	PROC( "pstrreplace" );
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

	return (uchar*)pmemdup( str, ( pstrlen( str ) + 1 ) * sizeof( uchar ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrndup()

	Author:			Jan Max Meyer

	Usage:			A function mixing strdup() with strncpy(). The resulting
					string will be zero-terminated.

	Parameters:		uchar*		str					Parameter string to be
													dupped. If (uchar*)NULL,
													the function returns
													(uchar*)NULL.
					psize		len					The length to be copied.

	Returns:		uchar*							Returns an allocated
													memory pointer holding
													the zero-terminated string
													duplicate. Must be freed
													with pfree().
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pstrndup( uchar* str, psize len )
{
	uchar*	ret;

	if( !str )
		return (uchar*)NULL;

	if( pstrlen( str ) < len )
		len = pstrlen( str );

	ret = (uchar*)pmalloc( ( len + 1 ) * sizeof( uchar ) );
	pstrncpy( ret, str, len );
	ret[ len ] = '\0';

	return ret;
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
	Function:		pvasprintf()

	Author:			Jan Max Meyer

	Usage:			Implementation and replacement for vasprintf.

	Parameters:		uchar**		str					A pointer receiving the
													resultung, allocated string
													pointer.
	 				uchar*		fmt					The format string.
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		int								Returns the number of
													characters written.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pvasprintf( uchar** str, uchar* fmt, va_list ap )
{
	uchar*		istr;
	int			ilen;
	int			len;
	va_list		w_ap;

	PROC( "pvasprintf" );
	PARMS( "str", "%p", str );
	PARMS( "fmt", "%s", fmt );
	PARMS( "ap", "%p", ap );

	if( !( istr = (uchar*)pmalloc( MALLOC_STEP * sizeof( uchar ) ) ) )
		RETURN( ERR_MEM );

	va_copy( w_ap, ap );

	MSG( "Invoking vsnprintf() for the first time" );
	len = vsnprintf( istr, MALLOC_STEP, fmt, w_ap );
	VARS( "len", "%d", len );

	if( len >= 0 && len < MALLOC_STEP )
		*str = istr;
	else if( len == INT_MAX || len < 0 )
	{
		MSG( "ret is negative or too big - can't handle this!" );
		va_end( w_ap );
		RETURN( ERR_MEM );
	}
	else
	{
		if( !( istr = prealloc( istr, ( ilen = len + 1 ) * sizeof( uchar ) ) ) )
		{
			va_end( w_ap );
			RETURN( ERR_MEM );
		}

		va_end( w_ap );
		va_copy( w_ap, ap );

		MSG( "Invoking vsnprintf() for the second time" );
		len = vsnprintf( istr, ilen, fmt, w_ap );
		VARS( "len", "%d", len );

		if( len >= 0 && len < ilen )
			*str = istr;
		else
		{
			pfree( istr );
			RETURN( ERR_MEM );
		}
	}

	va_end( w_ap );
	RETURN( len );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pasprintf()

	Author:			Jan Max Meyer

	Usage:			Implementation and replacement for asprintf.
					pasprintf() takes only the format-string and various
					arguments. It outputs an allocated string to be freed
					later on.

	Parameters:		uchar*		fmt					Format string
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		uchar*							Returns the allocated string
													which cointains the format
													string with inserted values.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pasprintf( uchar* fmt, ... )
{
	uchar*	str;
	va_list	args;

	PROC( "pasprintf" );
	PARMS( "fmt", "%s", fmt );

	if( !( fmt ) )
		RETURN( (uchar*)NULL );

	va_start( args, fmt );
	pvasprintf( &str, fmt, args );
	va_end( args );

	VARS( "str", "%s", str );
	RETURN( str );
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		psetstr()

	Author:			Jan Max Meyer

	Usage:			Manages the assignment of an dynamically allocated string.

	Parameters:		uchar**		str			Pointer to the target string pointer
											to be (re)allocated.
	 				uchar*		val			The string to be set as value.

	Returns:		uchar*					Allocated heap memory pointer on
											success, (uchar*)NULL else. This
											is the same pointer like *str.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* psetstr( uchar** str, uchar* val )
{
	if( *str )
	{
		if( pstrcmp( *str, val ) == 0 )
			return *str;

		pfree( *str );
	}

	*str = pstrdup( val );
	return *str;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pgetstr()

	Author:			Jan Max Meyer

	Usage:			Reads a string. If its value is (char*)NULL, it will return
					a pointer to a static empty string.

	Parameters:		uchar*		str			String pointer to be savely read.

	Returns:		uchar*					Returns str, or a pointer to a
											static, empty string.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* pgetstr( char* str )
{
	if( !str )
		return "";

	return str;
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
	Function:		pstrrender()

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
uchar* pstrrender( uchar* tpl, ... )
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
	Function:		pstrltrim()

	Author:			Jan Max Meyer

	Usage:			Removes whitespace on the left of a string.

	Parameters:		uchar*		s				String to be left-trimmed.

	Returns:		uchar*						Trimmed string, zero terminated.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* pstrltrim( char* s )
{
	char*	c;

	PROC( "pstr_ltrim" );
	PARMS( "s", "%s", pgetstr( s ) );

	if( !( s && *s ) )
	{
		RETURN( pgetstr( s ) );
	}

	for( c = s; *c; c++ )
		if( !( *c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' ) )
			break;

	memmove( s, c, ( pstrlen( c ) + 1 ) * sizeof( char ) );

	RETURN( s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrrtrim()

	Author:			Jan Max Meyer

	Usage:			Removes trailing whitespace on the right of a string.

	Parameters:		uchar*		s				String to be right-trimmed.

	Returns:		uchar*						Trimmed string, zero terminated.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* pstrrtrim( char* s )
{
	char*	c;

	PROC( "pstr_rtrim" );
	PARMS( "s", "%s", pgetstr( s ) );

	if( !( s && *s ) )
	{
		RETURN( pgetstr( s ) );
	}

	for( c = s + pstrlen( s ) - 1; c > s; c-- )
		if( !( *c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' ) )
			break;

	*( c + 1 ) = '\0';

	RETURN( s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrtrim()

	Author:			Jan Max Meyer

	Usage:			Removes beginning and trailing whitespace from a string.

	Parameters:		uchar*		s				String to be trimmed.

	Returns:		uchar*						Trimmed string, zero terminated.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* pstrtrim( char* s )
{
	PROC( "pstr_trim" );
	PARMS( "s", "%s", pgetstr( s ) );

	if( !( s && *s ) )
	{
		RETURN( pgetstr( s ) );
	}

	s = pstr_ltrim( pstr_rtrim( s ) );

	RETURN( s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrsplit()

	Author:			Jan Max Meyer

	Usage:			Splits a string at a delimiting token and returns an
					allocated array of token reference pointers.

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
int pstrsplit( uchar*** tokens, uchar* str, uchar* sep, int limit )
{
	uchar*	next;
	uchar*	tok		= str;
	int		cnt		= 0;

	PROC( "pstrsplit" );
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
	Function:		pstrcasecmp()

	Author:			Jan Max Meyer

	Usage:			A pstrcmp-pendant matching strings with different
					case order.

	Parameters:		uchar*		s1			First string to compare
					uchar*		s2			Second string to compare with s1.

	Returns:		The same result as pstrcmp().

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int	pstrcasecmp( uchar* s1, uchar* s2 )
{
	if( !( s1 && s2 ) )
		return -1;

	for( ; *s1 && *s2 && ptoupper( *s1 ) == ptoupper( *s2 ); s1++, s2++ )
		;

	return (int)( ptoupper( *s1 ) - ptoupper( *s2 ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrncasecmp()

	Author:			Jan Max Meyer

	Usage:			A pstrncmp-pendant matching strings with different
					case order.

	Parameters:		uchar*		s1			First string to compare
					uchar*		s2			Second string to compare with s1.
					int			n			Number of characters to be
											count until matching test stops.

	Returns:		The same result as pstrncmp().

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int	pstrncasecmp( uchar* s1, uchar* s2, pint n )
{
	if( !( s1 && s2 && n < 1 ) )
		return -1;

	for( ; n > 0 && *s1 && *s2 && ptoupper( *s1 ) == ptoupper( *s2 );
			s1++, s2++, n-- )
		;

	return (int)( ( !n ) ? 0 : ( ptoupper( *s1 ) - ptoupper( *s2 ) ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pstrparsechar()

	Author:			Jan Max Meyer

	Usage:			Reads a character from a byte stream. The character may
					exist of one single character or it may be made up of an
					escape sequence or UTF-8 character. The function returns
					the number of bytes read.

	Parameters:		wchar*		retc		Return pointer for the character
											code of the escaped string.
					uchar*		str			Begin pointer where string
					BOOLEAN		escapeseq	TRUE: Regards escape sequences
											FALSE: Ignores escape sequences

	Returns:		int						The number of bytes that had been
											read for the character.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pstrparsechar( wchar* retc, uchar *str, pboolean escapeseq )
{
	wchar	ch;
    uchar 	digs[9]		=	"\0\0\0\0\0\0\0\0";
    int		dno 		= 0;
	uchar*	p			= str;

	PROC( "pstrparsechar" );
	PARMS( "retc", "%p", retc );
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
					ch = *( p++ );
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

	return (pchar*)pmemdup( str, ( Pstrlen( str ) + 1 ) * sizeof( pchar ) );
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
	Function:		Pvasprintf()

	Author:			Jan Max Meyer

	Usage:			Wide-character implementation of pasprintf().

	Parameters:		pchar**		str					A pointer receiving the
													resultung, allocated string
													pointer.
	 				pchar*		fmt					The format string.
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		int								Returns the number of
													characters written.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int Pvasprintf( pchar** str, pchar* fmt, va_list ap )
{
	pchar*		istr;
	int			ilen;
	int			len;
	va_list		w_ap;

	PROC( "pvasprintf" );
	PARMS( "str", "%p", str );
	PARMS( "fmt", "%ls", fmt );
	PARMS( "ap", "%p", ap );

	if( !( istr = (pchar*)pmalloc( ( ilen = MALLOC_STEP + 1 )
				* sizeof( pchar ) ) ) )
		RETURN( ERR_MEM );

	do
	{
		va_copy( w_ap, ap );
		len = vswprintf( istr, (psize)ilen, fmt, w_ap );
		va_end( w_ap );
		VARS( "len", "%d", len );

		if( len < 0 )
		{
			if( !( istr = prealloc( istr, ( ilen = ilen + MALLOC_STEP + 1 )
					* sizeof( pchar ) ) ) )
			{
				RETURN( ERR_MEM );
			}
		}
	}
	while( len < 0 || len >= ilen );

	*str = istr;

	RETURN( len );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pasprintf()

	Author:			Jan Max Meyer

	Usage:			An implementation of pasprintf() for wide-character pchar.
					pasprintf() takes only the format-string and various
					arguments. It outputs an allocated string to be freed
					later on.

	Parameters:		pchar*		fmt					Format string
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		pchar*							Returns the allocated string
													which cointains the format
													string with inserted values.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pasprintf( pchar* fmt, ... )
{
	pchar*	str;
	va_list	args;

	PROC( "pasprintf" );
	PARMS( "fmt", "%ls", fmt );

	if( !( fmt ) )
		RETURN( (pchar*)NULL );

	va_start( args, fmt );
	Pvasprintf( &str, fmt, args );
	va_end( args );

	VARS( "str", "%ls", str );
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrcatchar()

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
pchar* Pstrcatchar( pchar* str, wchar chr )
{
	PROC( "Pstrcatchar" );
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
	Function:		Pstrcatstr()

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
pchar* Pstrcatstr( pchar* dest, pchar* src, boolean freesrc )
{
	PROC( "Pstrcatstr" );
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
	Function:		Pstrncatstr()

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
pchar* Pstrncatstr( pchar* str, pchar* append, psize num )
{
	psize	len		= 0;

	PROC( "Pstrncatstr" );
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
