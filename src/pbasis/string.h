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
	do all begin with uppercase P. */
#ifdef UNICODE

#define Pstrlen			wcslen
#define Pstrcpy			wcscpy
#define Pstrcat			wcscat
#define Pstrncpy		wcsncpy
#define Pstrcmp			wcscmp
#define Pstrncmp		wcsncmp	
#define Pstrtol			wcstol
#define Pstrtoul		wcstoul
#define Pstrtod			wcstod
#define Pstrstr			wcsstr
#define Pstrchr			wcschr
#define Pstrrchr		wcsrchr

#define Pisupper		iswupper
#define Ptoupper		towupper
#define Ptolower		towlower

#else

/* If no unicode is flagged, pchar is threated as uchar. */
#define Pstrlen			strlen
#define Pstrcpy			strcpy
#define Pstrcat			strcat
#define Pstrncpy		strncpy
#define Pstrcmp			strcmp
#define Pstrncmp		strncmp
#define Pstrtol			strtol
#define Pstrtoul		strtoul
#define Pstrtod			strtoud
#define Pstrchr			strchr
#define Pstrrchr		strrchr
#define Pstrstr			strstr

#define Pisupper		isupper
#define Ptoupper		toupper
#define Ptolower		tolower

/* UNICODE-only helper functions */
#define Psprintf		psprintf
#define Pstrdup			pstrdup

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
#define ptoupper( c )			toupper( c )
#define ptolower( c )			tolower( c )

#endif
