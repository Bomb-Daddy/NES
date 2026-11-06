#!/usr/bin/env python3
"""EXEC0039 optional private modified-ROM native-reference *replay integration*.
No copyrighted bytes embedded, printed, or published. Reads archived native
cases from a private ZIP only when explicitly supplied, verifies archive and
member hashes, runs the NEW bounded Linux executable and checks numeric
postmove outcome. Does not run NES, prove general mechanics, or do Windows QA.
"""
import hashlib
import json
import pathlib
import re
import subprocess
import sys
import zipfile

EXPECTED_ZIP_SHA256 = "aec170e8661a0011e373024caeb858f2657b716574b5f8555b63703e5cfd28f1"
EXPECTED_ROM_SHA256 = "4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5"
NAMES = {"friendly_occupied": "friendly", "enemy_occupied": "enemy", "insufficient_mobility": "low", "exact_mobility": "exact"}


def audit(archive: pathlib.Path, executable: pathlib.Path) -> int:
    raw = archive.read_bytes()
    if hashlib.sha256(raw).hexdigest() != EXPECTED_ZIP_SHA256:
        raise ValueError("private archive hash mismatch; refusing evidence")
    checks = 0
    with zipfile.ZipFile(archive) as z:
        if z.testzip() is not None:
            raise ValueError("ZIP CRC failed")
        for name, command in NAMES.items():
            prefix = f"private/{name}/after_attempt"
            report = json.loads(z.read(prefix + ".json"))
            sram = z.read(prefix + ".sram")
            zp = z.read(prefix + ".zp")
            if len(sram) != 8192 or len(zp) != 256:
                raise ValueError(f"{name}: capture length not native")
            for suffix, content in (("sram_sha256", sram), ("zp_sha256", zp)):
                if hashlib.sha256(content).hexdigest() != report[suffix]:
                    raise ValueError(f"{name}: archived {suffix} mismatch")
                checks += 1
            if report["rom_sha256"] != EXPECTED_ROM_SHA256:
                raise ValueError(f"{name}: wrong ROM identity")
            checks += 1
            proc = subprocess.run([str(executable), command], text=True,
                capture_output=True, check=True, timeout=20)
            line = proc.stdout.splitlines()[0]
            match = re.fullmatch(r"ATTEMPT result=(-?\d+) soldiers=(\d+) partner=(\d+) mobility=(\d+) word=(\d+) rng=([0-9a-f]{12})", line)
            if not match:
                raise ValueError(f"{name}: unrecognized running executable output")
            outcome, soldiers, partner, mobility, word, rng = match.groups()
            # The old source remains the oracle; the new running port is the
            # *test subject*. This is not a new observation of the NES.
            actual = [int(soldiers), int(partner), int(mobility), int(word), rng]
            expected = [report["unit0_soldiers"], report["unit1_soldiers"],
                        report["unit0_mobility"], report["completion_word"], report["rng"]]
            if actual != expected:
                raise AssertionError(f"{name}: executable vs archived numeric mismatch {actual!r}!={expected!r}")
            checks += len(actual)
            if int(outcome) != (2 if name in ("friendly_occupied", "exact_mobility") else 0):
                raise AssertionError(f"{name}: result class mismatch")
            checks += 1
            print(f"{name}: PASS; native archived outcome matched running C11 replay")
    print(f"PASS {checks} numeric/provenance checks; 4 old approved-modified-ROM cases; no NES or Windows runtime invoked")
    return checks


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("usage: python3 audit_private_replay.py /PRIVATE/EXEC0033.zip ./build039/gk1_battle_replay")
    try:
        audit(pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]).resolve())
    except (ValueError, AssertionError, KeyError, OSError, zipfile.BadZipFile, subprocess.CalledProcessError) as err:
        sys.exit(str(err))
