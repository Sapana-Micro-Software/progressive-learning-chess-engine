#!/bin/bash

# Build script for Curriculum Chess Learning System

set -e

echo "Building Curriculum Chess Learning System..."

# Create build directory
mkdir -p build
cd build

# Build CLI
echo "Building CLI..."
make -f ../Makefile cli

# Build GUI (macOS only)
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Building GUI..."
    make -f ../Makefile gui
    echo "GUI built: CurriculumChess.app"
else
    echo "GUI build skipped (macOS only)"
fi

echo "Build complete!"
echo ""
echo "Usage:"
echo "  CLI: ./build/curriculum_chess train"
echo "  GUI: open build/CurriculumChess.app"
