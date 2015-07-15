#!/bin/make

MAINSRC = phorward.t2t
REFSRC 	= ref.t2t
HTMLOUT = phorward.html

SRC 	= 	array.t2t \
			ccl.t2t \
			list.t2t \
			regex.t2t \
			\
			$(MAINSRC) \
			$(REFSRC)

all: $(HTMLOUT)

$(HTMLOUT): $(SRC) Makefile
	txt2tags -o $@ -t xhtml --toc --css-suggar --style=phorward.css $(MAINSRC)

clean:
	rm -f $(HTMLOUT)
	rm -f $(REFSRC)

$(REFSRC):
	cd ../src; make -f Makefile.gnu ref

