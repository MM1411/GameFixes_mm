#!/bin/bash
set -e

# external/sourcesdk (Wend4r/sourcesdk, with its protobuf and game protobufs)
# and external/metamod-source (build 1411) come in as submodules.
git submodule update --init --recursive

# SEMVER comes in from the CI, whose version job resolves the tag -- on the
# pushbuild.txt path it is not on HEAD yet, so git describe cannot see it. A
# tag push and a local build still find it on HEAD.
if [ -n "${SEMVER:-}" ]; then
  export SEMVER
  echo "=== Version: $SEMVER (from the environment) ==="
elif SEMVER="$(git describe --tags --exact-match 2>/dev/null)"; then
  export SEMVER
  echo "=== Version: $SEMVER (tag on HEAD) ==="
else
  unset SEMVER
fi

export GITHUB_SHA_SHORT="$(git rev-parse --short HEAD)"

### --- Build ---------------------------------------------------------------
echo "=== Starting build ==="

rm -rf build

CC=clang CXX=clang++ cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

echo "=== DONE ==="
