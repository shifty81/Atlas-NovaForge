#!/usr/bin/env python3
"""
Atlas Contract Scanner

Scans simulation source files for forbidden API usage that
violates the Atlas Core Contract.

Usage:
    python tools/contract_scan.py [--path ENGINE_DIR]

Returns exit code 1 if violations are found.
"""

import argparse
import pathlib
import sys

# APIs forbidden in simulation code
FORBIDDEN_PATTERNS = [
    ("std::chrono", "Non-deterministic time source"),
    ("time(", "Wall-clock time access"),
    ("rand(", "Non-deterministic randomness"),
    ("<random>", "OS-dependent random header"),
    ("srand(", "Non-deterministic seed"),
    ("clock(", "Wall-clock time access"),
    ("gettimeofday", "Wall-clock time access"),
    ("clock_gettime", "Wall-clock time access"),
]

# Directories to scan (relative to engine root)
SIMULATION_DIRS = [
    "ecs",
    "sim",
    "physics",
    "ai",
]

# Files that legitimately use otherwise-forbidden APIs
SKIP_FILES = {
    "TickScheduler.cpp",
    "TickScheduler.h",
}

SOURCE_EXTENSIONS = {".cpp", ".h", ".hpp", ".cxx"}


def scan_file(filepath: pathlib.Path) -> list:
    """Scan a single file for contract violations."""
    violations = []
    try:
        text = filepath.read_text(errors="ignore")
    except OSError:
        return violations

    for pattern, reason in FORBIDDEN_PATTERNS:
        if pattern in text:
            violations.append(
                f"{filepath}: uses forbidden API `{pattern}` -- {reason}"
            )

    return violations


def main():
    parser = argparse.ArgumentParser(description="Atlas Contract Scanner")
    parser.add_argument(
        "--path",
        default="engine",
        help="Root directory to scan (default: engine)",
    )
    args = parser.parse_args()

    root = pathlib.Path(args.path)
    if not root.exists():
        print(f"Error: directory {root} not found", file=sys.stderr)
        sys.exit(2)

    all_violations = []

    for sim_dir in SIMULATION_DIRS:
        scan_path = root / sim_dir
        if not scan_path.exists():
            continue
        for filepath in scan_path.rglob("*"):
            if filepath.suffix in SOURCE_EXTENSIONS:
                if filepath.name in SKIP_FILES:
                    continue
                all_violations.extend(scan_file(filepath))

    # Also scan entire tree for banned library usage
    for filepath in root.rglob("*"):
        if filepath.suffix in SOURCE_EXTENSIONS:
            try:
                text = filepath.read_text(errors="ignore")
            except OSError:
                continue
            for pattern, reason in [
                ("imgui.h", "ImGui is banned -- use Atlas custom UI"),
                ("nuklear.h", "Nuklear is banned -- use Atlas custom UI"),
            ]:
                if pattern in text:
                    all_violations.append(
                        f"{filepath}: uses banned library `{pattern}` -- {reason}"
                    )

    if all_violations:
        print(f"FAIL: {len(all_violations)} contract violation(s) found:\n")
        for v in all_violations:
            print(f"  {v}")
        sys.exit(1)
    else:
        print("PASS: No contract violations found.")
        sys.exit(0)


if __name__ == "__main__":
    main()
