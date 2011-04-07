# -MAKE-------------------------------------------------------------------------
# min_lalr1 - Experimental Minimalist LALR(1) Parser Generator
# Copyright (C) 2007-2011 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------
include		../../include/Make.inc

PROGRAM		=	$(RUN_DIR)$(PATH_SEP)$(MIN_LALR1)$(EXEEXT)

SRC			=	min_lalr1.c
OBJ			=	min_lalr1$(OBJEXT)
HEADERS		=	

#-------------------------------------------------------------------------------

all: $(PROGRAM)
	@echo
	@echo --- Compilation succeeded! ---
	
$(PROGRAM): $(SRC) $(HEADERS) Makefile
	$(CC) $(CEXEOPTS) $(SRC)
	$(LLINK) $(LINKOPTS)$@ $(OBJ)

clean_obj:
	-@$(RM) $(OBJ)

clean: clean_obj
	-@$(RM) $(PROGRAM)
