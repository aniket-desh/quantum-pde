#!/bin/bash
set -e

# 1: ensure checkpoints and plots directories exist in project root
echo "[1/5] Ensuring checkpoints and plots directories exist..."
mkdir -p checkpoints/plots

# 2: ruild the project
cd "$(dirname "$0")/.."  # Go to project root
PROJECT_ROOT=$(pwd)
cd build

echo "[2/5] Building the project..."
cmake ..
make

# 3: run the solver from project root so output goes to checkpoints/
echo "[3/5] Running the solver (output to $PROJECT_ROOT/checkpoints)..."
cd "$PROJECT_ROOT"
./build/demo

# 4: run the visualization script on checkpoints/
echo "[4/5] Running visualization script..."
python3 python/visualize_results.py checkpoints

echo "[5/5] All steps completed. Plots should be in checkpoints/plots/" 