# phorward [![Build Status](https://travis-ci.org/phorward/phorward.svg?branch=master)](https://travis-ci.org/phorward/phorward)

**phorward** is a free C programming toolkit for parser development, lexical analysis, regular expressions and more.

## About

Basically, **phorward** is a versatile C-library. It is divided into several modules, and mostly focuses on the definint and implementing parsers, recognizers, virtual machines and regular expressions.

- **any** provides a dynamical, extendible C data structure and interface to store, convert and handle variables of different value types,
- **base** provides tools for dynamic data structures and utility functions used throughout the library,
- **parse** defines tools to express grammars and provides a built-in LALR(1) parser generator and objects to handle abstract syntax trees,
- **regex** provides tools for regular expression parsing, handling and execution, as well as lexical analysis,
- **string** is an extended string processing library,
- **vm** generates stack-based virtual machines and instruction sets aimed to work with the *any* data type

## Examples

### Parsing

The following example program defines a simple expressional language, runs a parser on it and prints the generated abstract syntax tree.

```c
#include <phorward.h>

int main()
{
	ppgram*	grammar;
    pppar*  parser;
    ppast*  ast;
    char*   input = "1+2*(3+4)+5";
    char*   end;

	/* Define a grammar */
	grammar = pp_gram_create();
	pp_gram_from_ebnf( grammar,
		"f: /[0-9]+/@int | '(' e ')' ;"
		"mul@: t '*' f ;"
		"t: mul@ | f;"
		"add@: e '+' t;"
		"e: add@ | t ;" );

	/* Define a parser and lexer for the grammar */
	parser = pp_par_create( grammar );
	pp_par_auto_token( parser );

	if( !pp_par_parse( &ast, parser, input, &end ) )
		return 1; /* parse error */

    pp_ast_dump_short( stdout, ast );
    return 0;
}
```

It can easily be compiled with:

    $ cc -o example example.c -lphorward

Furthermore, the toolkit comes with a command-line tool serving testing and prototyping facilities. The following command call yields in an equivalent parser and its abstract syntax tree, althought some symbol names are shortened.

    $ pparse "f: /[0-9]+/@int | '(' e ')' ; mul@: t '*' f ; t: mul@ | f; add@: e '+' t; e: add@ | t ;"  "1+2*(3+4)*5"

## Features

*phorward* provides the following features:

- Parser development tools
  - Self-hosted Backus-Naur-Form (BNF) grammar definition language
  - *pparse* provides a modular LR(1) and LALR(1) parser generator
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

## Getting started

*phorward* is under heavy development since a few years. It is kept simple, clear and straightforward.
Documentation can be found [here](https://www.phorward-software.com/products/phorward/doc/phorward.html), but also locally after installation.

The documentation is currently in an under-development state and incomplete. It contains a generated functions reference and handles all library parts shortly.

### Building

Building the Phorward Toolkit is simple as every GNU-style open source program. Extract the downloaded release tarball or clone the source repository into a directory of your choice.

Then, run

    $ ./configure

to configure the build-system and generate the Makefiles for your current platform. After successful configuration, run

    $ make

and

    $ make install

(properly as root), to install the toolkit into your system.

### Local building

Alternatively there is also a simpler method for setting up a local build system for development and testing purposes locally in the file-system.

Once, type

    $ make -f Makefile.gnu make_install

then, a simple run of

    $ make

can be used to simply build the entire library or parts of it.

Note, that changes to the build system then must be done in the local Makefile, the local Makefile.gnu as well as the Makefile.am for the autotools-based build system.

## Credits

The Phorward Toolkit is developed and maintained by Jan Max Meyer, Phorward Software Technologies.

Some other projects by the author are:

- [UniCC](https://github.com/phorward/phorward), the universal parser generator.
- [RapidBATCH](https://github.com/phorward/rapidbatch), a scripting language.
- [pynetree](https://github.com/phorward/pynetree), a light-weight parsing toolkit written in pure Python.
- [JS/CC](https://jscc.brobston.com), the JavaScript parser generator.

## License

This software is an open source project released under the terms and conditions of the 3-clause BSD license. See the LICENSE file for more information.

Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer.

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license. The full license terms can be obtained from the file LICENSE.

