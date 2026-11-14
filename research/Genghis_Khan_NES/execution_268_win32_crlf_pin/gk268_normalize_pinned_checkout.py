#!/usr/bin/env python3
"""EXEC0268: preserve exact source SHA across Windows Git CRLF checkout.

Only canonicalize CRLF to LF when that produces the already-approved original
source SHA. Any other corruption is refused before writing. No pin bypass.
"""
from __future__ import annotations

import hashlib
import os
from pathlib import Path
import re
import sys
import tempfile

PINS = {
    "gk1_pad_router.c": "32fb1ac5f23520eeaf4a42aa1c6a147d9d2df9587b11f2f224604612d010ccbd",
    "gk1_pad_router.h": "481b9f39103721369e24214d2d6510d10f01ce7d7dc2b4d2021a81e43f9136ba",
    "test_gk266_controller_chord.c": "c628c476fe003d1fe5cf4564ad074499152eafa29559daaeda3d61a145e497e8",
}


def normalize_pinned(path: Path, expected_sha: str) -> str:
    """Verify entire source before modification; return canonical digest."""
    if path.name not in PINS or PINS[path.name] != expected_sha:
        raise ValueError("unexpected filename or SHA pin")
    if not re.fullmatch(r"[0-9a-f]{64}", expected_sha):
        raise ValueError("invalid SHA-256 pin")
    original = path.read_bytes()
    canonical = original.replace(b"\r\n", b"\n")
    if b"\r" in canonical:
        raise ValueError("non-CRLF carriage return found")
    digest = hashlib.sha256(canonical).hexdigest()
    if digest != expected_sha:
        raise ValueError("canonical source differs from approved exact SHA-256")
    if canonical == original:
        return digest
    temp_name = None
    try:
        with tempfile.NamedTemporaryFile(mode="wb", prefix=".gk268-", suffix=".tmp",
                                         dir=path.parent, delete=False) as f:
            temp_name = f.name
            f.write(canonical)
            f.flush()
            os.fsync(f.fileno())
        os.replace(temp_name, path)
        temp_name = None
    finally:
        if temp_name and os.path.exists(temp_name):
            os.unlink(temp_name)
    if hashlib.sha256(path.read_bytes()).hexdigest() != expected_sha:
        raise RuntimeError("post-write source digest differs from pinned bytes")
    return digest


def main(argv: list[str]) -> int:
    if len(argv) != 3:
        print("usage: gk268_normalize_pinned_checkout.py SOURCE_PATH EXPECTED_SHA256", file=sys.stderr)
        return 2
    path = Path(argv[1])
    try:
        digest = normalize_pinned(path, argv[2])
    except (OSError, ValueError, RuntimeError) as error:
        print(f"EXEC0268 REJECT {path.name}: {error}", file=sys.stderr)
        return 1
    print(f"EXEC0268 VERIFIED_STATIC canonical Git source {path.name}: {digest}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
