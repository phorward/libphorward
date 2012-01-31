# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: pbasis
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

VAR_GET = variant/var.get.c
VAR_SET = variant/var.set.c
VAR_CONV = variant/var.conv.c

SRC = 	\
		base/bitset.c \
		base/dbg.c \
		base/hashtab.c \
		base/llist.c \
		base/memory.c \
		base/stack.c \
		ccl/ccl.c \
		regex/comp.c \
		regex/dfa.c \
		regex/direct.c \
		regex/misc.c \
		regex/nfa.c \
		regex/pattern.c \
		regex/ref.c \
		string/convert.c \
		string/string.c \
		string/utf8.c \
		util/base64.c \
		util/system.c \
		variant/var.c \
		$(VAR_GET) \
		$(VAR_SET) \
		$(VAR_CONV) \
		xml/xml.c

PROTOFILE =	proto.h
LIBHEADER = ../include/phorward.h
RUNDIR = PATH="../run:$(PATH)"

all: $(VAR_GET) $(VAR_SET) $(VAR_CONV) $(PROTOFILE) $(LIBHEADER) 

clean:
	-rm $(VAR_GET) $(VAR_SET) $(VAR_CONV)
	-rm $(PROTOFILE)
	-rm $(LIBHEADER)

# Prototypes Files

$(PROTOFILE): $(SRC)
	$(RUNDIR) mkproto $(SRC) >$@

# Library Header

$(LIBHEADER): $(noinst_HEADERS) $(PROTOFILE) phorward.tpl.h
	$(RUNDIR) lincl phorward.tpl.h >$@

# Variant Data Type Modules (generated from definitions in var.h comments)

$(VAR_GET): variant/var.h variant/var.gen.awk
	variant/var.gen.awk -vwith_get=1 variant/var.h >$@

$(VAR_SET): variant/var.h variant/var.gen.awk
	variant/var.gen.awk -vwith_set=1 variant/var.h >$@

$(VAR_CONV): variant/var.h variant/var.gen.awk
	variant/var.gen.awk -vwith_conv=1 variant/var.h >$@

