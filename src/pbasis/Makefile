# -MAKE-------------------------------------------------------------------------
# Phorward Basis Library
# Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward-software<dot>com
# ------------------------------------------------------------------------------

include ../../include/Make.inc

SRC			=	bitset.c \
				dbg.c \
				llist.c \
				hashtab.c \
				util.c

SRC_HDR		=	bitset.h \
				dbg.h \
				llist.h \
				hashtab.h

OBJ			=	bitset$(OBJEXT) \
				dbg$(OBJEXT) \
				llist$(OBJEXT) \
				hashtab$(OBJEXT) \
				util$(OBJEXT)

PROTO		=	pbasis.proto.h
PROTO_H		=	$(INCLUDE_DIR)$(PATH_SEP)$(PROTO)

HEADERS		=	pbasis.h

# ------------------------------------------------------------------------------

all: $(PBASIS_LIB_H) $(PROTO_H) $(PBASIS_LIB)
	@echo
	@echo --- Compilation succeeded! ---

$(PBASIS_LIB): $(SRC) $(HEADERS) Makefile
	$(CC) $(CLIBOPTS) $(SRC)
	$(LLIB) $(LIBOPTS)$(PBASIS_LIB) $(OBJ)

$(PBASIS_LIB_H): $(SRC) $(HEADERS) $(SRC_HDR) Makefile
	$(CP) $(HEADERS) $@
	for i in $(SRC_HDR); \
	do \
		( $(CP) $$i $(INCLUDE_DIR); ); \
	done

$(PROTO_H): $(PROTO)
	$(CP) $(PROTO) $@

$(PROTO): $(SRC) $(HEADERS)
	cproto_all "$(SRC)" $(INCLUDE_DIR) $@

clean_obj:
	-@$(RM) $(OBJ)

clean: clean_obj
	-@$(RM) $(PBASIS_LIB)
	-@$(RM) $(PBASIS_LIB_H)
	-@$(RM) $(PROTO_H)
	-@$(RM) cscope.out
	for i in $(SRC_HDR); \
	do \
		( $(RM) $(INCLUDE_DIR)/$$i; ); \
	done

backup: clean
	-@$(RM) ../pbasis.tar
	tar cvf ../pbasis.tar ../pbasis
