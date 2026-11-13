#!/usr/bin/env python3
"""EXEC0262: require real exe-adjacent original-country source in *all* generators.

Never supplies ROM files. The public canonical CSV is hash-pinned and the
application itself independently re-checks it at startup. All negative cases
are isolated in Python's temporary directory and cannot mutate shipped data.
"""
from pathlib import Path
import hashlib
import sys
import tempfile

EXPECTED_SHA = "fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581"
BASENAME = "GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv"
NEEDED = (
    "add_custom_command(TARGET gk1_native_shell POST_BUILD",
    "$<TARGET_FILE_DIR:gk1_native_shell>/" + BASENAME,
    "${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/" + BASENAME,
    "copy_if_different",
    "GK1_262_NativeExecutableSourceCSVLayoutAndNegativeControls",
)


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else ""


def valid_layout(binary: Path, canonical: Path) -> bool:
    if not binary.is_file() or not canonical.is_file():
        return False
    target = binary.parent / BASENAME
    return digest(canonical) == EXPECTED_SHA and digest(target) == EXPECTED_SHA


def valid_cmake(source: str) -> bool:
    return all(part in source for part in NEEDED) and source.count(NEEDED[0]) == 1


def main(argv):
    if len(argv) != 4:
        print("Usage: test_gk262_binary_csv_stage.py NATIVE_EXE CANONICAL_CSV CMakeLists", file=sys.stderr)
        return 2
    binary, canonical, cmake = map(Path, argv[1:])
    if not valid_layout(binary, canonical) or not valid_cmake(cmake.read_text()):
        print("EXEC0262 native exe source CSV stage FAIL", file=sys.stderr)
        return 1
    checks = 2
    source = cmake.read_text()
    for token in NEEDED:
        assert not valid_cmake(source.replace(token, "FORGED")), token
        checks += 1
    with tempfile.TemporaryDirectory(prefix="gk262-public-") as temp:
        folder = Path(temp)
        fake = folder / binary.name
        fake.write_bytes(b"synthetic executable")
        sibling = folder / BASENAME
        assert not valid_layout(fake, canonical)
        checks += 1
        sibling.write_bytes(canonical.read_bytes())
        assert valid_layout(fake, canonical)
        checks += 1
        sibling.write_bytes(sibling.read_bytes() + b"tamper")
        assert not valid_layout(fake, canonical)
        checks += 1
        sibling.unlink()
        assert not valid_layout(fake, canonical)
        checks += 1
        wrong_canonical = folder / "wrong.csv"
        wrong_canonical.write_bytes(b"forged original CSV")
        assert not valid_layout(fake, wrong_canonical)
        checks += 1
    print(f"EXEC0262 real exe-adjacent, SHA-gated 41-country source layout PASS {checks}/{checks}; "
          "no private NES ROM; MSVC/Xbox tested only on actual host")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
