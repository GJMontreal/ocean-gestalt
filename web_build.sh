#!/bin/bash
DIR_NAME=build_emscripten
rm -r $DIR_NAME
mkdir $DIR_NAME
cd $DIR_NAME
emcmake cmake ..
make
python -m http.server 8000
