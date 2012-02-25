# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Libraries :: NEWPROJECT
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include ../Make.inc

NEWPROJECT = newproject$(EXEEXT)
NEWPROJECT_SRC = main.c
NEWPROJECT_PROTO = proto.h

RUNDIR = PATH="../run:$(PATH)"

all: $(NEWPROJECT) 

$(NEWPROJECT_PROTO): $(NEWPROJECT_SRC)
	$(RUNDIR) pproto $(NEWPROJECT_SRC) >$@

$(NEWPROJECT): $(NEWPROJECT_SRC) $(NEWPROJECT_PROTO)
	$(CC) $(CFLAGS) -o $@ $(NEWPROJECT_SRC) $(LDFLAGS)

install:

uninstall:

clean:
	-rm $(NEWPROJECT)
