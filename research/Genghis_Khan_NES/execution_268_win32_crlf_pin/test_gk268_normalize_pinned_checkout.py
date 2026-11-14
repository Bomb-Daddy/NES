#!/usr/bin/env python3
"""EXEC0268: real pinned source, CRLF checkout, and hostile corruption checks."""
import hashlib
from pathlib import Path
import subprocess
import sys
import tempfile

from gk268_normalize_pinned_checkout import PINS


def run(script: Path, p: Path, sha: str, ok: bool) -> None:
    proc = subprocess.run([sys.executable, str(script), str(p), sha],
                          capture_output=True, text=True, timeout=8)
    if (proc.returncode == 0) != ok:
        raise AssertionError(f"result {proc.returncode} but ok={ok}: {proc.stdout} {proc.stderr}")


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("usage: test_gk268_normalize_pinned_checkout.py SCRIPT CANONICAL_SOURCE_FOLDER")
    script, source = Path(sys.argv[1]), Path(sys.argv[2])
    count = 0
    with tempfile.TemporaryDirectory(prefix="gk268-qa-") as d:
        folder = Path(d)
        for name, sha in PINS.items():
            canonical = (source / name).read_bytes()
            assert b"\r" not in canonical and b"\n" in canonical
            assert hashlib.sha256(canonical).hexdigest() == sha
            crlf = canonical.replace(b"\n", b"\r\n")
            assert crlf != canonical
            p = folder / name
            for raw, should_accept in [
                (canonical, True),
                (crlf, True),
                (canonical.replace(b"\n", b"\r\n", 1), True),
                (canonical + b"\n# tampered", False),
                (canonical.replace(b"\n", b"\r", 1), False),
                (crlf + b"\x00", False),
                (canonical[:-1], False),
                (canonical + b"\r", False),
            ]:
                p.write_bytes(raw)
                run(script, p, sha, should_accept)
                assert p.read_bytes() == (canonical if should_accept else raw), name
                count += 1
            p.write_bytes(canonical)
            run(script, p, "0" * 64, False)
            assert p.read_bytes() == canonical
            count += 1
            run(script, p, "nonsense", False)
            assert p.read_bytes() == canonical
            count += 1
        missing = folder / "test_gk266_controller_chord.c"
        missing.unlink()
        run(script, missing, PINS[missing.name], False)
        count += 1
        badname = folder / "unapproved.c"
        badname.write_bytes((source / "gk1_pad_router.c").read_bytes())
        run(script, badname, PINS["gk1_pad_router.c"], False)
        count += 1
    print(f"EXEC0268 PASS {count}/{count} real-source/CRLF/hostile controls; no Win32 or Xbox claim")


if __name__ == "__main__":
    main()
