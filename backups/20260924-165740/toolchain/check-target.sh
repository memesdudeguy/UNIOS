#!/usr/bin/env bash
set -euo pipefail

compiler="${1:-./toolchain/aliencc}"

target="$("$compiler" -dumpmachine)"
echo "target: $target"

case "$target" in
    i386-unknown-none-elf)
        echo "UNIOS target: valid"
        ;;
    *)
        echo "UNIOS target: invalid: $target" >&2
        exit 1
        ;;
esac
