/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	vm.h
Usage:	Dynamic single-stack virtual machine
----------------------------------------------------------------------------- */

/* Typedefs */
typedef struct _pvm 	pvm;
typedef struct _pvmprog	pvmprog;
typedef struct _pvmexec	pvmexec;

typedef size_t			pvmaddr;
typedef unsigned char	pvmbyte;

#define PVM_MAXOPS		CHAR_MAX			/* Max. 256 ops per VM */

/* VM operation */
typedef void (*pvmop)( pvmexec* runtime );	/* Operation */

/* VM description */
struct _pvm
{
	pvmop		op		[ PVM_MAXOPS ];		/* Operations */
	char*		mn		[ PVM_MAXOPS ];		/* Mnemonics */

	plex*		lex;						/* Lexer for assembly compiler */
};

/* VM program */
struct _pvmprog
{
	pvm*		vm;							/* VM for the program */
	parray		prog;						/* Program */
	parray		lit;						/* Literals */
};

/* VM execution runtime */
struct _pvmexec
{
	parray*		stack;						/* Value stack */

	pvmaddr		fp;							/* Current frame pointer */
	pvmbyte*	cs;							/* Code segment */
	pvmbyte*	ecs;						/* Code segment limit */
	pvmbyte*	ip;							/* Instruction ptr */
};
