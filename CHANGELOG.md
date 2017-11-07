# Changelog

This file is used to document any relevant changes done to libphorward.

## v0.20

Released on: November 7, 2017

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

Released on: October 6, 2011

### v0.16.11

- pbasis: Bugfix in xml.c, relating the function xml_parse_file() and
  xml_parse_fd(). Now removed xml_parse_fd(), xml_parse_file() uses the
  map_file() function of util.c now. The problem only existed on the Windows
  port.

### v0.16.10

- Preparatories for a new build concept, because there are problems on
  Windows-platform relating the UniCC release soon.
- Improved mkproto to generate function prototypes with only one script.
- Removed cproto_all scripts, they are not necessary anymore.
- Rewrote Makefiles according to use mkproto for now and in future.


### v0.16.9

- pregex: Added possibilities to define greedyness of the regular expression
  execution machine via the NFA/DFA regular expression itself. Greedyness is set
  TRUE by default, but switched FALSE when any char calls are done.

### v0.16.8

- pbasis: system.c extended to new function pgetopt(), a command-line argument
  parser supporting short- and long-options including parameters.
- NEWPROJECT: Extended base project to new pgetopt() function.

### v0.16.6

- pbasis: pbasis.h now defines va_copy-macro on older systems.

### v0.16.5

- pbasis: Memory leaks and overlaps removed from ccl.c
- pregex: Memory leaks (lost pointers) removed from dfa.c

### v0.16.4

- mkdoc: Documentation generator extended to nicer HTML-output and support for
  txt2tags templates. Take a look into run/mkdoc.awk and your include/Make.inc,
  and the /doc-directory in $PHOME.


### v0.16.3

Entire development moved from SCCS to the Mercurial SCM, project maintenance
and repository now entirely hosted on SourceForge.net. Now development is more
transparent and faster than before. Enjoy!

### v0.16.0

- pregex: Implemented feature to finalize a NFA-stated machine into a DFA, the
  match-, split- and replace-functions can now also be run with DFA-compiled
  machine, for testing purposes.


## v0.15

### v0.15.23

- pregex: Bugfix in DFA subset construction, now the regular expression with the
  lowest accepting ID takes place over all other matches.

### v0.15.15

- pregex: Created some source improvements, default-transition matching
- pbasis: ccl_end()-macro

### v0.15.8

- NEWPROJECT: Added a new directory NEWPROJECT, which serves as a project
  template directory for new projects hosted within the Phorward Foundation
  Libraries. Simply copy this directory, change the NEWPROJECT-strings to your
  desired project name and start coding!
- DEMO: Extended demo project to new pbasis-library features
- pbasis/xml: Extended XML-processing functions: xml_count(), xml_count_all(),
  xml_set_float_attr(), xml_get_float_attr(), xml_set_int_attr(),
  xml_get_int_attr()
- pbasis/xml: Removed substitution and memory allocation bugs
- pbasis/xml: Rewritten output of XML-functions
- pbasis/xml: Integrated XML-Library (based on ezXML) into pbasis library
- pbasis/convert: New generalized functions to dynamically convert integers and
  floats into single- and multi-byte-character strings.
- pbasis/string: Added new functions pvasprintf(), pasprintf(), Pvasprintf() and
  Pasprintf()
- pbasis/var: Added a function generator and several utility functions to
  build-up a variant data type with automatic type conversion into the Phorward
  Foundation Libraries; This datatype can hold byte, char, integer, long,
  unsinged integer, float, double as well as string and wide-character string
  values in one structure. Check out the sources at pbasis/var.*. The get,
  set and conv-functions of the pvar-variant datatype are generated by an
  awk-script var.gen.awk with definitions from var.h

## v0.14.6

Initial Public Release.
