/* -HEADER----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	pregex.h
Author:	Jan Max Meyer
Usage:	Header for regex lib
----------------------------------------------------------------------------- */

#ifndef PREGEX_H
#define PREGEX_H

#include <pbasis.h>
#include <pstring.h>

/* Defines */
#define REGEX_ACCEPT_NONE		0xFFFF

#define REGEX_ALLOC_STEP		16

#define REGEX_UNCOMPILED		0
#define REGEX_IS_NFA			1
#define REGEX_IS_DFA			2
#define REGEX_IS_MINDFA			3

#ifndef PRIVATE
#define PRIVATE static
#endif

/* Typedefs */
typedef u_int*					pregex_ccl;
typedef struct _regex_nfa_st	pregex_nfa_st;
typedef struct _regex_dfa_ent	pregex_dfa_ent;
typedef struct _regex_dfa_st	pregex_dfa_st;
typedef struct _regex_nfa		pregex_nfa;
typedef struct _regex_dfa		pregex_dfa;
typedef struct _regex			pregex;

/* Structs */
struct _regex_nfa_st
{
	pregex_ccl*		ccl;		/* Char-class; if ccl == (pregex_ccl*)NULL,
									then this is an epsilon edge */

	u_int			next;		/* First following NFA-state */
	u_int			next2;		/* Second following NFA-state */
	
	u_int			accept;		/* Accepting state ID */
};

struct _regex_nfa
{	
	pregex_nfa_st*	states;		/* Pointer to array of nfa-states */
	u_int			states_cnt;	/* Number of nfa-states */
	
	pregex_nfa_st*	start;		/* Pointer to starting state */
	pregex_nfa_st*	end;		/* Pointer to ending state */
};

struct _regex_dfa_ent
{
	pregex_ccl		ccl;		/* Matching character range */
	u_int			go_to;		/* Go-To state */
};

struct _regex_dfa_st
{
	pregex_dfa_ent*	trans;		/* Transition table row for this DFA state */
	u_int			group; 		/* Required for DFA minimization */
	int				accept;		/* Accepting state and ID */
	BOOLEAN			done;		/* Done-Flag */
	
	LIST*			nfa_set;	/* List of closed sets of NFA-states */
};

struct _regex_dfa
{
	pregex_nfa*		nfa;		/* Pointer to NFA this DFA is based on */
	
	pregex_dfa_st*	states;		/* Pointer to array of dfa-states */
	int				states_cnt;	/* Number of dfa-states */
};

struct _regex
{
	byte			state;		/* Current regex state */

	union
	{
		pregex_nfa	nfa;
		pregex_dfa	dfa;
	};

	u_int			flags;		/* Several flags */
};

/* Prototypes */
#ifndef MAKE_PROTOTYPES
/* #include "pregex.proto.h" */
#endif

#endif
