#!/usr/bin/env python3
"""Simple parser for Robot and serial session logs.

Usage:
    python scripts/parse_logs.py logs/
    python scripts/parse_logs.py logs/samv71_serial_xxx.log
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

TX_RE = re.compile(r"] TX: (.*)")
RX_RE = re.compile(r"] RX: (.*)")
WARN_RE = re.compile(r"] WARN: (.*)")
INFO_RE = re.compile(r"] INFO: (.*)")


def summarize_file(path: Path) -> str:
    tx = rx = warn = info = 0
    commands: list[str] = []
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if TX_RE.search(line):
            tx += 1
            commands.append(TX_RE.search(line).group(1))
        if RX_RE.search(line):
            rx += 1
        if WARN_RE.search(line):
            warn += 1
        if INFO_RE.search(line):
            info += 1

    lines = [
        f"FILE: {path}",
        f"  INFO lines : {info}",
        f"  WARN lines : {warn}",
        f"  TX entries  : {tx}",
        f"  RX entries  : {rx}",
    ]
    if commands:
        lines.append("  Commands:")
        for cmd in commands:
            lines.append(f"    - {cmd}")
    return "\n".join(lines)


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2

    target = Path(sys.argv[1])
    if not target.exists():
        print(f"Path not found: {target}")
        return 2

    files = [target] if target.is_file() else sorted(target.glob("*.log"))
    if not files:
        print("No log files found")
        return 1

    for file in files:
        print(summarize_file(file))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
