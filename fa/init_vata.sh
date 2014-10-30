#! /bin/bash

cd libvata
if [ ! -d build ]; then
	mkdir build
fi
make release
