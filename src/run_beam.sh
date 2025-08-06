#!/bin/bash

echo "=== 3D Beam FEM Analysis with Animation ==="
echo "Compiling..."
make clean && make

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo ""
    echo "=== Beam Model Features ==="
    echo "• 3D beam model with 20 nodes and 32 elements"
    echo "• Animated deformation under load"
    echo "• Stress visualization with color coding"
    echo "• Pulsating load indicator"
    echo ""
    echo "=== Controls ==="
    echo "  A - Toggle animation (deformation)"
    echo "  S - Toggle stress visualization"
    echo "  D - Toggle deformed view"
    echo "  L - Toggle load display"
    echo "  1/2/3 - Deformation scale (0.5/1.0/2.0)"
    echo "  R - Reset view"
    echo "  X/Y/Z - Rotate around X/Y/Z axis"
    echo "  +/- - Zoom in/out"
    echo "  Q/ESC - Quit"
    echo ""
    echo "Starting program..."
    echo ""
    ./fem3d
else
    echo "Compilation failed!"
    exit 1
fi 