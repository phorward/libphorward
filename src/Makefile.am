# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Toolkit
# Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
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
	any/any.c \
	any/any.conv.c \
	any/any.get.c \
	any/any.print.c \
	any/any.set.c \
	base/array.c \
	base/ccl.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	base/system.c \
	parse/ast.c \
	parse/bnf.c \
	parse/gram.c \
	parse/lr.c \
	parse/parse.c \
	parse/pbnf.c \
	parse/prod.c \
	parse/sym.c \
	regex/dfa.c \
	regex/direct.c \
	regex/lex.c \
	regex/misc.c \
	regex/nfa.c \
	regex/ptn.c \
	regex/regex.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	vm/prog.c \
	vm/run.c \
	vm/vm.c

noinst_HEADERS = \
	any/any.h \
	base/array.h \
	base/ccl.h \
	base/dbg.h \
	base/list.h \
	parse/parse.h \
	regex/regex.h \
	vm/vm.h \
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
	$(PATHEXT) filehead phorward.h "Phorward Foundation Toolkit Global Header" \
		>$@
	$(PATHEXT) pinclude phorward.tpl.h >>$@

# Variant Data Type Modules (generated from definitions in var.h comments)

any/any.get.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_get=1 any/any.h >$@

any/any.set.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_set=1 any/any.h >$@

any/any.conv.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_conv=1 any/any.h >$@

any/any.print.c: any/any.h any/any.gen.awk
	$(AWK) -f any/any.gen.awk -vwith_fprint=1 any/any.h >$@

# Phorward Foundation Library Documentation
../doc/ref.t2t: $(libphorward_la_SOURCES) $(noinst_HEADERS)
	echo "% --- DON'T EDIT THIS FILE, IT IS GENERATED FROM SRC/ ---" >$@
	echo "" >>$@
	echo "" >>$@
	echo "" >>$@
	pdoc -d 2 $(libphorward_la_SOURCES) $(noinst_HEADERS) >>$@

ref: ../doc/ref.t2t

