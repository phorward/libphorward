# phorward [![Build Status](https://travis-ci.org/phorward/phorward.svg?branch=develop)](https://travis-ci.org/phorward/phorward) 

**phorward** *(libphorward)* is a C/C++ programming library, providing useful
extensions like dynamic data structures, enhanced string management functions
and regular expressions.

The library provides several modules which can be used together or on their own:

- **any** provides an extensible data structure and interface to store, convert and handle variables of different value types (some kind of "variant" data type) within one data structure.
- **base** provides tools for dynamic data structures and utility functions used throughout the library, including linked lists, hash-tables, stacks and arrays.
- **regex** provides tools for constructing, parsing, processing and modifying regular expressions.
- **string** is an extended string processing library with useful functions for `char*` and `wchar_t*`-string manipulation.

The library also comes with some tools and command-line helpers, including a documentation generator and a functions prototype generator for C modules.

## Documentation

A recently updated, full documentation can be found [here](https://www.phorward-software.com/products/phorward/doc/phorward.html),
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

### Building on Windows

On Windows systems, the usage of [Cygwin](https://cygwin.org/) or another Unix shell environment is required.

The Phorward Toolkit also perfectly cross-compiles on Linux using the MinGW and MinGW_x86-64 compilers.

```bash
# To compile into 32-Bit Windows executables, configure with
$ ./configure --host=i486-mingw32 --prefix=/usr/i486-mingw32

# To compile into 64-Bit Windows executables, configure with 
$ ./configure --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32
```

## Credits

*libphorward* is developed and maintained by [Jan Max Meyer](https://github.com/phorward/), Phorward Software Technologies.

Contributions by [Marco Wegner](https://github.com/Heavenfighter) and [AGS](https://github.com/FreeBASIC-programmer).

## License

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license.
The full license terms can be obtained from the file LICENSE.

Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer.
