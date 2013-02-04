all:
	cd src; make -f Makefile.gnu
	cd DEMO; make -f Makefile.gnu

clean:
	cd src; make -f Makefile.gnu clean
	cd DEMO; make -f Makefile.gnu clean
