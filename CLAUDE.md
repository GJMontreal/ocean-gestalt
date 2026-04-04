# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Native (Desktop OpenGL)
```bash
cmake -B build -G Ninja .
cmake --build build
./build/ocean-gestalt
```

### WebGL / Emscripten
```bash
source ~/emsdk/emsdk_env.sh
./web_build.sh   # cleans build_emscripten/, builds, and serves on localhost:8000
```
The web build uses `-DCMAKE_BUILD_TYPE=MinSizeRel`. Output lands in `build_emscripten/`.

There is no automated test runner — tests are built as part of the native build only.

## Architecture Overview

The application is a real-time ocean simulation with two separate build targets sharing the same core.

### Entry point: `src/main.cpp`
Wires together the main subsystems:
- `OceanGestalt` — GLFW window + OpenGL/WebGL2 render loop
- `UniformState` — central store for all tweakable parameters
- `OceanApi` — bridges `UniformState` to `OceanGestalt`
- `UniformAnimator` — smooth parameter transitions, ticked per frame
- `RestServer` (native only) — CivetWeb HTTP API on port 8080

### Core rendering (`src/core/`, library `app-core`)
`OceanGestalt` is the monolithic scene orchestrator. It owns:
- The GLFW render loop (via `Application`)
- Pre-passes: `ShadowPass`, `ReflectionPass`
- Scene elements (`std::vector<SceneElement>`) — each has an optional `Drawable` and optional `Moveable`
- UBO binding for matrices (`uboMatrices`) shared across shaders

Key rendering classes: `Ocean`, `Buoy`, `Sphere`, `Skybox`, `LODSurface`, `TextRenderer`.

`Ocean` drives Gerstner wave simulation. There are **two implementations in tension**: CPU (`GerstnerWave.cpp`) and GPU (shaders in `data/shader/` and `data/shader/webgl/`). These can diverge.

### API layer (`src/api/`, library `app-api`)
Hierarchical dot-path uniform access (e.g. `"gust.heading"`, `"buoy.position"`). The REST server maps URL paths (`/`) to these dot-paths. `ApiValue` is a type-safe variant; `UniformValue` wraps GLM types. Conversion: `apiToUniform()` / `uniformToApi()`.

### Shader duplication
There are two shader directories:
- `data/shader/` — desktop GLSL
- `data/shader/webgl/` — WebGL2 GLSL ES versions

Changes to shader logic often need to be applied in both places. The web shaders are bundled into the WASM binary via Emscripten `--preload-file`.

### WebGL2 / ANGLE constraints to keep in mind
- Use **sized** internal formats (`GL_RGB8`, `GL_RGBA8`, `GL_DEPTH_COMPONENT24`) — unsized formats like `GL_RGB` are rejected by ANGLE
- `GL_DEPTH_COMPONENT24` requires `GL_UNSIGNED_INT` pixel type (not `GL_FLOAT`); use `GL_DEPTH_COMPONENT32F` for float depth
- Depth-only FBOs need `glDrawBuffers(1, [GL_NONE])` in the `__EMSCRIPTEN__` branch (not `glDrawBuffer(GL_NONE)`)
- `glCheckError()` will surface stale GL errors from earlier calls — an FBO error can appear attributed to a completely unrelated subsequent operation

### Uniform configuration
Wave and scene parameters live in `data/config/`. The web build uses `uniforms_web.json`; native uses `uniforms_min.json`. These are manually maintained and can diverge. Uniforms can also be fetched from a URL at startup.
