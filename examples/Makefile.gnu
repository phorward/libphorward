CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LDFLAGS += -L../src -lphorward

SRC = 	lexing.c \
		regex.c

BIN = 	$(patsubst %.c,%,$(SRC))

% : %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

all: $(BIN)

clean:
	-rm $(BIN)

