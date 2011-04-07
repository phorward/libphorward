# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: Basis Library
# Copyright (C) 2006-2010 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include ../../include/Make.inc

SRC			=	base64.c \
				ccl.c \
				convert.c \
				bitset.c \
				dbg.c \
				hashtab.c \
				llist.c \
				stack.c \
				string.c \
				system.c \
				utf8.c \
				util.c \
				var.c \
				var.get.c \
				var.set.c \
				var.conv.c \
				xml.c

SRC_HDR		=	bitset.h \
				ccl.h \
				dbg.h \
				hashtab.h \
				llist.h \
				stack.h \
				string.h \
				utf8.h \
				var.h \
				xml.h

OBJ			=	base64$(OBJEXT) \
				ccl$(OBJEXT) \
				convert$(OBJEXT) \
				bitset$(OBJEXT) \
				dbg$(OBJEXT) \
				hashtab$(OBJEXT) \
				llist$(OBJEXT) \
				stack$(OBJEXT) \
				string$(OBJEXT) \
				system$(OBJEXT) \
				utf8$(OBJEXT) \
				util$(OBJEXT) \
				var$(OBJEXT) \
				var.get$(OBJEXT) \
				var.set$(OBJEXT) \
				var.conv$(OBJEXT) \
				xml$(OBJEXT)

PROTO		=	pbasis.proto.h

HEADERS		=	pbasis.h

DOCFILE		= 	$(DOC_DIR)$(PATH_SEP)pbasis.html

# ------------------------------------------------------------------------------

all: $(PBASIS_LIB_H) $(PBASIS_LIB)
	@echo
	@echo --- Compilation succeeded! ---

$(PBASIS_LIB): $(SRC) $(HEADERS) $(SRC_HDR) $(PROTO) Makefile
	$(CC) $(CLIBOPTS) $(DEBUG) $(SRC)
	$(LLIB) $(LIBOPTS)$(PBASIS_LIB) $(OBJ)

$(PBASIS_LIB_H): $(SRC) $(HEADERS) $(SRC_HDR) $(PROTO) Makefile
	$(LOCALINCLUDE) $(HEADERS) >$@

$(PROTO): $(SRC) $(HEADERS)
	cproto_all "$(SRC)" $(INCLUDE_DIR) $@

#-- Variant Generator	
var.get.c: var.h var.gen.awk
	$(AWK) -f var.gen.awk -vwith_get=1 var.h >$@

var.set.c: var.h var.gen.awk
	$(AWK) -f var.gen.awk -vwith_set=1 var.h >$@

var.conv.c: var.h var.gen.awk
	$(AWK) -f var.gen.awk -vwith_conv=1 var.h >$@
#-- until here!

clean_obj:
	-$(RM) $(OBJ)

clean: clean_obj
	-$(RM) $(PBASIS_LIB)
	-$(RM) $(PBASIS_LIB_H)

backup: clean
	-$(RM) ../pbasis.tar
	tar cvf ../pbasis.tar ../pbasis

#make doc
doc: 
	-$(MKDIR) $(DOC_DIR)
	$(MKDOC) -t "pbasis" -u "Phorward Basis Library" \
		-i $(SRC) >$(DOCFILE)
	@echo
	@echo --- Done making documentation! ---

