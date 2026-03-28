#!/bin/bash
DIR_NAME=build_emscripten
rm -r $DIR_NAME

emcmake cmake -B $DIR_NAME -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel . 
cd $DIR_NAME
cmake --build . 
python -m http.server 8000 --bind 0.0.0.0
