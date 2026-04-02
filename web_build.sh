#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DIR_NAME=build_emscripten
BUILD_DIR="$SCRIPT_DIR/$DIR_NAME"
MODULE_DIR="${OCEAN_MODULE_DIR:-$SCRIPT_DIR/../web/ocean-gestalt-module}"
MODULE_STATIC="$MODULE_DIR/static/ocean-gestalt"

rm -rf "$BUILD_DIR"

emcmake cmake -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel "$SCRIPT_DIR"
cmake --build "$BUILD_DIR"

# Deploy artifacts to Hugo module
echo "Deploying to $MODULE_STATIC..."
cp "$BUILD_DIR/ocean-gestalt.js" \
   "$BUILD_DIR/ocean-gestalt.wasm" \
   "$BUILD_DIR/ocean-gestalt.data" \
   "$BUILD_DIR/keypress.js" \
   "$BUILD_DIR/manifest.json" \
   "$BUILD_DIR/favicon.ico" \
   "$BUILD_DIR/apple-touch-icon.png" \
   "$BUILD_DIR/github-mark-white.png" \
   "$BUILD_DIR/thumbnail.jpg" \
   "$MODULE_STATIC/"
cp "$SCRIPT_DIR/data/config/uniforms_web.json" "$MODULE_STATIC/"

git -C "$MODULE_DIR" add static/ocean-gestalt/

echo ""
echo "Staged changes in ocean-gestalt-module. Suggested commit message:"
echo ""
echo "  Update WASM artifacts $(date +%Y-%m-%d)"
echo ""

# Serve locally for quick testing (hugo server is needed for template testing)
cd "$BUILD_DIR"
python -m http.server 8000 --bind 0.0.0.0
