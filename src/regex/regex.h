/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	regex.h
Author:	Jan Max Meyer
Usage:	Header for the pregex object and functions.
----------------------------------------------------------------------------- */

/* Defines */
#define PREGEX_ALLOC_STEP		16
#define PREGEX_MAXREF			32

/* Regex flags */

/* ---> Part I: Compile-time Flags */
#define PREGEX_COMP_WCHAR		0x01	/* 	Regular expression is provided
											in wchar_t. */
#define PREGEX_COMP_NOERRORS	0x02	/*	Don't report errors, and try to
											compile as much as possible */
#define PREGEX_COMP_NOANCHORS	0x04	/* 	Ignore anchor tokens, handle them
											as normal characters */
#define PREGEX_COMP_INSENSITIVE	0x08	/* 	Regular expression is case
											insensitive */
#define PREGEX_COMP_STATIC		0x10	/*	The regular expression passed
											to the compiler should be converted
											1:1 as it where a string-constant.
											Any regex-specific symbols will
											be ignored. */

/* ---> Part II: Runtime flags */
#define PREGEX_RUN_WCHAR		0x100	/*	Run regular expression with
											wchar_t as input */
#define PREGEX_RUN_NOANCHORS	0x200	/* 	Ignore anchors while processing */
#define PREGEX_RUN_NOREF		0x400	/*	Don't create references */
#define PREGEX_RUN_GREEDY		0x800		/*	Run regular expression greedy */
#define PREGEX_RUN_NONGREEDY	0x1000	/*	Run regular expression nongreedy */
#define PREGEX_RUN_DEBUG		0x2000 	/*	Debug mode; output some debug to
											stderr */

/* Matching flags */
#define PREGEX_FLAG_NONE		0x00	/* No flags defined */
#define PREGEX_FLAG_BOL			0x01	/* Match at begin of line only */
#define PREGEX_FLAG_EOL			0x02	/* Match at end of line only */
#define PREGEX_FLAG_BOW			0x04	/* Match at begin of word only */
#define PREGEX_FLAG_EOW			0x08	/* Match at end of word only */
#define PREGEX_FLAG_GREEDY		0x10	/* Greedy match, overwrite mode. */
#define PREGEX_FLAG_NONGREEDY	0x20	/* Nongreedy match, overwrite mode. */

/* Regular Expression pattern types */
enum _regex_ptntype
{
	PREGEX_PTN_NULL,
	PREGEX_PTN_CHAR,
	PREGEX_PTN_SUB,
	PREGEX_PTN_ALT,
	PREGEX_PTN_KLE,
	PREGEX_PTN_POS,
	PREGEX_PTN_OPT
};

/* Typedefs */
typedef struct	_regex_accept	pregex_accept;

typedef struct	_regex_nfa_st	pregex_nfa_st;
typedef struct	_regex_nfa		pregex_nfa;

typedef struct	_regex_dfa_tr	pregex_dfa_tr;
typedef struct	_regex_dfa_st	pregex_dfa_st;
typedef struct	_regex_dfa		pregex_dfa;

typedef enum 	_regex_ptntype	pregex_ptntype;
typedef struct	_regex_ptn		pregex_ptn;

typedef struct	_regex			pregex;
typedef	struct	_regex_range	pregex_range;

/* Callback function */
typedef	int 					(*pregex_fn)( pregex*, pregex_range* );
#define PREGEX_FN_NULL			( (pregex_fn)NULL )

/*
 * Internal Structures
 */

/* Accepting state definition */
struct _regex_accept
{
	int				accept;		/* Accepting ID */
	int				flags;		/* Flags */
};

/* NFA state */
struct _regex_nfa_st
{
	pccl*			ccl;		/* Char-class; if ccl == (pccl*)NULL,
									then this is an epsilon edge */
	pregex_nfa_st*	next;		/* First following NFA-state */
	pregex_nfa_st*	next2;		/* Second following NFA-state */

	pregex_accept	accept;		/* Match parameters */
	int				refs;		/* References flags */
};

/* NFA state machine */
struct _regex_nfa
{
	plist*			states;		/* NFA states */
	int				modifiers;	/* Regex-modifiers */
};

/* DFA transition */
struct _regex_dfa_tr
{
	pccl*			ccl;		/* Matching character range */
	unsigned int	go_to;		/* Go-To state */
};

/* DFA state */
struct _regex_dfa_st
{
	plist*			trans;		/* Transition table row for this DFA state */
	pregex_dfa_tr*	def_trans;	/* If != (pregex_dfa_tr*)NULL, this points
									to a default dfa transition, which covers
									the most character of the entire character
									range */

	pregex_accept	accept;		/* Match parameters */
	int				refs;		/* References flags */

	pboolean		done;		/* Done-Flag */
	plist*			nfa_set;	/* List of closed sets of NFA-states */
};

/* DFA state machine */
struct _regex_dfa
{
	plist*			states;		/* List of dfa-states */
};

/*
 * Patterns
 */

struct _regex_ptn
{
	pregex_ptntype	type;		/* Pattern state element type */
	pccl*			ccl;		/* Character-class for PREGEX_PTN_CHAR */

	pregex_ptn*		child[ 2 ];	/* Links to child */
	pregex_ptn*		next;		/* Next sequence element */

	pregex_accept*	accept;		/* Optional accepting information */
};

struct _regex_range
{
	char*			begin;		/* Begin pointer */
	char*			end;		/* End pointer */
};

/*
 * Userspace Objects
 */

/* The pregex object structure */
struct _regex
{
	int				flags;		/* Flags */
	pregex_ptn*		ptn;		/* Pattern */

	int				trans_cnt;	/* Counts of DFA states */
	wchar_t**		trans;		/* DFA transitions */

	pregex_range	ref			[ PREGEX_MAXREF ];
};

/* The pregex object structure */
struct _lex
{
	plist*			ptns;		/* Patterns */

	int				trans_cnt;	/* Counts of DFA states */
	wchar_t**		trans;		/* DFA transitions */

	pregex_range	ref			[ PREGEX_MAXREF ];
};

