
DESCRIPTION
===========

The Phorward SDK provides a powerful software development kit for compiler-related development tasks, primarily targetting the C programming language. It mainly focuses on the implementation of parsers to map arbitrary input into logically defined data structures. As the library is hold entirely dynamic, any generation of source code or hand-writing parsers or lexers is unnecessary. Just define a grammar describing the structure of the input data and that's all - the software does the rest of the job!

The following example programm, written in C, defines a simple expressional grammar, runs a parser on it and prints the generated abstract syntax tree:

    #include <phorward.h>
    
    int main()
    {
        pparse* parser;
        ppast*  ast;
        char*   input = "1+2*(3+4)+5";
        char*   end;
    
        parser = pp_create( 0,  "@int /[0-9]+/ ;"
    							"fact : int | '(' expr ')' ;"
                                "term : @mul( term '*' fact ) | fact ;"
                                "expr : @add( expr '+' term ) | term ;" );
    
        if( !pp_parse_to_ast( &ast, parser, input, &end ) )
            return 1; /* parse error */
    
        pp_ast_dump_short( stdout, ast );
        return 0;
    }

This program can quickly be compiled with

    $ cc -o example example.c -lphorward

Additionally, the toolkit comes with several handy command-line tools serving testing and prototyping facilities. The following command call yields in an equivalent parse tree, altought some symbol names where shortened.

    $ pparse -e "1+2*(3+4)+5" -g "@int /[0-9]+/; f: int | '(' e ')'; t: @mul( t '*' f ) | f; e: @add( e '+' t ) | t;"

The SDK also provides useful general-purpose extensions for C programs, including dynamic data structures (e.g. linked lists, hash-tables, stacks and arrays), extended string management functions and some platform-independent, system-specific helper functions.


FEATURES
========

All tools of the Phorward SDK are hold as a part of a library, which can be dynamically invoked or merged together.
Altought the library is written in C, it provides data objects following a strict object-oriented style.

- Parser development tools
    - Self-hosted Backus-Naur-Form (BNF) grammar definition language
    - pparse provides a modular LR(1) and LALR(1) parser generator
    - ppast is a representation of a browsable abstract syntax tree (ast)
    - Coming soon: Tools for ast traversal and parser deployment
- Lexer development tools
    - regular expressions and pattern definition interface
    - plex provides a lexical analyzer
    - pregex for definition and execution of regular expression
    - pccl for unicode-enabled character classes
    - tools for regex and lexer deployment
    - string functions for regular expression matching, splitting and replacement
- Runtime evaluation tools
    - construction of dynamic intermediate languages and interpreters
    - pany for handling different C-data-types in one object
    - pvm for defining stack-based virtual machine instruction sets
- Dynamic data structures
    - plist for linked-lists with build-in hash table support,
    - parray for arrays and stacks.
- Extended string management functions
    - concat, extend, tokenize and  shorthand allocation of strings and wide-character strings
    - consistent byte- and wide-character (unicode) function support
    - unicode support for UTF-8 in byte-character functions
- Universal system-specific functions for platform-independent C software development
    - Unix-style command-line parser
    - Mapping files to strings
- Debug und trace facilities
- Consequent object-oriented build-up of all function interfaces (e.g. plist, parray, pregex, pparse, ...)
- Growing code-base of more and more powerful functions

Please check out http://phorward.phorward-software.com/ continuously to get latest news, documentation, updates and support on the Phorward Toolkit.


BUILDING
========

Building the Phorward Toolkit is simple as every GNU-style open source program. Extract the downloaded release tarball or clone the hg repository into a directory of your choice.

Then, run

    $ ./configure

to configure the build-system and generate the Makefiles for your current platform. After successful configuration, run

    $ make

and

    $ make install

(properly as root), to install the toolkit into your system.


LOCAL DEVELOPMENT BUILD-SYSTEM
==============================

Alternatively there is also a simpler method for setting up a local build system for development and testing purposes locally in the file-system.

Once, type

    $ make -f Makefile.gnu make_install

then, a simple run of

    $ make

can be used to simply build the entire library or parts of it.

Note, that changes to the build system then must be done in the local Makefile, the local Makefile.gnu as well as the Makefile.am for the autotools-based build system.


AUTHOR
======

The Phorward Toolkit is developed and maintained by Jan Max Meyer, Phorward Software Technologies.

This work is the result of several years experiencing in parser development systems, and has been preceded by the open source parser generators UniCC (http://unicc.phorward-software.com/) and JS/CC (http://jscc.brobston.com). It shall be the final step for an ultimate, powerful compiler toolchain, mainly focusing on compiler-frontends. A sister project is the pynetree parsing library (http://pynetree.org) which is written in and for the Python programming language. It shares the same BNF-syntax for expressing grammars.

Help of any kind to extend and improve this software is always appreciated.


LICENSE
=======

This software is an open source project released under the terms and conditions of the 3-clause BSD license. See the LICENSE file for more information.


COPYRIGHT
=========

Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer.

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license. The full license terms can be obtained from the file LICENSE.

THIS SOFTWARE IS PROVIDED BY JAN MAX MEYER (PHORWARD SOFTWARE TECHNOLOGIES) AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN MAX MEYER (PHORWARD SOFTWARE TECHNOLOGIES) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

