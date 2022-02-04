#!/bin/bash

VERSION=1.74.0

curl -OL https://gitlab.com/lkkmpn/boost-mingw/-/releases/${VERSION}/downloads/boost-mingw-w64-x86-64-posix-${VERSION}.tar.bz2
tar xf boost-mingw-w64-x86-64-posix-${VERSION}.tar.bz2
mv boost-mingw-w64-x86-64-posix-${VERSION} boost-mingw-w64-x86-64-posix
