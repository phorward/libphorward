CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

PLEX		= ../run/plex
PREGEX		= ../run/pregex

all: $(PLEX) $(PPARSE) $(PREGEX) $(PVM)

clean:
	-rm *.o
	-rm $(PLEX) $(PREGEX)

$(PLEX): plex.o util.o $(LIBPHORWARD)
	$(CC) -o $@ $+

$(PREGEX): pregex.o util.o $(LIBPHORWARD)
	$(CC) -o $@ $+

