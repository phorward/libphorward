# phorward [![Build Status](https://travis-ci.org/phorward/phorward.svg?branch=master)](https://travis-ci.org/phorward/phorward)

**phorward** is a C library for parser development, lexical analysis, regular expressions and more.

## About

**phorward** is a versatile C-library. It is split into several modules, and mostly focuses on the definition and implementation of parsers, recognizers, virtual machines and regular expressions.

- **any** provides a dynamical, extendible data structure and interface to store, convert and handle variables of different value types ("variant" data type),
- **base** provides tools for dynamic data structures and utility functions used throughout the library, including linked lists, hash-tables, stacks and arrays,
- **parse** defines tools to express grammars and provides a built-in LALR(1) parser generator and objects to handle abstract syntax trees, integrating perfectly with the tools from *regex* for lexical analysis,
- **regex** provides tools for lexical analysis and regular expression processing,
- **string** is an extended string processing library,
- **vm** can be used to implement and run stack-based virtual machines and instruction sets aimed to work with the *any* data type.

## Examples

All examples can easily be compiled with

```bash
$ cc -o example example.c -lphorward
```

### Parsing

The following example defines a simple expressional language, runs a parser on it and evaluates a result. It is some very short form of a compiler, running a program on a virtual machine afterwards.

```c
#include <phorward.h>

static int  stack[100];                             /* Stack for calculations */
static int* tos = &stack[0];                        /* Top-of-stack pointer */

void calc( ppasteval type, ppast* node )            /* AST evaluation */
{
    if( type != PPAST_EVAL_BOTTOMUP )
        return;

    if( !strcmp( node->emit, "Int" ) )
        *(tos++) = atoi( node->start );
    else if( !strcmp( node->emit, "add" ) )
        *((tos--) - 2) = *(tos - 2) + *(tos - 1);
    else if( !strcmp( node->emit, "mul" ) )
        *((tos--) - 2) = *(tos - 2) * *(tos - 1);
}

int main()
{
    ppgram* grm;
    pppar*  par;
    ppast*  ast;

    grm = pp_gram_create();                         /* Create grammar */
    pp_gram_from_pbnf( grm,                         /* Describe grammar */
         "Int  := /[0-9]+/ ;"
         "fact : Int | '(' expr ')' ;"
         "term : term '*' fact = mul | fact ;"
         "expr : expr '+' term = add | term ;" );

    par = pp_par_create( grm );                     /* Construct parser on it */
    pp_par_autolex( par );                          /* Auto-construct a lexer */

    if( !pp_par_parse( &ast, par, "1+2*(3+4)+8" ) ) /* Parse an input string, */
        return 1;                                   /* exit on parse error */

    pp_ast_eval( ast, calc );                       /* Evaluate parsed AST */
    printf( "%d\n", stack[0] );                     /* Dump stacked result */

    return 0;
}
```

### Lexing

Here is a short example for a lexical analyzer matching a C token subset.

```c
#include <phorward.h>

int main()
{
	char* 	tok[] = { "keyword", "literal", "identifier", "operator", "other" };
    plex*   l;
    parray* a;
    prange* r;

	/* Set up a lexer */
    l = plex_create( 0 );

	/* Define tokens */
    plex_define( l, "if|else|while|continue|break", 1, 0 );
    plex_define( l, "\\d+|\\d*\\.\\d+|\\d+\\.\\d*|true|false", 2, 0 );
    plex_define( l, "\\w+", 3, 0 );
    plex_define( l, "=|\\+|-|\\*|/|^|>|<|==|>=|<=|!=", 4, 0 );
    plex_define( l, ";|:|\\(|\\)|{|}|\\[\\]", 5, 0 );

	/* Prepare for execution */
    plex_prepare( l );

	/* Tokenize a string */
    plex_tokenize( l,
		"a = 12+39.5*7; while( true ) if( a > 0 ) break; else continue;", &a );

	/* Iterate through the result */
    parray_for( a, r )
        printf( "%-10s %.*s\n", tok[r->id - 1], r->end - r->start, r->start );
}
```

### Regular expressions

Grab URLs from an HTML-file.

```c
#include <phorward.h>

int main( int argc, char** argv )
{
    pregex* re;
    char*   s;
    char*   ptr;

    if( argc < 2 || !pfiletostr( &s, argv[ 1 ] ) )      /* Load file into str */
        return 1;

    ptr = s;
    re = pregex_create(
            "(href|src)=\"((https://|http://|//).*)\"", /* Regular expression */
                PREGEX_COMP_NONGREEDY );                /* Handling options */

    while( pregex_find( re, ptr, &ptr ) )               /* Dump matches */
        printf( "%.*s\n", re->ref[2].end - re->ref[2].start, re->ref[2].start );

    return 0;
}
```


## Features

*phorward* provides the following features:

- Parser development tools
  - *ppgram* for grammar definition
  - *pppar* provides a modular LALR(1) parser generator
  - *ppast* is a representation of a browsable abstract syntax tree (AST)
- Lexer development tools
  - regular expressions and pattern definition interface
  - *plex* provides a lexical analyzer
  - *pregex* for definition and execution of regular expression
  - *pccl* for unicode-enabled character classes
  - tools for regex and lexer deployment
  - string functions for regular expression match, split and replace
- Runtime evaluation tools
  - construction of dynamic intermediate languages and interpreters
  - *pany* is a data object for handling different data-types in one object
  - *pvm* for defining stack-based virtual machine instruction sets
- Dynamic data structures
  - *plist* for linked-lists with build-in hash table support,
  - *parray* for arrays and stacks.
- Extended string management functions
  - concat, extend, tokenize and short-hand allocation of strings and wide-character strings
  - consistent byte- and wide-character (unicode) function support
  - unicode support for UTF-8 in byte-character functions
- Universal system-specific functions for platform-independent C software development
  - Unix-style command-line parser
  - Mapping files to strings
- Debug und trace facilities
- Consequent object-oriented build-up of all function interfaces (e.g. plist, parray, pregex, pparse, ...)
- Growing code-base of more and more powerful functions

## Documentation

Recently updated, full documentation can be found [here](https://www.phorward-software.com/products/phorward/doc/phorward.html), but also locally after installation. The documentation currently focuses on the stable parts of the library only. Parts which are experimental or under-development not covered or only shortly mentioned.

## Building

Building *phorward* is simple as every GNU-style open source program. Extract the downloaded release tarball or clone the source repository into a directory of your choice. Then, do the steps

```bash
$ ./configure
$ make
$ make install
```

And you're ready to go!

### Windows platforms

On Windows, the usage of [Cygwin #http://cygwin.org/] or another Unix shell environment is required. *phorward* also perfectly cross-compiles on Linux using the MinGW and MinGW_x86-64 compilers.

To compile into 32-Bit Windows executables, configure with

```bash
$ ./configure --host=i486-mingw32 --prefix=/usr/i486-mingw32
```

To compile into 64-Bit Windows executables, configure with

```bash
$ ./configure --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32
```

### Local development build

Alternatively there is also a simpler method for setting up a local build system for development and testing purposes locally in the file-system:

```bash
$ make -f Makefile.gnu make_install
$ make
```

This locally compiles the toolkit and parts of it.

## Credits

*phorward* is developed and maintained by Jan Max Meyer at Phorward Software Technologies.

Some other, related projects by the author are:

- [UniCC](https://github.com/phorward/unicc), the universal parser generator, created on top of *phorward*,
- [RapidBATCH](https://github.com/phorward/rapidbatch), a scripting language, created on top of *phorward*,
- [pynetree](https://github.com/phorward/pynetree), a light-weight parsing toolkit written in pure Python,
- [JS/CC](https://jscc.brobston.com), the JavaScript parser generator.

## License

This software is an open source project released under the terms and conditions of the 3-clause BSD license. See the LICENSE file for more information.

Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer.

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license. The full license terms can be obtained from the file LICENSE.

