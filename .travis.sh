#!/bin/sh
set -ex
./configure --with-trace
make
sudo make install
pparse "f: /[0-9]+/@int | '(' e ')' ; mul@: t '*' f ; t: mul@ | f; add@: e '+' t; e: add@ | t ;"  "1+2*3+4*5"


