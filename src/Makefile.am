# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Toolkit
# Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
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

libphorward_la_SOURCES = \
	base/array.c \
	base/ccl.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	parse/ast.c \
	parse/bnf.c \
	parse/gram.c \
	parse/ll.c \
	parse/lr.c \
	parse/parse.c \
	parse/prod.c \
	parse/scan.c \
	parse/sym.c \
	regex/dfa.c \
	regex/direct.c \
	regex/lex.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/regex.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/system.c \
	util/xml.c \
	any/any.c \
	any/any.conv.c \
	any/any.get.c \
	any/any.set.c

noinst_HEADERS = \
	base/array.h \
	base/ccl.h \
	base/dbg.h \
	base/list.h \
	parse/parse.h \
	phorward.tpl.h \
	regex/regex.h \
	util/xml.h \
	any/any.h \
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

# Phorward Foundation Library Documentation
$(DOCFILE): $(libphorward_la_SOURCES)
	$(PATHEXT) $(AWK) -f ../run/pdoc \
	--tpldir ../doc/tpl --no-sections --full-index \
		$(libphorward_la_SOURCES) >$@
