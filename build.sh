#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 [--client|--server]"
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

ARTIFACTS_DIR="${ROOT_DIR}/artifacts"
PRODUCTS_DIR="${ARTIFACTS_DIR}/artifacts"
CMAKE_CACHE_DIR="${ARTIFACTS_DIR}/cmake_cache"

mkdir -p "$PRODUCTS_DIR" "$CMAKE_CACHE_DIR"

BUILD_CLIENT=OFF
BUILD_SERVER=OFF

case "$1" in
  --client)
    BUILD_CLIENT=ON
    PROJECT_NAME="stx-server"
    ;;
  --server)
    BUILD_SERVER=ON
    PROJECT_NAME="stx-server"
    ;;
  *)
    echo "Usage: $0 [--client|--server]"
    exit 1
    ;;
esac

if [[ "$BUILD_CLIENT" == "ON" && "$BUILD_SERVER" == "OFF" ]]; then
  PROJECT_NAME="stx-client"
# elif [[ "$BUILD_CLIENT" == "OFF" && "$BUILD_SERVER" == "ON" ]]; then
#   PROJECT_NAME="stx-server"
else
  PROJECT_NAME="stx-server"
fi

echo "Configuring build: CLIENT=$BUILD_CLIENT SERVER=$BUILD_SERVER PROJECT_NAME=$PROJECT_NAME"
cmake -S "$ROOT_DIR" -B "$CMAKE_CACHE_DIR" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$PRODUCTS_DIR" \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="$PRODUCTS_DIR" \
    -DBUILD_CLIENT=$BUILD_CLIENT \
    -DBUILD_SERVER=$BUILD_SERVER \
    -DPROJECT_NAME="$PROJECT_NAME"

echo "Building..."
cmake --build "$CMAKE_CACHE_DIR"

echo "Build completed. Artifacts in: $PRODUCTS_DIR"
