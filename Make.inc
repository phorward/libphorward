PHOME ?= $(shell pwd)/..

OBJEXT = .o
EXEEXT =

CFLAGS += -DUNICODE -DUTF8 -I$(PHOME)/include -funsigned-char
LDFLAGS += -L$(PHOME)/lib -lphorward

LIBPHORWARD = $(PHOME)/lib/libphorward.a

