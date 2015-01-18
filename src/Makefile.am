# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: pbasis
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = subdir-objects

PROTOFILE =	proto.h
LIBHEADER = phorward.h
DOCFILE = ../doc/phorward.html
PATHEXT = PATH="../run:$(PATH)"

lib_LTLIBRARIES = libphorward.la

libphorward_la_SOURCES = \
	base/array.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	ccl/ccl.c \
	parse/ast.c \
	parse/gram.c \
	parse/ll.c \
	parse/lr.c \
	parse/scan.c \
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
	value/value.c \
	value/value.conv.c \
	value/value.get.c \
	value/value.set.c \
	xml/xml.c

noinst_HEADERS = \
	base/array.h \
	base/dbg.h \
	base/list.h \
	ccl/ccl.h \
	parse/parse.h \
	phorward.tpl.h \
	regex/regex.h \
	value/value.h \
	xml/xml.h \
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

value/value.get.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_get=1 value/value.h >$@

value/value.set.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_set=1 value/value.h >$@

value/value.conv.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_conv=1 value/value.h >$@

# Phorward Foundation Library Documentation
$(DOCFILE): $(libphorward_la_SOURCES)
	$(PATHEXT) $(AWK) -f ../run/pdoc \
	--tpldir ../doc/tpl --no-sections --full-index \
		$(libphorward_la_SOURCES) >$@
