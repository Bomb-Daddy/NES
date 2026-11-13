#!/usr/bin/env python3
"""Hostile static gate for a tested, expiring, non-release Windows QA installer.

The hosted Win32 runner, not this static script, must prove genuine MSVC,
WinMain controller acceptance, 202+ Win32 CTests and silent NSIS installation.
"""
from pathlib import Path
import sys

NEEDED = (
    "Verify and expand SHA-pinned complete current-game build source",
    "Execute complete Win32 CTest registry, not an old subset",
    "Run real WinMain controller simulation, paint and CLI hostile controls",
    "Build and silently install actual current NSIS package",
    "Verify and manifest QA-only installer payload",
    "Hostile source gate for QA-only artifact publication",
    "Upload tested Windows installer for QA only, not release",
    "actions/upload-artifact@v4",
    "if-no-files-found: error",
    "retention-days: 3",
    "GK1_EXEC0263_WIN64_QA_ONLY_NOT_RELEASE",
    "GK1_EXEC0263_QA_ONLY_manifest.json",
    "PHYSICAL_XBOX_NOT_RUN",
    "FULL_CAMPAIGNS_NOT_RUN",
    "release_accepted = $false",
    "Test-Path -LiteralPath $exe",
    "Get-FileHash -LiteralPath $setup.FullName -Algorithm SHA256",
    "Get-FileHash -LiteralPath $csv -Algorithm SHA256",
    "Get-FileHash -LiteralPath $exe -Algorithm SHA256",
    "Get-FileHash -LiteralPath $buildExe -Algorithm SHA256",
    "throw 'QA installer contains original NES/private research data'",
    "if ($packages.Count -ne 1)",
)
ORDER = (
    "Execute complete Win32 CTest registry, not an old subset",
    "Run real WinMain controller simulation, paint and CLI hostile controls",
    "Build and silently install actual current NSIS package",
    "Verify and manifest QA-only installer payload",
    "Hostile source gate for QA-only artifact publication",
    "Upload tested Windows installer for QA only, not release",
)

def check(source: str) -> bool:
    if any(source.count(token) != 1 for token in ORDER):
        return False
    if not all(token in source for token in NEEDED):
        return False
    if list(map(source.index, ORDER)) != sorted(map(source.index, ORDER)):
        return False
    # An upload with unconditional execution or tolerated failures would
    # publish an untested build, so it must remain forbidden.
    if "continue-on-error: true" in source or "if: always()" in source:
        return False
    return True

def main() -> int:
    if len(sys.argv) != 2:
        return 2
    original = Path(sys.argv[1]).read_text(encoding="utf-8-sig")
    assert check(original), "Untested or unverified installer publication"
    negative = 0
    for token in NEEDED:
        mutation = original.replace(token, "CORRUPTED", 1)
        assert mutation != original and not check(mutation), f"Negative control accepted: {token}"
        negative += 1
    for token in ORDER:
        mutation = original.replace(token, "CORRUPTED", 1)
        assert not check(mutation), f"Order mutation accepted: {token}"
        negative += 1
    assert not check(original + "\ncontinue-on-error: true\n")
    assert not check(original + "\nif: always()\n")
    negative += 2
    print(f"EXEC0263 artifact gate PASS: {negative}/{negative} deliberate corruptions rejected; hosted Win32 still required")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
