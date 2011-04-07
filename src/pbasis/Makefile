# -MAKE-------------------------------------------------------------------------
# Phorward Basis Library
# Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward-software<dot>com
# ------------------------------------------------------------------------------

include ../../include/Make.inc

SRC			=	ccl.c \
				bitset.c \
				dbg.c \
				llist.c \
				hashtab.c \
				util.c \
				system.c \
				utf8.c

SRC_HDR		=	ccl.h \
				bitset.h \
				dbg.h \
				llist.h \
				hashtab.h

OBJ			=	ccl$(OBJEXT) \
				bitset$(OBJEXT) \
				dbg$(OBJEXT) \
				llist$(OBJEXT) \
				hashtab$(OBJEXT) \
				util$(OBJEXT) \
				system$(OBJEXT) \
				utf8$(OBJEXT)

PROTO		=	pbasis.proto.h

HEADERS		=	pbasis.h

DOC			=	$(DOC_DIR)$(PATH_SEP)pbasis$(DOCEXT)

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

clean_obj:
	-$(RM) $(OBJ)

clean: clean_obj
	-$(RM) $(PBASIS_LIB)
	-$(RM) $(PBASIS_LIB_H)

backup: clean
	-$(RM) ../pbasis.tar
	tar cvf ../pbasis.tar ../pbasis

#make doc
doc: $(DOC)
	@echo
	@echo --- Done making documentation! ---

docclean: $(DOC)
	$(RM) -f $?

$(DOC): $(SRC)
	-$(RM) $@
	$(MKDOC) $(SRC) >$@

