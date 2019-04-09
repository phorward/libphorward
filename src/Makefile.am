# -MAKEFILE---------------------------------------------------------------------
# Phorward C/C++ Library
# Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
# https://phorward.info ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See LICENSE for more information.
#
# File:		Makefile.am
# Usage:	Automake input file
# ------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = subdir-objects

PROTOFILE =	proto.h
LIBHEADER = phorward.h
DOCFILE = ../doc/phorward.html
PATHEXT = PATH="../run:$(PATH)"

lib_LTLIBRARIES = libphorward.la

libphorward_la_LDFLAGS = -no-undefined

libphorward_la_SOURCES = \
	base/array.c \
	base/ccl.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	base/system.c \
	regex/dfa.c \
	regex/direct.c \
    regex/lex.c \
	regex/misc.c \
	regex/nfa.c \
	regex/ptn.c \
	regex/regex.c \
	string/convert.c \
	string/string.c \
	string/utf8.c

noinst_HEADERS = \
	base/array.h \
	base/ccl.h \
	base/dbg.h \
	base/list.h \
	regex/regex.h \
	version.h \
	phorward.tpl.h \
	$(PROTOFILE)

include_HEADERS = \
	$(LIBHEADER)

phorwarddir = $(datadir)/phorward
dist_phorward_DATA = \
	$(DOCFILE)

#BUILT_SOURCES = \
#	variant/var.conv.c \
#	variant/var.get.c \
#	variant/var.set.c \
#	$(PROTOFILE) \
#	$(LIBHEADER)

#
# Generated targets
#

# Prototypes Files

$(PROTOFILE): $(libphorward_la_SOURCES)
	$(PATHEXT) pproto $(libphorward_la_SOURCES) >$@

# Version

version.h:
	echo "#define LIBPHORWARD_VERSION \"`../Version`\"" >$@

# Library Header
$(LIBHEADER): $(noinst_HEADERS) $(PROTOFILE)
	$(PATHEXT) filehead phorward.h "Phorward C/C++ Library Global Header" \
		>$@
	$(PATHEXT) pinclude phorward.tpl.h >>$@

# Phorward Foundation Library Documentation
../doc/ref.t2t: $(libphorward_la_SOURCES) $(noinst_HEADERS)
	echo "% --- DON'T EDIT THIS FILE, IT IS GENERATED FROM SRC/ ---" >$@
	echo "" >>$@
	echo "" >>$@
	echo "" >>$@
	pdoc -d 2 $(libphorward_la_SOURCES) $(noinst_HEADERS) >>$@

ref: ../doc/ref.t2t

