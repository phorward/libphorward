# -MAKE-------------------------------------------------------------------------
# Phorward Regular Expression Library
# Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com
# ------------------------------------------------------------------------------

include ../../include/Make.inc

SRC			=	pregex.c

OBJ			=	pregex$(OBJEXT)

PROTO		=	pregex.proto.h
PROTO_H		=	$(INCLUDE_DIR)$(PATH_SEP)$(PROTO)

HEADERS		=	pregex.h

# ------------------------------------------------------------------------------

all: $(PREGEX_H) $(PROTO_H) $(PREGEX_LIB)
	@echo
	@echo --- Compilation succeeded! ---

$(PREGEX_LIB): $(SRC) $(HEADERS) Makefile
	$(CC) $(CLIBOPTS) $(SRC)
	$(LLIB) $(LIBOPTS)$(PREGEX_LIB) $(OBJ)

$(PREGEX_LIB_H): $(SRC) $(HEADERS) Makefile
	$(CP) $(HEADERS) $@

$(PROTO_H): $(PROTO)
	$(CP) $(PROTO) $@

$(PROTO): $(SRC) $(HEADERS)
	cproto_all "$(SRC)" $(INCLUDE_DIR) $@

clean_obj:
	-@$(RM) $(OBJ)

clean: clean_obj
	-@$(RM) $(PREGEX_LIB)
	-@$(RM) $(PREGEX_LIB_H)
	-@$(RM) $(PROTO_H)
	-@$(RM) cscope.out

backup: clean
	-@$(RM) ../pregex.tar
	tar cvf ../pregex.tar ../pregex
