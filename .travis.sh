#!/bin/sh
set -ex
./configure --with-trace
make
sudo make install

# Run test suite
hash -r
LD_LIBRARY_PATH="/usr/lib" LDLIBS="-lphorward" CFLAGS="-DDEBUG -DUTF8 -DUNICODE -g" ptest -v src/array.c src/list.c src/string.c
