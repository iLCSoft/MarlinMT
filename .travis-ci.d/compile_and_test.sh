#!/bin/bash

source $HOME/setup.sh

cd /Package
mkdir build
cd build

cmake \
  -C $HOME/CMakeCache.cmake \
  -DCMAKE_INSTALL_PREFIX=$PWD/../install \
  -DCMAKE_CXX_STANDARD=17 \
  -DMARLIN_BOOK=ON \
  -DMARLIN_LCIO=ON \
  -DMARLIN_DD4HEP=ON \
  -DMARLIN_GEAR=ON \
  -DBUILD_TESTING=ON \
  -DMARLIN_WERROR=ON \
	-DMARLIN_BOOK_IMPL=root7 \
  .. && \
make install && \
ctest --output-on-failure
