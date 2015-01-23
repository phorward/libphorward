
PATHEXT = PATH="../../run:$(PATH)"

# generating parser for ppgram
mygram.c: gram.syn mygram.tpl.c
	$(PATHEXT) filehead mygram.c "Self-contained parser for ppgram." >$@
	$(PATHEXT) pinclude mygram.tpl.c >>$@
	

clean:
	rm mygram.c
