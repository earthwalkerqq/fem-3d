#!/bin/bash

echo "=== 3D FEM Analysis Test ==="
echo "Building program..."
cd src
make clean
make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running program..."
    echo "Press ESC to exit the visualization window"
    ./fem3d
else
    echo "Build failed!"
    exit 1
fi 