CFLAGS=-DUNICODE -DUTF8 -I../src

all: basis regex

clean:
	-rm -f basis.o
	-rm -f regex.o
	-rm -f regex
	-rm -f basis

basis: basis.o
	$(CC) -o $@ $? ../src/libphorward.a

regex: regex.o
	$(CC) -o $@ $? ../src/libphorward.a
