#!/bin/bash

VERSION=5.15.2

curl -OL https://gitlab.com/lkkmpn/qtbase-mingw/-/releases/${VERSION}/downloads/qtbase-mingw-w64-x86-64-posix-${VERSION}.tar.bz2
tar xf qtbase-mingw-w64-x86-64-posix-${VERSION}.tar.bz2
mv qtbase-mingw-w64-x86-64-posix-${VERSION} qtbase-mingw-w64-x86-64-posix
