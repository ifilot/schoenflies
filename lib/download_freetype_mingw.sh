#!/bin/bash

VERSION=2.10.4

curl -OL https://gitlab.com/lkkmpn/freetype-mingw/-/releases/${VERSION}/downloads/freetype-mingw-w64-x86-64-posix-${VERSION}.tar.bz2
tar xf freetype-mingw-w64-x86-64-posix-${VERSION}.tar.bz2
mv freetype-mingw-w64-x86-64-posix-${VERSION} freetype-mingw-w64-x86-64-posix
