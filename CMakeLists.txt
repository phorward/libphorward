# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# 
# File: 	CMakeLists.txt
# Usage:	cmake definitions to compile Phorward Foundation Libraries
#			platform independently.
# ------------------------------------------------------------------------------

cmake_minimum_required( VERSION 2.8 )

# ------------------------------------------------------------------------------
# Global definitions
# ------------------------------------------------------------------------------
project( phorward C )

add_definitions( "-DUNICODE -DUTF8" )
include_directories( include )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/run )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

file( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

# ------------------------------------------------------------------------------
# libphorward
# ------------------------------------------------------------------------------
set( phorward_src 
		src/base/bitset.c
		src/base/dbg.c
		src/base/hashtab.c
		src/base/llist.c
		src/base/memory.c
		src/base/stack.c
		src/ccl/ccl.c
		src/regex/comp.c
		src/regex/dfa.c
		src/regex/direct.c
		src/regex/misc.c
		src/regex/nfa.c
		src/regex/pattern.c
		src/regex/ref.c
		src/string/convert.c
		src/string/string.c
		src/string/utf8.c
		src/util/base64.c
		src/util/system.c
		src/variant/var.c
		src/variant/var.conv.c
		src/variant/var.get.c
		src/variant/var.set.c
		src/xml/xml.c
	)

add_library( phorward ${phorward_src} )

# ------------------------------------------------------------------------------
# min_lalr1
# ------------------------------------------------------------------------------

add_executable( min_lalr1 min_lalr1/min_lalr1.c )

# ------------------------------------------------------------------------------
# DEMO
# ------------------------------------------------------------------------------
add_executable( basis DEMO/basis.c )
add_executable( regex DEMO/regex.c )

target_link_libraries( basis phorward )
target_link_libraries( regex phorward )

set_target_properties( basis PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/DEMO )
set_target_properties( regex PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/DEMO )

# ------------------------------------------------------------------------------
# Install
# ------------------------------------------------------------------------------

install( TARGETS phorward DESTINATION lib )
install( TARGETS min_lalr1 DESTINATION bin )
install( FILES include/phorward.h DESTINATION include )

install( PROGRAMS 
	run/dmake
	run/dos_to_unix
	run/lincl
	run/mkdoc.awk
	run/mkproto
	run/rem_comments
	run/text_to_c
		DESTINATION bin 
		)
