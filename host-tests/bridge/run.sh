#!/bin/sh
# The bridge transports' TLS pre-flight (src/apps_local/bridge/SyncHeadroom.h).
# Freestanding: no Arduino, no device.
set -e
cd "$(dirname "$0")"
BUILD_DIR="${TMPDIR:-/tmp}/$(basename "${CXX:-c++}")-bridge-tests-$(cd ../.. && pwd | cksum | cut -d" " -f1)"
mkdir -p "$BUILD_DIR"
"${CXX:-c++}" -std=c++17 -O2 -Wall -Wextra -Werror -I../../src/apps_local/bridge \
  test_sync_headroom.cpp -o "$BUILD_DIR/test_sync_headroom"
"$BUILD_DIR/test_sync_headroom"
