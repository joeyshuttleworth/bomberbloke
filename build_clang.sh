#!/bin/bash

clang_c=/usr/bin/clang-7
clang_cxx=/usr/bin/clang-cpp-7

if [ ! -f "$clang_c" ]; then
	clang_c=/usr/bin/clang
	clang_cxx=/usr/bin/clang++
fi

cmake -DCMAKE_C_COMPILER=$clang_c -DCMAKE_CXX_COMPILER=$clang_cxx clean .
cmake -DCMAKE_C_COMPILER=$clang_c -DCMAKE_CXX_COMPILER=$clang_cxx .

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	make -j || nproc #Pass number of cpu cores
else
	make -j || sysctl -n hw.logicalcpu # Use mac command
fi
