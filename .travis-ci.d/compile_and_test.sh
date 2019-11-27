#!/bin/bash

source setup.sh

cd Package
mkdir build
cd build
# TODO enable 'warning as error' asap
cmake -GNinja \
  -C ../../CMakeCache.cmake 
  -DCMAKE_INSTALL_PREFIX=$PWD/../install \
  -DCMAKE_CXX_STANDARD=17 \
  -DMARLIN_BOOK=ON \
  -DMARLIN_LCIO=ON \
  -DMARLIN_DD4HEP=ON \
  -DMARLIN_GEAR=ON \
  -DBUILD_TESTING=ON \
  ../../../local/soft/MarlinMT && \
ninja -k0 && \
ninja install && \
ctest --output-on-failure
