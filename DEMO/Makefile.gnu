CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

all: basis regex

clean:
	-rm -f basis.o
	-rm -f regex.o
	-rm -f regex
	-rm -f basis

basis: basis.o $(LIBPHORWARD)
	$(CC) -o $@ basis.o $(LIBPHORWARD)

regex: regex.o $(LIBPHORWARD)
	$(CC) -o $@ regex.o $(LIBPHORWARD)
