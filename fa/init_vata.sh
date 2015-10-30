#! /bin/bash

#cd libvata
#git submodule init
#git submodule update
#make release

cd libvata
if [ ! -d build ]; then
	mkdir build
fi
make release
