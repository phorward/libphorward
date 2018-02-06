#!/bin/sh
set -ex
./configure
make
sudo make install
pparse "f: /\\d+/@int | '(' e ')' ; mul@: t '*' f ; t: mul@ | f; add@: e '+' t; e: add@ | t ;"  "1+2*3+4*5"
