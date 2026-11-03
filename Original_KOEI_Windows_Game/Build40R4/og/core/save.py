"""Canonical versioned save: sorted-key JSON, embedded SHA-256, tmp+replace, migration registry."""
import hashlib
import json
import os

from .fixed import CoreError
from .world import SCHEMA_VERSION, canonical_bytes, validate_world
from .world2 import upgrade_legacy_v2

# v2 entry = explicit backward read: a pre-Q v2 shape is upgraded to the current v2 shape, any other v2 passes through
# unchanged and is judged by validate_world.
MIGRATIONS = {1: lambda w: w, 2: upgrade_legacy_v2}


def _pairs(pairs):
    d = {}
    for k, v in pairs:
        if k in d:
            raise CoreError("DUP_KEY", k)
        d[k] = v
    return d


def _no_float(s):
    raise CoreError("FLOAT", s)


def _no_const(s):
    raise CoreError("BAD_CONST", s)


def dump_save(data):
    validate_world(data)
    sha = hashlib.sha256(canonical_bytes(data)).hexdigest()
    return canonical_bytes({"schema_version": data["schema_version"], "sha256": sha, "world": data})


def load_save(raw):
    try:
        doc = json.loads(raw.decode("utf-8"), object_pairs_hook=_pairs, parse_float=_no_float, parse_constant=_no_const)
    except (ValueError, UnicodeDecodeError) as e:
        raise CoreError("BAD_SAVE", str(e)[:80])
    if not isinstance(doc, dict) or set(doc) != {"schema_version", "sha256", "world"}:
        raise CoreError("BAD_SAVE", "document keys")
    ver = doc["schema_version"]
    if isinstance(ver, bool) or not isinstance(ver, int) or ver not in MIGRATIONS:
        raise CoreError("UNKNOWN_SCHEMA", str(ver))
    if hashlib.sha256(canonical_bytes(doc["world"])).hexdigest() != doc["sha256"]:
        raise CoreError("HASH_MISMATCH", "stored sha256 does not match world")
    world = MIGRATIONS[ver](doc["world"])
    if not isinstance(world, dict) or world.get("schema_version") != ver:
        raise CoreError("BAD_SAVE", "envelope schema_version does not match world")
    validate_world(world)
    return world


def write_save(path, data):
    raw = dump_save(data)
    tmp = path + ".tmp"
    with open(tmp, "wb") as f:
        f.write(raw)
        f.flush()
        os.fsync(f.fileno())
    os.replace(tmp, path)


def read_save(path):
    with open(path, "rb") as f:
        return load_save(f.read())
