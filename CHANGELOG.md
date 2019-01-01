# Changelog

This file is used to document any relevant changes done to libphorward.

## v0.24

Released on: Jan 1, 2019

- Improved hash table feature of `plist` to use non-mersenne prime numbers for collision avoidance
- Updated layout and cross-linking of the HTML-documentation
- Removed former `parse` module and integrated it into the [UniCC repository](https://github.com/phorward/unicc)
- Removed former `vm` module and integrated it into the [phosphor repository](https://github.com/phorward/phosphor)
- Install `pdoc`, `pinclude` and `pproto` command-line tools to /bin
- Removed `premcomment` tool

## v0.23

Released on: Sept 27, 2018

- regex: Implemented push-scanning
  - Lexical analyzers are now configured and executing using the lexer context
    data structure *plexctx*
  - New functions `plexctx_lex()`, `plexctx_init()`,
	`plexctx_create()`, `plexctx_reset()` and `plexctx_free()`.
- regex: Several improvements
  - `plex_dump_dot()` allows to du- Install `pdoc`, `pinclude` and `pproto` command-line tools to /bin
- Removed `premcomment` toolmp plex-scanners in dot-formatted
     GraphViz format; This can also be triggered with plex command-line tool,
     `--dot` switch.
  - *pregex_ptn* function API white-listed for documentation generator
  - New function `pregex_ptn_create()` as an object-oriented shortcut
    for `pregex_ptn_parse()`

## v0.22

Released on: April 17, 2018

- v0.22.4: Memory leaks removed
- v0.22.3: Updated documentation, thanks to AGS
- v0.22.2: inline-symbols incorrectly implemented in pbnf
- v0.22.1: Build-system fixes (Windows only)
- v0.22.1: pgetline() function as POSIX compatiblity layer
- Regular expressions
  - Internal revisions and renamings.
  - Cleaning data structures from temporal and ephemeral values.
  - Removing ``pregex_accept`` structure
  - Renamed ``begin`` to ``start`` in the ``prange`` structure.
- Trace facilities
  - Evalutation of the TRACEFUNCTION environment variable to switch trace output
    also on function-level, rather than module level.
  - New LOG macro to allow for printf-style formatted output.
- Bugfixes
  - Improved the plex command-line utility, it now recognizes `-b` and `-e`
    correctly, allows for escape sequences and can read from stdin.
  - Improved the pregex command-line utility to use the input parameter as is,
    if the parameter is not the name of a file.
  - Removed warnings and unused static functions from the entire library.
  - Fixing & refactoring in pccl_parseshorthand() that caused invalid dfa state
    machines generated from regular expressions on some 32-bit machine
    configurations.
  - plex_tokenize() ran under some circumstances into an endless-loop.
- Documentation updated

## v0.21

Released on: December 13, 2017

- Documentation updated and refreshed with some syntax-highlighting
- New array functions parray_next(), parray_prev(), parray_iter(), parray_riter() and parray_for()
- New list functions plist_iter(), plist_riter(), plist_iter_access(), plist_riter_access() and plist_concat()

## v0.20

Released on: November 7, 2017

- v0.20.2: Removed warnings that came up on Mingw32, issue #1
- v0.20.1: Purged build system
- Changed behavior of command-line tools to be more pynetree compliant.
- Fixing an ancient bug in the DFA construction mechanism building arbitrary
  state orders.
- New README.md and updated CHANGELOG.md

## v0.19

Released on: February 3, 2017

- Redesign of the grammar definition language
- Introduced command-line utilities `pregex` and `plex`
- Generated documentation added
- Several bugfixes in the build-system
- Cleared build-system from unrequired files

## v0.18

Released on: September 9, 2016

-  Full refactoring of all modules, evolving in the following new modules:
   - base (plist, parray, pccl) for data structures
   - string for extended string functions
   - util for system-independent utility functions
   - pany for a dynamic, object-oriented data type
   - pregex for an object-oriented regular expression toolchain
   - plex for an object-oriented lexing toolchain
- Started to create a full reference manual and user guide
- New README.md file with examples
- More test programs and tools in DEMO folder

## v0.17

Released on: July 2, 2012

- New string functions pstrltrim(), pstrrtrim(), pstrtrim(), pstrndup(),
  pstrset(), pstrget(). There had also been some renamings on existing
  functions to follow an integrative naming convention (without underscore
  in its name).
- Added pregex_ptn-structure (pregex-pattern) and rewrote the entire
  parsing and modification tools of regular expressions.
- Extended the regular expression facilities to an object-oriented access
  using the pregex-object. This will give much more flexibility and a
  better handling for various purposes.
- New regular expression object functions for matching, splitting and replacing.
- The trace facilities are now handled in a different way. To switch them on,
  use the environment variable TRACEMODULE and configure the buildsystem with
  the --with-trace option.
- New README documentation generated with txt2tags, with installation of a
  manpage also.
- Moved entire directory and installation structure to fit the GNU coding
  standards.
- Removed own build toolchain and moved to GNU autotools as the main development
  toolchain. There is also a support for scons and Visual C++ integrated
  currently.

## v0.16

First mentionable initial public release was on Sep 16, 2011 as v0.16.3.
