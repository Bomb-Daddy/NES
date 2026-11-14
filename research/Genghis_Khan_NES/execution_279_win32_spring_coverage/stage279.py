#!/usr/bin/env python3
"""EXEC0279: stage the exact Linux-tested EXEC0278 game into the existing Win32 app.

No original ROM, RAM capture, save, images or licensed bytes are read/copied.
This is a mechanical bridge from the currently staged EXEC0277 C11 application.
No test success or gameplay outcome is manufactured by this stage.
"""
import argparse
import hashlib
import pathlib
import shutil
import subprocess
import sys

PRE = {
    "CMakeLists.txt": "ee3ede843168714b52b654f79527079d2997328211ff74fb2e3b59601e72bfbc",
    "gk1_app_state.h": "5198a073a38d5246ae99a86a17bb03b250328e44c06fecfeae712d77c75448ef",
    "gk1_app_state.c": "e7c7e02ed35760234bf7ddc8738f3a0ac87699fb2f1b46ca40f3071df7207372",
    "gk1_native_shell.c": "29fb529883e0fcdf991380c44aebd34dec3dc6a0d77413fad7ced75476c38cf5",
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

def normalized(path):
    content = path.read_bytes()
    # Windows checkout and PowerShell Add-Content may introduce CRLF.
    # Only an LF-normalized SHA-pinned source is allowed to enter the build.
    return content.replace(b'\r\n', b'\n')

def digest(path):
    return hashlib.sha256(normalized(path)).hexdigest()

def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=pathlib.Path, required=True,
                        help="Extracted inherited complete root, containing execution_199")
    parser.add_argument("--review-root", type=pathlib.Path, required=True,
                        help="Public GitHub execution_278 source directory")
    parser.add_argument("--patch", type=pathlib.Path, required=True)
    a = parser.parse_args(argv)
    root = a.source_root.resolve()
    app = root / "execution_199"
    review = a.review_root.resolve()
    patch = a.patch.resolve()
    if not root.is_dir() or not app.is_dir() or not review.is_dir() or not patch.is_file():
        raise RuntimeError("EXEC0279 missing complete source, review directory or patch")
    for name, expected in PRE.items():
        p = app / name
        if not p.is_file() or digest(p) != expected:
            raise RuntimeError("EXEC0279 predecessor bytes changed: " + name + "; got_sha256=" + (digest(p) if p.is_file() else "missing") + "; expected_sha256=" + expected + ("; CMAKE_TAIL=" + repr(normalized(p)[-600:]) if name == "CMakeLists.txt" and p.is_file() else ""))
    for name in ADD:
        dest = app / name
        src = review / name
        if dest.exists() or not src.is_file() or digest(src) != POST[name]:
            raise RuntimeError("EXEC0279 new reviewed source mismatch/duplicate: " + name)
    if digest(patch) != '8f9720f14a30737d37faba3febb726fd6a874b6d67376b05e7fe63ed5f70c966':
        raise RuntimeError('EXEC0279 integration patch digest mismatch')
    # Preflight finished: canonicalize only the four authenticated input files
    # and exact published patch before applying the strict patch gate.
    for name in PRE:
        (app / name).write_bytes(normalized(app / name))
    patch.write_bytes(normalized(patch))
    proc = subprocess.run(["git", "-C", str(root), "apply", "--check", str(patch)],
                          capture_output=True, text=True, check=False)
    if proc.returncode != 0:
        raise RuntimeError("EXEC0279 patch does not match EXEC0277: " + proc.stderr[:700])
    proc = subprocess.run(["git", "-C", str(root), "apply", str(patch)],
                          capture_output=True, text=True, check=False)
    if proc.returncode != 0:
        raise RuntimeError("EXEC0279 patch failed: " + proc.stderr[:700])
    for name in ADD:
        (app / name).write_bytes(normalized(review / name))
    for name, expected in POST.items():
        if digest(app / name) != expected:
            raise RuntimeError("EXEC0279 postimage SHA mismatch: " + name)
    cmake=(app / "CMakeLists.txt").read_text()
    if cmake.count("add_executable(test_gk278_original_spring_coverage ") != 1:
        raise RuntimeError("EXEC0279 no uniquely registered executable test")
    if cmake.count("GK1_278_OriginalSpring24DirectFieldsAtomicAndNoFakeParity") != 1:
        raise RuntimeError("EXEC0279 duplicate/missing CTest")
    if not ("gk278_original_spring_coverage.c" in cmake and
            "gk1_app_original_spring_coverage" in (app / "gk1_native_shell.c").read_text()):
        raise RuntimeError("EXEC0279 production game not actually wired")
    print("VERIFIED_STATIC EXEC0279 PASS: exact EXEC0277 inherited app -> EXEC0278 7-file postimage; CTest and WinMain actually wired")
    return 0

if __name__ == "__main__":
    try: sys.exit(main())
    except Exception as exc:
        print(f"EXEC0279 FAILED: {exc}", file=sys.stderr)
        sys.exit(2)
