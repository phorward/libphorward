# phorward [![Build Status](https://travis-ci.org/phorward/phorward.svg?branch=develop)](https://travis-ci.org/phorward/phorward) 

**phorward** is a C/C++ programming library, providing useful extensions like dynamic data structures, enhanced string management functions and regular expressions. 

## Features

The library provides several modules for their objects and functions.

- **any** provides an extensible data structure and interface to store, convert and handle variables of different value types (some kind of "variant" data type) within one data structure.
- **base** provides tools for dynamic data structures and utility functions used throughout the library, including linked lists, hash-tables, stacks and arrays.
- **regex** provides tools for constructing, parsing, processing and modifying regular expressions.
- **string** is an extended string processing library with useful functions for `char*` and `wchar_t*`-string manipulation.

## Documentation

Recently updated, full documentation can be found [here](https://www.phorward-software.com/products/phorward/doc/phorward.html), but is also locally available after installation.

## Building

Building *phorward* is simple as every GNU-style open source program. Extract the downloaded release tarball or clone the source repository into a directory of your choice.

Then, run

```bash
./configure
make
make install
```

And you're ready to go!

### Windows platforms

On Windows, the usage of [Cygwin](https://cygwin.org/) or another Unix shell environment is required. *phorward* also perfectly cross-compiles on Linux using the MinGW and MinGW_x86-64 compilers.

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

*libphorward* is developed and maintained by Jan Max Meyer, Phorward Software Technologies.

## License

This software is an open source project released under the terms and conditions of the 3-clause BSD license. See the LICENSE file for more information.

Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer.

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license. The full license terms can be obtained from the file LICENSE.

