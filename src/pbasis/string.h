/* -HEADER----------------------------------------------------------------------
Phorward Basis Library :: String utility functions
Copyright (C) 2009, 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	string.h
Author:	Jan Max Meyer
Usage:	Header for string utility functions
----------------------------------------------------------------------------- */

#ifndef STRUTIL_H
#define STRUTIL_H

/* pchar wide-character replacements for standard string functions
	do all begin with uppercase 'P'. */
#ifdef UNICODE

#define Pstrcpy					wcscpy
#define Pstrcat					wcscat
#define Pstrncpy				wcsncpy
#define Pstrcmp					wcscmp
#define Pstrncmp				wcsncmp	
#define Pstrtol					wcstol
#define Pstrtoul				wcstoul
#define Pstrtod					wcstod
#define Pstrstr					wcsstr
#define Pstrchr					wcschr
#define Pstrrchr				wcsrchr

#define Pisupper				iswupper
#define Pislower				iswlower
#define Ptoupper				towupper
#define Ptolower				towlower
#define Pisdigit				iswdigit

#else

/* If no unicode is flagged, pchar is threated as uchar. */
#define Pstrlen					pstrlen
#define Pstrcpy					pstrcpy
#define Pstrcat					pstrcat
#define Pstrncpy				pstrncpy
#define Pstrcmp					pstrcmp
#define Pstrncmp				pstrncmp
#define Pstrtol					pstrtol
#define Pstrtoul				pstrtoul
#define Pstrtod					pstrtoud
#define Pstrchr					pstrchr
#define Pstrrchr				pstrrchr
#define Pstrstr					pstrstr

#define Pisupper				pisupper
#define Pislower				pislower
#define Ptoupper				ptoupper
#define Ptolower				ptolower
#define Pisdigit				pisdigit

/* UNICODE-only helper functions are replaced by uchar-pendants */
#define Pstrlen					pstrlen
#define Psprintf				psprintf
#define Pstrdup					pstrdup
#define Pstr_append_str			pstr_append_str
#define Pstr_append_char 		pstr_append_char
#define Pstr_append_nchar 		pstr_append_nchar

#endif

/* Standard string function replacements */
#define pstrcmp( s1, s2 )		strcmp( pstrzero( s1 ), pstrzero( s2 ) )
#define pstrncmp( s1, s2, n )	strcmp( pstrzero( s1 ), pstrzero( s2 ), n )
#define pstrcpy( s1, s2 )		strcpy( s1, pstrzero( s2 ) )
#define pstrncpy( s1, s2, n )	strncpy( s1, pstrzero( s2 ), n )
#define pstrstr( s1, s2 )		strstr( s1, pstrzero( s2 ) )
#define pstrtok( s1, s2 )		strtok( s1, s2 )
#define pstrchr( s1, ch )		strchr( s1, ch )
#define pstrrchr( s1, ch )		strrchr( s1, ch )
#define pstrcat( s1, s2 )		strcat( s1, s2 )
#define pstrtol( s, p, b )		strtol( s, p, b )
#define pstrtoul( s, p, b )		strtoul( s, p, b )
#define pstrtod( s, p )			strtod( s, p )

#define pisupper( c )			isupper( c )
#define pislower( c )			islower( c )
#define ptoupper( c )			toupper( c )
#define ptolower( c )			tolower( c )
#define pisdigit( c )			isdigit( c )

#endif
