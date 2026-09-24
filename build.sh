#!/usr/bin/env bash
set -euo pipefail
make clean
make
echo
echo "Built: build/unios.iso"
echo "Run:   make run"
