#!/usr/bin/env python3
"""EXEC0279: exact source-guarded Win32 staging of reviewed EXEC0278 native C11.

Accept only the Linux EXEC0277 baseline or the observed exact Windows
EXEC0277 workflow CMake preimage. Windows' earlier CMake staging has different
historical text from the archived Linux CMake; preserve it without weakening
full-file provenance checks. Never import ROMs, private RAM or game assets.
"""
import argparse
import hashlib
import pathlib
import subprocess
import sys

PRE = {
    "CMakeLists.txt": {
        "ee3ede843168714b52b654f79527079d2997328211ff74fb2e3b59601e72bfbc",
        "2bb214155f8450e2c20532c0642819d015f8c913c930a1bcdc7b0c4ac196a25b",
    },
    "gk1_app_state.h": {"5198a073a38d5246ae99a86a17bb03b250328e44c06fecfeae712d77c75448ef"},
    "gk1_app_state.c": {"e7c7e02ed35760234bf7ddc8738f3a0ac87699fb2f1b46ca40f3071df7207372"},
    "gk1_native_shell.c": {"29fb529883e0fcdf991380c44aebd34dec3dc6a0d77413fad7ced75476c38cf5"},
}
POST = {
    "CMakeLists.txt": "9de41a6bb9f39d9f998f259e82ce22ecbd981d24e75134d99e848334123bab8c",
    "gk1_app_state.h": "4b88bb9d8cde5358189688793ad080105d9b576227cb3778d41ae93ba7da8645",
    "gk1_app_state.c": "058b27ff7608b8d799568cce503023736e815820003a6a19cb36dc9c026b1cb9",
    "gk1_native_shell.c": "ee3a8ed192670379f812554de0c3e5ba29311f3084a4336db8fa985be6e84df5",
    "gk278_original_spring_coverage.c": "585e71a68d9ed55e6d472990d4abd2babfa5b7fadd242269996b7f35f35dbbf6",
    "gk278_original_spring_coverage.h": "526fef9de109ebb6d940afdc620c8ad695480a5202d1393e0cc716a513c3bd4e",
    "test_gk278_original_spring_coverage.c": "4d2c6bf75a2f2f340ecaacba0b4c06b35773911bbe0bb3f843ef8a015a703296",
}
ADD = set(POST) - set(PRE)
PATCH_SHA = "8f9720f14a30737d37faba3febb726fd6a874b6d67376b05e7fe63ed5f70c966"
LIBRARY_ANCHOR = b"gk215_original_spring_gap.c gk216_fresh_cpu_pc.c"
LIBRARY_REPLACEMENT = b"gk215_original_spring_gap.c gk278_original_spring_coverage.c gk216_fresh_cpu_pc.c"
TEST_BLOCK = b"""\n# EXEC0278: authenticated first-Spring 24-field coverage matrix.
# No ROM/raw original input in public CI; private 3-history QA run separately.
add_executable(test_gk278_original_spring_coverage test_gk278_original_spring_coverage.c)
target_link_libraries(test_gk278_original_spring_coverage PRIVATE gk1_app_state)
add_test(NAME GK1_278_OriginalSpring24DirectFieldsAtomicAndNoFakeParity
 COMMAND test_gk278_original_spring_coverage
 ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
"""

def normalized(path):
    return path.read_bytes().replace(b"\r\n", b"\n")

def checksum(blob):
    return hashlib.sha256(blob).hexdigest()

def digest(path):
    return checksum(normalized(path))

def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=pathlib.Path, required=True)
    parser.add_argument("--review-root", type=pathlib.Path, required=True)
    parser.add_argument("--patch", type=pathlib.Path, required=True)
    a = parser.parse_args(argv)
    root = a.source_root.resolve()
    app = root / "execution_199"
    review = a.review_root.resolve()
    patch = a.patch.resolve()
    if not root.is_dir() or not app.is_dir() or not review.is_dir() or not patch.is_file():
        raise RuntimeError("EXEC0279 missing full inherited source, review or patch")
    before = {}
    for name, allowed in PRE.items():
        p = app / name
        if not p.is_file():
            raise RuntimeError("EXEC0279 missing predecessor " + name)
        raw = normalized(p)
        before[name] = raw
        if checksum(raw) not in allowed:
            raise RuntimeError("EXEC0279 predecessor bytes changed: " + name + "; sha=" + checksum(raw))
    for name in ADD:
        dest = app / name
        src = review / name
        if dest.exists() or not src.is_file() or digest(src) != POST[name]:
            raise RuntimeError("EXEC0279 new reviewed source mismatch/duplicate: " + name)
    patch_data = normalized(patch)
    if checksum(patch_data) != PATCH_SHA:
        raise RuntimeError("EXEC0279 reviewed integration patch digest mismatch")
    begin = patch_data.index(b"--- a/execution_199/gk1_app_state.h\n")
    end = patch_data.index(b"--- a/execution_199/gk278_original_spring_coverage.h\n")
    production_patch = patch_data[begin:end]
    if production_patch.count(b"--- a/execution_199/") != 3:
        raise RuntimeError("EXEC0279 reviewed production C11 patch changed")
    cmake = before["CMakeLists.txt"]
    if cmake.count(LIBRARY_ANCHOR) != 1:
        raise RuntimeError("EXEC0279 CMake native game target anchor invalid")
    if b"GK1_278_OriginalSpring24DirectFieldsAtomicAndNoFakeParity" in cmake:
        raise RuntimeError("EXEC0279 duplicate CTest")
    if not cmake.endswith(b"\n") or cmake.count(b"GK1_277_FreshSourceRequiresUntouchedTitleOrKnownWizardChoices") != 1:
        raise RuntimeError("EXEC0279 inherited controller/source CTest registry missing")
    revised_cmake = cmake.replace(LIBRARY_ANCHOR, LIBRARY_REPLACEMENT) + TEST_BLOCK
    if checksum(cmake) == "ee3ede843168714b52b654f79527079d2997328211ff74fb2e3b59601e72bfbc" and checksum(revised_cmake) != POST["CMakeLists.txt"]:
        raise RuntimeError("EXEC0279 Linux CMake postimage not exact reviewed source")
    for name in PRE:
        (app / name).write_bytes(before[name])
    checked = subprocess.run(["git", "-C", str(root), "apply", "--check", "-"],
                             input=production_patch, capture_output=True)
    if checked.returncode != 0:
        raise RuntimeError("EXEC0279 production source patch preflight refused: " + checked.stderr.decode(errors='replace')[:800])
    applied = subprocess.run(["git", "-C", str(root), "apply", "-"],
                             input=production_patch, capture_output=True)
    if applied.returncode != 0:
        raise RuntimeError("EXEC0279 production source patch failed: " + applied.stderr.decode(errors='replace')[:800])
    # Git-for-Windows may convert git-applied text back to CRLF. Digesting
    # a normalized view is insufficient: the compiled on-disk source must
    # itself be byte-identical to the independently tested Linux C11 source.
    for name in ("gk1_app_state.c", "gk1_app_state.h", "gk1_native_shell.c"):
        target = app / name
        target.write_bytes(normalized(target))
        if checksum(target.read_bytes()) != POST[name]:
            raise RuntimeError("EXEC0279 patched on-disk C11 SHA mismatch: " + name)
    (app / "CMakeLists.txt").write_bytes(revised_cmake)
    for name in ADD:
        (app / name).write_bytes(normalized(review / name))
    for name, expected in POST.items():
        if name != "CMakeLists.txt" and checksum((app / name).read_bytes()) != expected:
            raise RuntimeError("EXEC0279 verified production postimage SHA mismatch: " + name)
    if normalized(app / "CMakeLists.txt") != revised_cmake:
        raise RuntimeError("EXEC0279 CMake source changed beyond two reviewed additions")
    if (revised_cmake.count(LIBRARY_REPLACEMENT) != 1 or
        revised_cmake.count(b"add_executable(test_gk278_original_spring_coverage test_gk278_original_spring_coverage.c)") != 1 or
        revised_cmake.count(b"GK1_278_OriginalSpring24DirectFieldsAtomicAndNoFakeParity") != 1 or
        b"gk1_app_original_spring_coverage" not in normalized(app / "gk1_native_shell.c")):
        raise RuntimeError("EXEC0279 did not link actual C11 WinMain and CTest")
    print("VERIFIED_STATIC EXEC0279 PASS: full inherited C11+3 authentic reviewed production patches; native WinMain and registered CTest; CMake_SHA256="+checksum(revised_cmake))
    return 0

if __name__ == "__main__":
    try: sys.exit(main())
    except Exception as exc:
        print("EXEC0279 FAILED:", exc, file=sys.stderr)
        sys.exit(2)
