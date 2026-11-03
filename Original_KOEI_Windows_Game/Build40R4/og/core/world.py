"""Authoritative WorldState (plain dict), validation, canonical bytes, hash."""
import hashlib
import json

from .fixed import CoreError, INT_MAX, check_int, check_tree
from .rng import MASK

SCHEMA_VERSION = 1
MAX_BP = 200
CULT_MAX = 100
STAT_MAX = 100
XP_MAX = 1000000
TOP_KEYS = {"schema_version", "revision", "rng_state", "processed_ids", "journal", "provinces", "officers"}
OFFICER_KEYS = {"province_id", "politics", "war", "leadership", "intelligence", "charisma", "xp", "bp"}
STATS = ("politics", "war", "leadership", "intelligence", "charisma")


def new_world(seed):
    check_int(seed, 0, MASK, "seed")
    return {"schema_version": SCHEMA_VERSION, "revision": 0, "rng_state": seed,
            "processed_ids": [], "journal": [], "provinces": {}, "officers": {}}


def add_province(data, pid, cultivation):
    data["provinces"][pid] = {"cultivation": cultivation}


def add_officer(data, oid, province_id, stats, xp=0, bp=None):
    """New officers start with BP equal to Politics, unless explicitly supplied for a save/test fixture."""
    if bp is None:
        bp = stats[0]
    o = {"province_id": province_id, "xp": xp, "bp": bp}
    for name, v in zip(STATS, stats):
        o[name] = v
    data["officers"][oid] = o


def validate_world(d):
    if not isinstance(d, dict):
        raise CoreError("BAD_SHAPE", "world must be an object")
    check_tree(d)
    if d.get("schema_version") == 2:
        from . import world2
        return world2.validate_v2(d)
    if set(d) != (TOP_KEYS | ({'balance_policy'} if d.get('balance_policy') in ('OG_1000_SLOW_GROWTH_V1_CANDIDATE','OG_1000_DIRECTOR_V2') else set())):
        raise CoreError("BAD_SHAPE", "top-level keys")
    if d["schema_version"] != SCHEMA_VERSION:
        raise CoreError("BAD_VERSION", "world schema_version")
    check_int(d["revision"], 0, INT_MAX, "revision")
    check_int(d["rng_state"], 0, MASK, "rng_state")
    ids = d["processed_ids"]
    if not isinstance(ids, list) or len(set(ids)) != len(ids) or any(not isinstance(i, str) or not i for i in ids):
        raise CoreError("BAD_SHAPE", "processed_ids")
    j = d["journal"]
    if not isinstance(j, list):
        raise CoreError("BAD_SHAPE", "journal")
    for idx, ev in enumerate(j):
        if not isinstance(ev, dict) or set(ev) != {"seq", "id", "kind", "revision"} or ev["seq"] != idx + 1:
            raise CoreError("BAD_SHAPE", "journal event " + str(idx))
    if d["revision"] != len(j) or len(ids) != len(j):
        raise CoreError("BAD_SHAPE", "revision/journal/processed_ids disagree")
    for pid, p in d["provinces"].items():
        if not isinstance(p, dict) or set(p) != {"cultivation"}:
            raise CoreError("BAD_SHAPE", "province " + pid)
        check_int(p["cultivation"], 0, 1000 if d.get("balance_policy") in ("OG_1000_SLOW_GROWTH_V1_CANDIDATE","OG_1000_DIRECTOR_V2") else CULT_MAX, "cultivation")
    for oid, o in d["officers"].items():
        if not isinstance(o, dict) or set(o) != OFFICER_KEYS or not isinstance(o["province_id"], str):
            raise CoreError("BAD_SHAPE", "officer " + oid)
        for s in STATS:
            check_int(o[s], 0, STAT_MAX, s)
        check_int(o["xp"], 0, XP_MAX, "xp")
        check_int(o["bp"], 0, MAX_BP, "bp")


def canonical_bytes(obj):
    return json.dumps(obj, sort_keys=True, separators=(",", ":"), ensure_ascii=True).encode("ascii")


def world_hash(data):
    return hashlib.sha256(canonical_bytes(data)).hexdigest()
