#!/usr/bin/env python3
"""Hostile Win32 smoke preflight, runnable on both Linux and Windows.
Checks the same compiled native smoke executable against canonical and tampered
original-source CSV. No ROM bytes, no physical Xbox/stock Mesen claims.
"""
import pathlib
import shutil
import subprocess
import sys
import tempfile


def main() -> int:
    if len(sys.argv) != 3:
        raise SystemExit("usage: test_gk228_windows_contract.py BINARY CSV")
    binary = pathlib.Path(sys.argv[1]).resolve()
    csv = pathlib.Path(sys.argv[2]).resolve()
    assert binary.is_file() and csv.is_file(), "missing native binary/source"
    with tempfile.TemporaryDirectory(prefix="gk228_") as temp:
        p = pathlib.Path(temp)
        valid = p / "verified.csv"
        shutil.copyfile(csv, valid)
        result = subprocess.run([str(binary), str(valid)], capture_output=True,
                                text=True, timeout=45)
        assert result.returncode == 0, (result.returncode, result.stderr)
        assert "PASS" in result.stdout and "stock_first_Spring=OPEN" in result.stdout
        assert "physical_Xbox=NOT_RUN" in result.stdout
        if sys.platform == "win32":
            assert "NATIVE_WIN32_BUILD_EXECUTED" in result.stdout
        else:
            assert "LINUX_PORT_NEGATIVE_CONTROL" in result.stdout
        altered = p / "altered.csv"
        b = bytearray(csv.read_bytes())
        mid = len(b) // 2
        b[mid] ^= 1
        altered.write_bytes(b)
        short = p / "short.csv"
        short.write_bytes(b[:mid])
        for bad in (altered, short, p / "absent.csv"):
            rejected = subprocess.run([str(binary), str(bad)], capture_output=True,
                                      text=True, timeout=45)
            assert rejected.returncode != 0, f"accepted invalid source: {bad}"
        assert valid.read_bytes() == csv.read_bytes(), "canonical source mutated"
        print("EXEC0228 PASS: native source/campaign/Win32 probe and 3 invalid source controls")
    return 0


if __name__ == "__main__":
    sys.exit(main())
