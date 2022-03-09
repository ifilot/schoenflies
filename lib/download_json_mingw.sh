#!/bin/bash

VERSION=3.10.5-2

curl -OL https://mirror.msys2.org/mingw/mingw64/mingw-w64-x86_64-nlohmann-json-${VERSION}-any.pkg.tar.zst
tar xf mingw-w64-x86_64-nlohmann-json-${VERSION}-any.pkg.tar.zst --use-compress-program=unzstd
mv mingw64 nlohmann-json-mingw-w64-x86-64-posix
