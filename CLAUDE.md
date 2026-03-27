# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Goal

An interactive ocean wave simulation and visualization — a navigable patch of ocean rendered in real time. The simulation uses Gerstner waves for physically-plausible (not photorealistic) water surface displacement, with procedural audio, a floating buoy, and a skybox. It runs as both a native desktop app and a browser app (WebAssembly).

## Build Commands

**Native desktop build:**
```bash
cmake -B build -G Ninja .
cmake --build build
./build/ocean-gestalt
# Optionally pass a URL to a uniforms JSON:
./build/ocean-gestalt <url-to-uniforms-json>
```

**Web/WebAssembly build** (requires Emscripten):
```bash
bash web_build.sh
# Builds to build_emscripten/ and starts HTTP server on :8000
```

**Run tests** (native only):
```bash
cmake --build build --target test
# or run the test binary directly after building
```

## Architecture

Ocean Gestalt is an interactive Gerstner wave ocean simulation that compiles to both native desktop (OpenGL) and browser (WebAssembly/WebGL2) from the same C++17 codebase.

**Core layers:**

- `src/main.cpp` — Entry point; initializes the app, REST server (native only), and optional URL-based uniform loading.
- `src/core/Application` — GLFW window, OpenGL context, input loop, frame timing.
- `src/core/OceanGestalt` — Scene coordinator; owns and ticks all scene objects (ocean, buoy, camera, skybox, light, audio).
- `src/core/Configuration` — Central app state; holds shader programs, wave config, camera config, and the API reference. Nearly everything accesses state through here.
- `src/core/Ocean` — The ocean mesh, LOD surface, and wave rendering.
- `src/core/GerstnerWave` + `Wave` + `WaveGenerator` — Wave physics: individual wave params → Gerstner displacement → mesh vertex positions.
- `src/core/HeightMapGenerator` — Accumulates Gerstner displacements across all waves to compute vertex heights.
- `src/core/SurfAudio` — Procedural surf audio via miniaudio, driven by wave dynamics and camera position.
- `src/api/OceanApi` — The main programmatic API for adjusting simulation parameters at runtime.
- `src/api/UniformState` — Shader uniform values managed at the API layer.
- `src/api/PathHandler` — Routes API calls to handlers; tested in `tests/`.
- `src/rest/RestServer` — CivetWeb-based HTTP server (native only, port 8080) exposing REST endpoints for uniform and pause control.

**Dual-build differences:**
- Native: OpenGL 4.x, compute shaders, normal maps, REST API active.
- Web: WebGL2, GLSL ES shaders (`data/shader/webgl/`), no REST server; the HTML/JS UI (`src/index.html`, `src/keypress.js`) drives the API directly.

**Configuration:** All wave, shader, rendering, and API parameters are JSON files under `data/config/`. Shader sources are in `data/shader/webgl/`.

**Key libraries** (vendored under `lib/`): GLFW, GLEW, GLM, miniaudio, CivetWeb, stb. `nlohmann/json` is fetched via CMake FetchContent. Tests use `doctest`.
