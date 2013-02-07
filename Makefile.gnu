all:
	cd src; make -f Makefile.gnu
	cd DEMO; make -f Makefile.gnu

clean:
	cd src; make -f Makefile.gnu clean
	cd DEMO; make -f Makefile.gnu clean

config:
	cp Makefile.gnu Makefile
	cp src/Makefile.gnu src/Makefile
	cp DEMO/Makefile.gnu DEMO/Makefile
	
unconfig:
	-rm Makefile
	-rm src/Makefile
	-rm DEMO/Makefile
