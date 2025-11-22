#!/bin/bash

# Exit on error (optional but recommended)
set -e

# Ensure the user gave an argument
if [ $# -lt 1 ]; then
    echo "Usage: $0 <scene_name (without .json)>"
    exit 1
fi

scene="$1"

# ----- Build -----
cd build 
make

# ----- Run ray tracer -----
./raytracer --input ../../ASCII/${scene}.json --output ../../Output/${scene}.ppm

# ----- Convert to PNG -----
cd ..
cd ..
cd Output
convert "${scene}.ppm" "${scene}.png"

echo "✔ Done: Output/${scene}.png"
