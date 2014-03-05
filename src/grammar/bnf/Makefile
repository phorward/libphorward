
../bnfgen.h: bnf
	awk -f csv2pg.awk $? >$@
	touch ../bnf.c

clean:
	-rm -f ../bnfgen.h
