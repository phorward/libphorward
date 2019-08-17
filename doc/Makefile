#!/bin/make

MAINSRC = phorward.t2t
REFSRC 	= ref.t2t
HTMLOUT = phorward.html
MDOUT = phorward.md
TXTOUT  = phorward.txt

SRC 	= 	array.t2t \
			ccl.t2t \
			list.t2t \
			regex.t2t \
			\
			$(MAINSRC) \
			$(REFSRC)

all: $(HTMLOUT) $(MDOUT) $(TXTOUT)

$(HTMLOUT): $(SRC) phorward.css Makefile
	txt2tags -o $@ -t xhtml --toc --css-sugar --style=phorward.css $(MAINSRC)

$(MDOUT): $(SRC) Makefile
	txt2tags -o $@ -t md --toc $(MAINSRC)

$(TXTOUT): $(SRC) Makefile
	txt2tags -t txt -H --toc -o - $(MAINSRC) | sed -E -n '1h;1!H;$${;g;s/ +([-A-Z ]+)\n +(=+)/\1\n\2/g;p;}' | sed 's/^  /    /g' >$@

clean:
	rm -f $(HTMLOUT) $(MDOUT) $(TXTOUT)
	rm -f $(REFSRC)

$(REFSRC):
	cd ../src; make -f Makefile.gnu ref

