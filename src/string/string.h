/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	string.h
Author:	Jan Max Meyer
Usage:	Header for string functions and standard library replacements
----------------------------------------------------------------------------- */

#ifndef PHORWARD_STRING_H
#define PHORWARD_STRING_H

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

/* If no unicode is flagged, pchar is threated as char. */
#define Pstrlen						pstrlen
#define Pstrcpy						strcpy
#define Pstrcat						strcat
#define Pstrncpy					strncpy
#define Pstrcmp						strcmp
#define Pstrncmp					strncmp
#define Pstrtol						strtol
#define Pstrtoul					strtoul
#define Pstrtod						pstrtoud
#define Pstrchr						strchr
#define Pstrrchr					strrchr
#define Pstrstr						strstr

#define Pisupper					isupper
#define Pislower					islower
#define Ptoupper					toupper
#define Ptolower					tolower
#define Pisdigit					isdigit
#define Pisblank					isblank
#define Pisalnum					isalnum
#define Pisalpha					isalpha

/* UNICODE-only helper functions are replaced by char-pendants */
#define Pstrlen						pstrlen
#define Psprintf					psprintf
#define Pstrdup						pstrdup
#define Pstrcatstr					pstrcatstr
#define Pstrcatchar 				pstrcatchar
#define Pstrncatstr 				pstrncatchar
#define Pvasprintf					pvasprintf
#define Pasprintf					pasprintf

#endif

#endif
