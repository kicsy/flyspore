#!/bin/sh

cd $2

cmake $1 -DCMAKE_BUILD_TYPE=$3

cmake --build .
