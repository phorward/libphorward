# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Libraries :: min_lalr1
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include ../Make.inc

MIN_LALR1 = $(PHOME)/run/min_lalr1$(EXEEXT)

all: $(MIN_LALR1)

$(MIN_LALR1): min_lalr1.c
	$(CC) $(CFLAGS) -o $@ $+

install: $(MIN_LALR1) $(INSTPATH_BIN)
	cp $(MIN_LALR1) $(INSTPATH_BIN)

uninstall: 
	-rm -f $(INSTPATH_BIN)/$(shell basename $(MIN_LALR1))

clean:
	-rm $(MIN_LALR1)
