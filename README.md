# phorward [![Build Status](https://travis-ci.org/phorward/phorward.svg?branch=master)](https://travis-ci.org/phorward/phorward) 

**phorward** *(libphorward)* is a generic C/C++ library and toolbox, serving platform-independent utilities for different purposes.

## Generic data structures

- [parray](doc/phorward.html#parray) - Dynamically managed arrays & stacks
- [pccl](doc/phorward.html#pccl) - Character-classes
- [plex](doc/phorward.html#plex) - Lexical analysis
- [plist](doc/phorward.html#plist) - Linked lists, hash-tables, queues & stacks
- [pregex](doc/phorward.html#pregex) - Regular expressions

## Generic helpers

- [DEBUG-facilities](doc/phorward.html#ptrace) - Logging, tracing and run-time analysis
- [pgetopt](doc/phorward.html#fn_pgetopt) - Command-line options interpreter
- [pstr*, pwcs*](doc/phorward.html#pstr) - Enhanced string operations

## Command-line tools

- [pdoc](doc/phorward.html#c_pdoc) - C source code documentation tool
- [pinclude](doc/phorward.html#c_pinclude) - Generate big files from various smaller ones
- [plex](doc/phorward.html#c_plex) - Lexical analyzer generator and interpreter
- [pproto](doc/phorward.html#c_pproto) - C function prototype generator
- [pregex](doc/phorward.html#c_pregex) - Regular expressions match/find/replace
- [ptest](doc/phorward.html#c_ptest) - C program test facilities

## Documentation

A recently updated, [full documentation can be found here](https://www.phorward-software.com/products/phorward/doc/phorward.html),
and is also locally available after installation.

## Building

Building *phorward* is simple as every GNU-style open source program. Extract the downloaded release tarball or clone the source repository into a directory of your choice.

Then, run

```bash
./configure
make
make install
```

And you're ready to go!

### Alternative development build

Alternatively, there is also a simpler method for setting up a local build system for development and testing purposes.

To do so, type

```bash
$ make -f Makefile.gnu make_install
$ make
```

This locally compiles the library or parts of it, and is ideal for development purposes.

## Credits

*libphorward* is developed and maintained by [Jan Max Meyer](https://github.com/phorward/), Phorward Software Technologies.

Contributions by [Marco Wegner](https://github.com/Heavenfighter) and [AGS](https://github.com/FreeBASIC-programmer).

## License

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license.
The full license terms can be obtained from the file LICENSE.

Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer.
