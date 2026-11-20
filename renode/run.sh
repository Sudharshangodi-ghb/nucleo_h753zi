#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
renode="${RENODE:-renode}"
firmware_elf="${FIRMWARE_ELF:-${project_root}/build/nucleo_h753zi.elf}"

if ! command -v "$renode" >/dev/null 2>&1; then
    printf 'Error: Renode executable not found: %s\n' "$renode" >&2
    printf 'Install Renode or set RENODE to its executable path.\n' >&2
    exit 1
fi

renode_root="$(dirname -- "$(readlink -f -- "$(command -v "$renode")")")"
platform_file="${RENODE_PLATFORM_FILE:-${renode_root}/platforms/boards/nucleo_h753zi.repl}"

if [[ ! -f "$firmware_elf" ]]; then
    printf 'Error: firmware ELF not found: %s\n' "$firmware_elf" >&2
    printf 'Build it from the project root with: cmake -S . -B build && cmake --build build\n' >&2
    exit 1
fi

if [[ ! -f "$platform_file" ]]; then
    printf 'Error: NUCLEO-H753ZI Renode platform file not found: %s\n' "$platform_file" >&2
    printf 'Set RENODE_PLATFORM_FILE to the path of nucleo_h753zi.repl.\n' >&2
    exit 1
fi

resc_file="$(mktemp "${TMPDIR:-/tmp}/nucleo_h753zi.XXXXXX.resc")"
trap 'rm -f -- "$resc_file"' EXIT

printf 'mach create "nucleo_h753zi"\nmachine LoadPlatformDescription @%s\ncpu VectorTableOffset 0x08000000\nsysbus LoadELF @%s\nshowAnalyzer sysbus.usart3\n' \
    "$platform_file" "$firmware_elf" > "$resc_file"

"$renode" --console --disable-gui --plain "$resc_file"
