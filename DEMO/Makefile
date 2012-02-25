# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Libraries :: DEMO
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include ../Make.inc

BASIS = basis$(EXEEXT)
REGEX = regex$(EXEEXT)

all: $(BASIS) $(REGEX)

$(BASIS): basis.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

$(REGEX): regex.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

install:

uninstall:

clean:
	-rm $(BASIS)
	-rm $(REGEX)
