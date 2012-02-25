# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Libraries :: Global Makefile
# Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

DIRS = src min_lalr1 run DEMO NEWPROJECT

all:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done

install:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done

uninstall:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done

clean:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done
	
