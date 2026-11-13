#!/usr/bin/env python3
"""EXEC0261 — test the *generated native Windows CTest registry*, not an invented one.

The authentic WinMain Win32 host deliberately does not execute Linux X11-only
CLI smoke commands, POSIX/Xvfb Mesen capture, or a Linux desktop/.deb assertion.
All game/finance/negative tests that are portable remain in the full registry.
"""
from pathlib import Path
import re
import sys

PLATFORM_INCOMPATIBLE = (
    "GK1_071_NativeControllerVIEWASafeGateAndBothWorldsImmutable",
    "GK1_073_MergedControllerBudgetAndBattlePassPreSetupWorldImmutable",
    "GK1_074_MergedSpringControllerNoWorldMutation",
    "GK1_075_ActualNativeControllerSpringPreviewNoMutation",
    "GK1_176_LauncherAndDualPackagingVersionContract",
    "GK1_221_PrivateStockMesenCaptureRunnerAdversarialHarness",
    "GK1_250_OriginalThreeOrderFullColdSmoke_FailClosed",
)
REQUIRED_NATIVE = (
    "GK1_259_Win32LocalAppDataPathAndRefusal",
    "GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass",
    "GK1_256_ClassicViewFirstPass_SourceImmutable",
    "GK1_249_FullClassicColdSaveHandleGate",
    "GK1_246_SourceBoundClassicTaxReceipt_AtomicPublicNegative",
)
def audit(source):
    names = re.findall(r'add_test\(\[?=?\[?([^\] \r\n)]+)', source)
    # CMake generates add_test([=[NAME]=]...) with bracket quoting on modern versions.
    if not names:
        names = re.findall(r'add_test\(\[?=?\[?([^\] \r\n)]+)', source, re.I)
    return (
        len(names) >= 195 and len(names) == len(set(names))
        and all(any(n in name for name in names) for n in REQUIRED_NATIVE)
        and all(not any(n in name for name in names) for n in PLATFORM_INCOMPATIBLE)
    ), names

def main(argv):
    if len(argv) != 2:
        print("Usage: test_gk261_win32_registry.py WINDOWS_BUILD_ROOT/CTestTestfile.cmake", file=sys.stderr)
        return 2
    s = Path(argv[1]).read_text(encoding="utf-8-sig")
    ok, names = audit(s)
    if not ok:
        print(f"EXEC0261 Windows CTest registration FAILED: found {len(names)} tests", file=sys.stderr)
        for n in REQUIRED_NATIVE:
            if not any(n in z for z in names): print(f"missing required: {n}", file=sys.stderr)
        for n in PLATFORM_INCOMPATIBLE:
            if any(n in z for z in names): print(f"host-incompatible still registered: {n}", file=sys.stderr)
        return 1
    for n in REQUIRED_NATIVE:
        assert not audit(s.replace(n, "MUTATED_OR_MISSING_TEST"))[0], f"Mutation escaped {n}"
    for n in PLATFORM_INCOMPATIBLE:
        # Deliberately register a fake malformed host-incompatible case: audit must reject.
        assert not audit(s + f'\nadd_test([=[{n}]=] [=[bad]=])')[0], f"Excluded case escaped {n}"
    print(f"EXEC0261 Win32 generated CTest registry PASS: {len(names)} real native tests; {len(REQUIRED_NATIVE)+len(PLATFORM_INCOMPATIBLE)} hostile controls")
    print("7 Linux-specific tests excluded only from Win32; full native Windows registry still executes.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
