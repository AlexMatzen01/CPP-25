#!/bin/bash
# Fast parallel build script using Make
# Usage: ./build.sh [fast|debug|release|clean]

BUILD_TYPE="${1:-fast}"
JOBS="${JOBS:-4}"  # Default to 4 jobs, can be overridden with JOBS=8 ./build.sh

echo "🔨 Building AdventureQuestEngine ($BUILD_TYPE) with $JOBS parallel jobs..."

# Build with make
make -j$JOBS "$BUILD_TYPE"

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo "Run with: ./bin/AdventureQuestEngine"
else
    echo "✗ Build failed"
    exit 1
fi
