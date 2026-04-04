
I wanted to revisit an idea I had some 15 years ago. Nothing new under the sun, but I did enjoy putting this together. I wanted to create something which while hinting at a physical analogue didn't take the approach of trying for verisimilitude.  

You can try out the results at:  
 https://geoffreyjones.ca/ocean-gestalt/

It's a bit of ocean you can fit in your pocket.  


=======================

**Controls**

| Input | Action |
|-------|--------|
| W / A / S / D | Move forward / left / back / right |
| Space / Shift | Move up / down |
| Drag | Look around |
| T | Toggle camera attachment to water surface |
| P | Pause/resume simulation |
| M | Toggle mesh visibility |
| L | Toggle wireframe |
| F | Toggle fullscreen (native only) |
| B | Dump current uniforms to stdout |
| O | Save current uniforms to `data/config/output.json` |
| G | Reload uniforms from config file |
| R | Generate new random wave uniforms |
| / | Toggle HUD text |

=======================

**Running locally (WebGL build)**

Requires [emsdk](https://emscripten.org/docs/getting_started/downloads.html). Activate it before building:
```
source ~/emsdk/emsdk_env.sh
./web_build.sh
```
Then open `http://localhost:8000`. The script builds and serves in one step.

=======================

**Running locally (native build)**

Build:
```
cmake -B build -G Ninja .
cmake --build build
./build/ocean-gestalt
```

The simulation exposes a REST API on port 8080. To run the parameter control UI, open a second terminal:
```
cd web_ui && python3 server.py
```
Then open `http://localhost:8000`. The UI connects to the simulation automatically. Use the Transition slider to animate parameter changes over time rather than snapping.

=======================

I am open to any comments and contributions

Jerry Tessendorf's paper on simulating ocean water.  
https://people.computing.clemson.edu/~jtessen/reports/papers_files/coursenotes2004.pdf  

A guide to implementing Gerstner waves and more 
https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models  

I started with the OpenGL CMake Skeleton.  
https://github.com/ArthurSonzogni/OpenGL_CMake_Skeleton.git 

How it's even possible to get C++ running in a browser   
https://emscripten.org  

An updated glfw api  
https://github.com/pongasoft/emscripten-glfw  

https://learnopengl.com/About  

http://get.webgl.org  

=====================  
A note for android users; I'm not certain this will work on your device. If there's someone who could give me a hand debugging that I'd really appreciate the help. I've seen it work on some handsets and not others     
