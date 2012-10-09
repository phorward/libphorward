# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: pbasis
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

PROTOFILE =	proto.h
LIBHEADER = phorward.h
DOCFILE = ../doc/phorward.html
PATHEXT = PATH="../run:$(PATH)"

lib_LTLIBRARIES = libphorward.la

libphorward_la_SOURCES = \
	base/bitset.c \
	base/dbg.c \
	base/hashtab.c \
	base/llist.c \
	base/memory.c \
	base/stack.c \
	ccl/ccl.c \
	regex/dfa.c \
	regex/direct.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/pregex.c \
	regex/range.c \
	regex/ref.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/base64.c \
	util/system.c \
	union/union.c \
	union/union.conv.c \
	union/union.get.c \
	union/union.set.c \
	xml/xml.c

noinst_HEADERS = \
	base/bitset.h \
	base/dbg.h \
	base/hashtab.h \
	base/llist.h \
	base/stack.h \
	ccl/ccl.h \
	phorward.tpl.h \
	regex/pregex.h \
	string/string.h \
	string/utf8.h \
	union/union.h \
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
	$(PATHEXT) pinclude phorward.tpl.h >$@

# Variant Data Type Modules (generated from definitions in var.h comments)

union/union.get.c: union/union.h union/union.gen.awk
	$(AWK) -f union/union.gen.awk -vwith_get=1 union/union.h >$@

union/union.set.c: union/union.h union/union.gen.awk
	$(AWK) -f union/union.gen.awk -vwith_set=1 union/union.h >$@

union/union.conv.c: union/union.h union/union.gen.awk
	$(AWK) -f union/union.gen.awk -vwith_conv=1 union/union.h >$@

# Phorward Foundation Library Documentation
$(DOCFILE): $(libphorward_la_SOURCES)
	$(PATHEXT) $(AWK) -f ../run/pdoc \
	--tpldir ../doc/tpl --no-sections --full-index \
		$(libphorward_la_SOURCES) >$@
