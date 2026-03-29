# Architecture Notes

Honest assessment of the current state and where the seams are showing.

## What's working well

- The `ApiValue → UniformValue → GL dispatch` pipeline via `std::variant` and the visitor pattern is type-safe, extensible, and clean.
- Treating shader parameters as a first-class REST resource is a smart design — tune the simulation without recompiling.
- `UniformAnimator` with per-path cancellation is a nice touch most projects wouldn't bother with.
- The JSON-driven web UI config is a good idea in principle.

---

## Concerns

### Dual implementation of wave physics
Gerstner wave height is calculated in both `GerstnerWave.cpp` (CPU) and in the vertex/fragment shaders (GPU). We had a bug where they diverged because one used `fmod` and the other didn't. Whenever the same physical model exists in two languages, that gap will keep producing bugs. The CPU version is only needed for buoyancy — something that could be driven by sampling the GPU output or accepting a small approximation.

### Shader duplication
Every shader exists twice: `data/shader/` and `data/shader/webgl/`. The differences are mostly mechanical — `in/out` vs attribute/varying, UBO vs flat uniforms, `gl_ClipDistance`. Changes have to be applied by hand to both. A preprocessor pass or build-time generation step that produces the WebGL versions from the desktop versions would eliminate an entire class of maintenance burden.

### Uniform setting has two competing paths
Some uniforms are pushed explicitly every frame in `drawMesh()` — `time`, `lightPos`, `viewPos`, all ten wave arrays. Others come through the REST/uniform-state system. The explicit `drawMesh()` calls silently win and can't be overridden at runtime. The boundary between "owned by the render loop" and "tunable at runtime" is implicit and undocumented.

### Web UI configs are a manually-maintained shadow of the shaders
Adding a uniform to a shader and a default to `uniforms_min.json` still doesn't make it appear in the UI — you also have to update the relevant `*.json` page config by hand. The REST API already has the full uniform list (it seeds from the shader programs at startup). The web UI could query that and render controls dynamically, eliminating `buoy.json`, `gust.json`, etc. as a maintenance concern entirely.

### `OceanGestalt.cpp` is doing too much
It manages scene construction, the render loop, the shadow pre-pass, the reflection pre-pass, UBO swapping, and uniform orchestration. Five distinct responsibilities. The blast radius of adding anything new — a second light, a new renderable, a post-process pass — is large.

### Path routing has sharp edges
The `/` → `.` URL normalization combined with dotted JSON keys having semantic meaning is non-obvious. `"gust.heading"` as a JSON key encodes something specific about the resulting URL structure. The `heading` → `direction` conversion is hardcoded as a string comparison in `parseUniformPath`. The comment in that function already says "this seems very fragile." It is.

### Wave uniforms set as individual calls
`Sphere::drawMesh` makes ~50 individual `glUniform` calls per frame for the wave array. Not a correctness issue but it adds up. A UBO for wave parameters would be cleaner and faster, and would make the wave data accessible to all shaders without redundant uploads.

---

## The underlying pattern

Most of these share the same root cause: the project grew organically and the abstractions haven't been revisited to match what the system actually became. The REST API and uniform system are sophisticated enough that a lot of the manual scaffolding — explicit per-frame uniform setting, duplicate shader maintenance, manually-curated UI configs — could be replaced or generated. The bones are good; it needs a pass where the seams are closed.
