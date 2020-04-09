#!/bin/bash

source $HOME/setup.sh

cd /Package
mkdir build
cd build

cmake \
  -C $HOME/CMakeCache.cmake \
  -DCMAKE_INSTALL_PREFIX=$PWD/../install \
  -DCMAKE_CXX_STANDARD=17 \
  -DMARLINMT_BOOK=ON \
  -DMARLINMT_LCIO=ON \
  -DMARLINMT_DD4HEP=ON \
  -DMARLINMT_GEAR=ON \
  -DBUILD_TESTING=ON \
  -DMARLINMT_WERROR=ON \
	-DMARLINMT_BOOK_IMPL=root7 \
  .. && \
make install && \
ctest --output-on-failure
