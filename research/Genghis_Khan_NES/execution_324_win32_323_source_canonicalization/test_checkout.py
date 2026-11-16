#!/usr/bin/env python3
"""EXEC0324 hostile checkout tests; no licensed gameplay bytes."""
import hashlib
from pathlib import Path
from tempfile import TemporaryDirectory
from normalize_checkout import normalize_verified, canonical, Rejected, EXPECTED

N = 0
def ok(value):
    global N
    N += 1
    if not value:
        raise AssertionError("GK324 assertion " + str(N))

def stage(root, data):
    root.mkdir()
    return {name: (root / name).write_bytes(body) for name, body in data.items()}

def digests(data):
    return {name: hashlib.sha256(body).hexdigest() for name, body in data.items()}

def denied(fn):
    try:
        fn()
    except (Rejected, OSError, FileNotFoundError):
        ok(True)
    else:
        raise AssertionError("unsafe case accepted")

ok(len(EXPECTED) == 8)
ok(all(len(x) == 64 for x in EXPECTED.values()))
ok(canonical(b"abc\r\ndef\n") == b"abc\ndef\n")
for bad in (b"", b"a\x00b", b"a\rb", b"a\r\r\nb"):
    denied(lambda b=bad: canonical(b))
with TemporaryDirectory() as d:
    root = Path(d)
    lf = {"a.c": b"a\nb\n", "b.py": b"print(1)\n", "c.h": b"header\n"}
    target = root / "a"
    stage(target, {"a.c": b"a\r\nb\r\n", "b.py": lf["b.py"], "c.h": b"header\r\n"})
    good = digests(lf)
    ok(normalize_verified(target, good) == 2)
    ok(all((target / name).read_bytes() == data for name, data in lf.items()))
    ok(normalize_verified(target, good) == 0)
    (target / "a.c").write_bytes(b"forged\r\n")
    before = {name: (target / name).read_bytes() for name in lf}
    denied(lambda: normalize_verified(target, good))
    ok(all((target / name).read_bytes() == before[name] for name in lf))
    (target / "a.c").write_bytes(b"a\r\nb\r\n")
    (target / "c.h").write_bytes(b"header\r\n")
    before = {name: (target / name).read_bytes() for name in lf}
    denied(lambda: normalize_verified(target, good, "after_first_write"))
    ok(all((target / name).read_bytes() == before[name] for name in lf))
    (target / "c.h").write_bytes(b"header\r\r\n")
    before = {name: (target / name).read_bytes() for name in lf}
    denied(lambda: normalize_verified(target, good))
    ok(all((target / name).read_bytes() == before[name] for name in lf))
    (target / "c.h").unlink()
    denied(lambda: normalize_verified(target, good))
    denied(lambda: normalize_verified(target, {"../escape": "0" * 64}))
print("GK324 hostile Windows source canonicalization", N, "assertions PASS")
