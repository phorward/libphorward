# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Libraries :: run
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include ../Make.inc

SCRIPTS = pdbgmake pdoc pdostounix pinclude pproto premcomments

all: $(SCRIPTS)

install: $(SCRIPTS)
	for i in $(SCRIPTS); do cp $$i $(INSTPATH_BIN); done

uninstall:
	-for i in $(SCRIPTS); do rm -f $(INSTPATH_BIN)/$$i; done

clean:
