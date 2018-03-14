# Changelog

This file is used to document any relevant changes done to libphorward.

## v0.22

Current development version.

- Parsing tools
  - Created better definition language called PBNF (Phorward BNF, pbnf.c)
  - Revised all modules, separating the grammar definition entirely from the
    parsing algorithm, lexer, parser and abstract syntax tree. This allows for
    a much higher modularity. The ``pppar`` object now represents the internal
    LALR parser that can be executed on arbitrary input, with a combined lexical
    analyzer implemented using ``plex``.
  - Revised and simplified LR parser driver, now working on state machine, and
    not the data-structures from lr.c anymore.
  - Support for a BNF, EBNF and a Phorward-style BNF (PBNF) as input grammars
    using the functions pp_gram_from_bnf(), pp_gram_from_ebnf() and
    pp_gram_from_pbnf().
- Regular expressions
  - Renamed ``begin`` to ``start`` in the ``prange`` structure.
  - Flipped default greedyness of expressions into non-greedyness, greedyness
    can be switched by command-line.
- Trace facilities
  - Evalutation of the TRACEFUNCTION environment variable to switch trace output
    also on function-level, rather than module level.
  - New LOG macro to allow for printf-style formatted output.
- Bugfixes
  - Improved the plex command-line utility, it now recognizes `-b` and `-e`
    correctly and can read from stdin.
  - Removed warnings and unused static functions from the entire library.
  - Fixing & refactoring in p_ccl_parseshorthand() that caused invalid dfa state
    machines generated from regular expressions on some 32-bit machine
    configurations.
  - plex_tokenize() ran under some circumstances into an endless-loop.

## v0.21

Released on: December 13, 2017

- Documentation updated and refreshed with some syntax-highlighting
- New array functions parray_next(), parray_prev(), parray_iter(), parray_riter() and parray_for()
- New list functions plist_iter(), plist_riter(), plist_iter_access(), plist_riter_access() and plist_concat()

## v0.20

Released on: November 7, 2017

- v0.20.2: Removed warnings that came up on Mingw32, issue #1
- v0.20.1: Purged build system
- Started use of a lexical analyzer in the build-in LR parser (very unstable!).
- Changed behavior of command-line tools to be more pynetree compliant.
- Fixing an ancient bug in the DFA construction mechanism building arbitrary
  state orders.
- New README.md and updated CHANGELOG.md

## v0.19

Released on: February 3, 2017

- Redesign of the grammar definition language
- Introduced command-line utilies for pparse, pregex, plex and pvm
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
   - pparse for an object-oriented parsing toolchain with build-in
     abstract syntax tree traversal tools (unfinished!)
   - pvm for object-oriented definition of virtual instruction sets and
     machines (unfinished!)
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

The toolkit's mentionable initial public release was on Sep 16, 2011 as v0.16.3.
