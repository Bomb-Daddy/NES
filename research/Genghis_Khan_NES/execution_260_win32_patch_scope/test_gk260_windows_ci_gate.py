#!/usr/bin/env python3
"""Hostile static verification for the actual EXEC0260 native Win32 workflow."""
import sys
from pathlib import Path
SHA="97421aa2ef45288684bf22d868ad110b850396c316bce9253b0fc8a05166b619"
REQUIRED=["GK1_EXEC0256_Lean_Inherited_Build_Source.zip",SHA,
"test_gk257_win32_acceptance.ps1","test_gk260_windows_ci_gate.py",
"test_gk259_win_user_home.c","GK1_259_Win32LocalAppDataPathAndRefusal",
"cmake -S current-gk1/execution_199","GK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON",
"zlib:x64-windows-static",
"ctest --test-dir build-gk1-win32 -C Release --output-on-failure --timeout 90 -j 4",
"cpack --config build-gk1-win32/CPackConfig.cmake -G NSIS","PHYSICAL_XBOX_NOT_RUN"]
GAME=["--smoke-frames=15","--smoke-gamepad-nav","--smoke-frames=121",
"--smoke-frames=4294967297","WM_PAINT completed","hardware Xbox NOT RUN"]
def audit(w,g):
    return (all(k in w for k in REQUIRED) and all(k in g for k in GAME)
      and g.count("Invoke-NativeExit")>=5
      and "git -C current-gk1 apply " not in w
      and "cmake -S research/Genghis_Khan_NES/execution_228_win32_build_gate/" not in w
      and " -j 4 -R " not in w)
def main(argv):
    if len(argv)!=3:return 2
    w=Path(argv[1]).read_text(encoding="utf-8-sig")
    g=Path(argv[2]).read_text(encoding="utf-8-sig")
    assert audit(w,g),"Live Win32 workflow failed source/static audit"
    n=1
    for original in REQUIRED:
        bad=w.replace(original,"CORRUPTED")
        assert bad!=w and not audit(bad,g),"Hostile workflow mutation passed"
        n+=1
    for original in GAME:
        bad=g.replace(original,"CORRUPTED")
        assert bad!=g and not audit(w,bad),"Hostile smoke mutation passed"
        n+=1
    assert not audit(w+"\ngit -C current-gk1 apply foo",g)
    n+=1
    print(f"EXEC0260 native Win32 CI source gate PASS {n}/{n}, native acceptance separately required")
    return 0
if __name__=="__main__":raise SystemExit(main(sys.argv))
