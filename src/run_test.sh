#!/bin/bash

echo "=== 3D FEM Analysis Test ==="
echo "Compiling..."
make clean && make

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Starting program..."
    echo "Controls:"
    echo "  R - Reset view"
    echo "  X/Y/Z - Rotate around X/Y/Z axis"
    echo "  +/- - Zoom in/out"
    echo "  S - Toggle stress visualization"
    echo "  D - Toggle deformed view"
    echo "  V - Toggle value display"
    echo "  A - Toggle animation"
    echo "  Q/ESC - Quit"
    echo ""
    ./fem3d
else
    echo "Compilation failed!"
    exit 1
fi 