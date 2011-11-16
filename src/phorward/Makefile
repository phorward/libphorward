# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: pbasis
# Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include $(PHOME)/include/Make.inc

PROTOFILE	=	proto.h

SOURCES		=	base64.c \
				ccl.c \
				convert.c \
				bitset.c \
				dbg.c \
				hashtab.c \
				llist.c \
				memory.c \
				stack.c \
				string.c \
				system.c \
				utf8.c \
				util.c \
				var.c \
				var.get.c \
				var.set.c \
				var.conv.c \
				xml.c \
				\
				regex/comp.c \
				regex/dfa.c \
				regex/direct.c \
				regex/nfa.c \
				regex/pattern.c \
				regex/ref.c \
				regex/misc.c \
				\
				string/get.c \
				string/set.c \
				string/pstring.c \
				string/util.c

HEADERS		=	phorward.h \
				\
				bitset.h \
				ccl.h \
				dbg.h \
				hashtab.h \
				llist.h \
				stack.h \
				string.h \
				utf8.h \
				var.h \
				xml.h \
				\
				regex/regex.h \
				\
				string/string.h

OBJECTS		=	$(patsubst %.c, %$(OBJEXT), $(SOURCES))

#------------------------------------------------------------------------------

all: $(LIBPHORWARD_LIB)

$(LIBPHORWARD_LIB): $(PROTOFILE) $(LIBPHORWARD_H) $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

$(PROTOFILE): $(SOURCES)
	$(MKPROTO) $(SOURCES) >$@

$(LIBPHORWARD_H): $(HEADERS)
	$(LOCALINCLUDE) phorward.h >$@

clean:
	-$(RM) $(LIBPHORWARD_H)
	-$(RM) $(OBJECTS)
	-$(RM) $(PROTOFILE)

#------------------------------------------------------------------------------

# Variant Data Type
var.get.c: var.h var.gen.awk
	$(AWK) -f var.gen.awk -vwith_get=1 var.h >$@

var.set.c: var.h var.gen.awk
	$(AWK) -f var.gen.awk -vwith_set=1 var.h >$@

var.conv.c: var.h var.gen.awk
	$(AWK) -f var.gen.awk -vwith_conv=1 var.h >$@

