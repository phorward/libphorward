/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: Base Library
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	string.h
Author:	Jan Max Meyer
Usage:	Header for string functions and standard library replacements
----------------------------------------------------------------------------- */

#ifndef STRING_H
#define STRING_H

/* pchar wide-character replacements for standard string functions
	do all begin with uppercase 'P'. */
#ifdef UNICODE

#define Pstrcpy						wcscpy
#define Pstrcat						wcscat
#define Pstrncpy					wcsncpy
#define Pstrcmp						wcscmp
#define Pstrncmp					wcsncmp
#define Pstrtol						wcstol
#define Pstrtoul					wcstoul
#define Pstrtod						wcstod
#define Pstrstr						wcsstr
#define Pstrchr						wcschr
#define Pstrrchr					wcsrchr

#define Pisupper					iswupper
#define Pislower					iswlower
#define Ptoupper					towupper
#define Ptolower					towlower
#define Pisdigit					iswdigit
#define Pisblank					iswblank
#define Pisalnum					iswalnum
#define Pisalpha					iswalpha

#else

/* If no unicode is flagged, pchar is threated as uchar. */
#define Pstrlen						pstrlen
#define Pstrcpy						pstrcpy
#define Pstrcat						pstrcat
#define Pstrncpy					pstrncpy
#define Pstrcmp						pstrcmp
#define Pstrncmp					pstrncmp
#define Pstrtol						pstrtol
#define Pstrtoul					pstrtoul
#define Pstrtod						pstrtoud
#define Pstrchr						pstrchr
#define Pstrrchr					pstrrchr
#define Pstrstr						pstrstr

#define Pisupper					pisupper
#define Pislower					pislower
#define Ptoupper					ptoupper
#define Ptolower					ptolower
#define Pisdigit					pisdigit
#define Pisblank					pisblank
#define Pisalnum					pisalnum
#define Pisalpha					pisalpha

/* UNICODE-only helper functions are replaced by uchar-pendants */
#define Pstrlen						pstrlen
#define Psprintf					psprintf
#define Pstrdup						pstrdup
#define Pstrcatstr					pstrcatstr
#define Pstrcatchar 				pstrcatchar
#define Pstrncatstr 				pstrncatchar
#define Pvasprintf					pvasprintf
#define Pasprintf					pasprintf

#endif

/* Standard string function replacements */
#define pstrcmp( s1, s2 )			strcmp( pgetstr( s1 ), pgetstr( s2 ) )
#define pstrncmp( s1, s2, n )		strncmp( pgetstr( s1 ), pgetstr( s2 ), n )
#define pstrcpy( s1, s2 )			strcpy( s1, pgetstr( s2 ) )
#define pstrncpy( s1, s2, n )		strncpy( s1, pgetstr( s2 ), n )
#define pstrstr( s1, s2 )			strstr( pgetstr( s1 ), pgetstr( s2 ) )
#define pstrchr( s1, ch )			strchr( pgetstr( s1 ), ch )
#define pstrrchr( s1, ch )			strrchr( pgetstr( s1 ), ch )
#define pstrcat( s1, s2 )			strcat( s1, pgetstr( s2 ) )
#define pstrtol( s, p, b )			strtol( pgetstr( s ), p, b )
#define pstrtoul( s, p, b )			strtoul( pgetstr( s ), p, b )
#define pstrtod( s, p )				strtod( pgetstr( s ), p )

#define pisupper( c )				isupper( c )
#define pislower( c )				islower( c )
#define ptoupper( c )				toupper( c )
#define ptolower( c )				tolower( c )
#define pisdigit( c )				isdigit( c )
#define pisblank( c )				isblank( c )
#define pisalnum( c )				isalnum( c )
#define pisalpha( c )				isalpha( c )

#endif
