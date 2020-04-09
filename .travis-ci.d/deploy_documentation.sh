#!/bin/bash

source $HOME/setup.sh

cd /Package
mkdir -p build
cd build

# Re-run the CMake command with MKDocs ON. 
# This keeps the options from the previous build, 
# so the doxygen sources will be populated according to the options 
# switched ON before running this. 
cmake \
  -C $HOME/CMakeCache.cmake \
  -DCMAKE_INSTALL_PREFIX=$PWD/../install \
  -DMARLINMT_MKDOCS=ON \
  ..
  
# Run mkdocs
make mkdocs

# setup the remote with the access token
cd ..
git remote rm origin
git remote add origin https://rete:$GITHUB_TOKEN@github.com/iLCSoft/MarlinMT.git

# copy the site to the Package directory.
# Can't relocate the site dir using gh-deploy 
cp -r build/MkDocs/site .

# Push the doc
mkdocs gh-deploy --force
