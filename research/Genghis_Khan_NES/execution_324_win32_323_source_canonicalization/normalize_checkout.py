#!/usr/bin/env python3
"""EXEC0324: fail-closed exact LF source verification on Windows autocrlf checkout.

Checks ALL eight published EXEC0323 sources BEFORE changing any file.
The only admitted representation change is CRLF -> LF. No SHA waiver.
Never reads or publishes private NES ROM/RAM assets.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

EXPECTED = {
    "gk323_original_spring.c": "d428b0c0b9e683076a6fd4e39f3bc7893c0919cc694d4c6315187a3305a034b1",
    "gk323_original_spring.h": "fc2074ef04969e2f73cfb45421828844c7e440f0a7a5981aaf831fc04a05c657",
    "gk323_app_stock_bridge.c": "23b4616022c38b1604c31e5c768774ee099780be087bd7e531f5ed0f63c8fb7c",
    "gk323_app_stock_bridge.h": "3f3d5ed0477651e0030b7b81a4169ddb4b6bbecb64d19949b228a8409fa8d6ea",
    "test_gk323_original_spring.c": "256f556d2ca44f1e1d484e86e607e25a0800d8d842364c9202acd7b864cd944c",
    "test_gk323_app_bridge.c": "2a572727ade4e50e1a5e07c9cd67b5843329973e3cc17207d041f2d5072d97df",
    "stage323.py": "ae7d7cc6d26c949310c85c3f5f92f9a40ea9266b5bc9a2d585fa9a98e9d4ea26",
    "test_stage323.py": "8d7499736e6a5399b09b7052f531a2af3ba24f69ae35dec90bb90d1fb4f98102",
}

class Rejected(ValueError):
    pass

def canonical(raw):
    if not raw or b"\x00" in raw:
        raise Rejected("empty source or NUL")
    data = raw.replace(b"\r\n", b"\n")
    if b"\r" in data:
        raise Rejected("not a canonicalizable LF/CRLF source")
    return data

def normalize_verified(root, expected=EXPECTED, inject=None):
    """Verify entire batch first; normalize checked-out CRLF only; rollback on failure."""
    root = Path(root)
    before = {}
    checked = {}
    for name, digest in expected.items():
        if not name or Path(name).name != name:
            raise Rejected("unsafe name")
        original = (root / name).read_bytes()
        normalized = canonical(original)
        if hashlib.sha256(normalized).hexdigest() != digest:
            raise Rejected("exact SHA256 mismatch: " + name)
        before[name] = original
        checked[name] = normalized
    changed = []
    try:
        for name, data in checked.items():
            target = root / name
            if data == before[name]:
                continue
            fd, temp = tempfile.mkstemp(prefix=".gk324-", dir=root)
            try:
                with os.fdopen(fd, "wb") as output:
                    output.write(data)
                    output.flush()
                    os.fsync(output.fileno())
                os.replace(temp, target)
            finally:
                if os.path.exists(temp):
                    os.unlink(temp)
            changed.append(name)
            if inject == "after_first_write":
                raise OSError("intentional rollback test")
        for name, digest in expected.items():
            if hashlib.sha256((root / name).read_bytes()).hexdigest() != digest:
                raise Rejected("post-normalization SHA mismatch: " + name)
    except Exception:
        for name in reversed(changed):
            (root / name).write_bytes(before[name])
        raise
    return len(changed)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: normalize_checkout.py <EXEC0323 source checkout>")
    count = normalize_verified(Path(sys.argv[1]))
    print("GK324 exact SHA256 provenance PASS, CRLF->LF canonicalized", count,
          "files; no NES gameplay, controller or release claim")
