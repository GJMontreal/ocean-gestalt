<html>
    <canvas id="canvas" oncontextmenu="event.preventDefault()">
    </canvas>

    <script>
      var canvas = document.getElementById('canvas');

      window.Module = {
        preRun: [],
        postRun: [],
        canvas: canvas,
        print: console.log,
        printErr: console.error,
      };
    </script>

    <script async src="ocean-gestalt.js"></script>
</html>

OpenGL CMake Skeleton [![Build Status](https://travis-ci.org/ArthurSonzogni/OpenGL_CMake_Skeleton.svg?branch=master)](https://travis-ci.org/ArthurSonzogni/OpenGL_CMake_Skeleton)
=======================

A ready to use C++11 CMake OpenGL skeleton using **GLFW**, **Glew** and **glm**. (use git submodules)

It compiles on:
 * **WebAssembly**  (on branch webassembly. See instruction below)
 * Linux
 * Windows
 * Mac

It can compile for the Web with WebAssembly thanks to emscripten, as well as on
Linux, Windows and Mac.

Shader class and example Application are included.

![output result](output.gif)

I am open to any comments and contributions.

Clone (With submodules):
========================

```
git clone --recursive git@github.com:ArthurSonzogni/OpenGL_CMake_Skeleton.git
```

Alternatively, if you don't used the --recursive option, you can type:
```bash
git submodule init
git submodule update
```

usage (WebAssembly) : 
---------------------

Install emscripten, then
```bash
mkdir build_emscripten
cd build_emscripten
emcmake cmake ..
make
python -m SimpleHTTPServer 8000
```

Now, visit [http://localhost:8000](http://localhost:8000)

usage (Linux) : 
---------------
Install some standard library, fetch the project, build and run:
```bash
sudo apt-get install cmake libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev
git clone --recursive git@github.com:ArthurSonzogni/OpenGL_CMake_Skeleton.git
cd OpenGL_CMake_Skeleton
mkdir build
cd build
cmake ..
make -j
./opengl-cmake-skeleton
```

usage (Windows) :
-----------------
For instance :
* cmake-gui .
* Configure (Choose for example Visual Studio generator)
* Generate
Launch the generated project in your favorite IDE and run it.

git@github.com:ArthurSonzogni/OpenGL_CMake_Skeleton.git

https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models

https://learnopengl.com/About

