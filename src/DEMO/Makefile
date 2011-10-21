# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: DEMO
# Copyright (C) 2010, 2011 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include			../../include/Make.inc

DEMO_BASIS	=	basis$(EXEEXT)
DEMO_REGEX	=	regex$(EXEEXT)
DEMO_STRING	=	string$(EXEEXT)

DEMO_BASIS_SRC	=	basis.c
DEMO_REGEX_SRC	=	regex.c
DEMO_STRING_SRC	=	string.c

DEMO_BASIS_OBJ	=	basis$(OBJEXT)
DEMO_REGEX_OBJ	=	regex$(OBJEXT)
DEMO_STRING_OBJ	=	string$(OBJEXT)

#-------------------------------------------------------------------------------

all: $(DEMO_BASIS) $(DEMO_REGEX) $(DEMO_STRING)
	@echo
	@echo --- Compilation succeeded! ---
	
$(DEMO_BASIS): $(DEMO_BASIS_OBJ) $(DEMO_BASIS_LIB)
	$(LLINK) $(LINKOPTS)$@ $(DEMO_BASIS_OBJ) $(LIBPHORWARD_LIB)

$(DEMO_REGEX): $(DEMO_REGEX_OBJ) $(DEMO_REGEX_LIB)
	$(LLINK) $(LINKOPTS)$@ $(DEMO_REGEX_OBJ) $(LIBPHORWARD_LIB)

$(DEMO_STRING): $(DEMO_STRING_OBJ) $(DEMO_STRING_LIB)
	$(LLINK) $(LINKOPTS)$@ $(DEMO_STRING_OBJ) $(LIBPHORWARD_LIB)

$(DEMO_BASIS_OBJ): $(DEMO_BASIS_SRC)
	$(CC) $(CEXEOPTS) $(DEBUG) $?

$(DEMO_REGEX_OBJ): $(DEMO_REGEX_SRC)
	$(CC) $(CEXEOPTS) $(DEBUG) $?

$(DEMO_STRING_OBJ): $(DEMO_STRING_SRC)
	$(CC) $(CEXEOPTS) $(DEBUG) $?

clean_obj:
	-$(RM) $(DEMO_BASIS_OBJ)
	-$(RM) $(DEMO_REGEX_OBJ)
	-$(RM) $(DEMO_STRING_OBJ)

clean: clean_obj
	-$(RM) $(DEMO_BASIS)
	-$(RM) $(DEMO_REGEX)
	-$(RM) $(DEMO_STRING)

backup: clean
	-@$(RM) ../DEMO.tar
	tar cvf ../DEMO.tar ../DEMO

