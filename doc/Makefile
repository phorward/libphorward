#!/bin/make

MAINSRC = phorward.t2t
REFSRC = ref.t2t

SRC = 	array.t2t \
		ccl.t2t \
		list.t2t \
		regex.t2t \
		\
		$(MAINSRC) \
		$(REFSRC)

all: $(SRC)
	txt2tags -t html $(MAINSRC)

clean:
	rm -f phorward.html
	rm -f ref.t2t

$(REFSRC):
	cd ../src; make -f Makefile.gnu ref
