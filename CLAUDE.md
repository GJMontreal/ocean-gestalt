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

## Spray Particle System

`src/core/SprayParticleSystem` is a CPU-simulated, GPU-instanced particle system for ocean spray at wave crests.

**Goal:** A convincing mist/spray cloud that rises from breaking wave crests and falls back to the surface. The effect should read as a group — individual particles should not be recognisable as distinct shapes. Accumulation of many semi-transparent soft sprites (additive blending) builds the cloud brightness, not per-particle opacity.

**How it works:**
- Each frame, a `GRID_SIZE×GRID_SIZE` grid is swept across the ocean. At each cell, per-wave `cos(phase)` detects crests (=1 at crest). Amplitude×steepness-weighted crestness drives both spawn probability and launch velocity (derived from wave phase speed).
- Particles are launched with a mostly-horizontal velocity matching wave propagation direction, plus a small upward component. Physics: gravity (~3× real for visual speed), mass-based drag, per-particle swirl.
- GPU side: instanced `GL_TRIANGLE_STRIP` quads, camera-aligned billboards. Each particle uploads `{worldPos, lifetimeFrac, size, velocityDir, seed}`.
- Fragment shader: simple radial soft-disc gradient, very low per-particle alpha. Additive blending (`GL_SRC_ALPHA, GL_ONE`) accumulates to bright white at dense crests.
- The same `crestness` value computed in `gerstner.vert` also drives a whitecap layer directly on the water surface (`water.frag`), so surface foam and airborne spray read as the same substance.

**Known tension:** particle density vs. visual subtlety. More particles → more convincing cloud but higher CPU cost (the grid sweep and pool search are O(N²) and O(MAX_PARTICLES) respectively).
