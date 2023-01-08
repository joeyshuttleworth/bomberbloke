#!/bin/bash

clang_cxx=/usr/bin/clang++-7

if [ ! -f "$clang_cxx" ]; then
	clang_cxx=/usr/bin/clang++
fi

cmake -DCMAKE_CXX_COMPILER=$clang_cxx clean .
cmake -DCMAKE_CXX_COMPILER=$clang_cxx .

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	make -j || nproc
else
	make -j || sysctl -n hw.logicalcpu # Use mac command
fi
