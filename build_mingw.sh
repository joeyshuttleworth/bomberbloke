#!/bin/bash

mkdir mingw_build
cd mingw_build

cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_CLAR=OFF -DTHREADSAFE=ON -DCMAKE_SYSTEM_NAME=Generic -D_WIN32=TRUE -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc-posix -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-gcc-posix -DCMAKE_RC_COMPILER="$(which x86_64-w64-mingw32-windres)" -DDLLTOOL="$(which x86_64-w64-mingw32-dlltool)" -DCMAKE_C_COMPILER_WORKS=1 -DCMAKE_CXX_COMPILER_WORKS=1 ..

make
