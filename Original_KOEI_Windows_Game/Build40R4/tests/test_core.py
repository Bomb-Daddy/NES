import copy
import hashlib
import json
import os
import tempfile
import unittest

from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.rng import next_u64, randrange
from og.core.save import dump_save, load_save, read_save, write_save
from og.core.world import add_officer, add_province, canonical_bytes, new_world, validate_world, world_hash


def h_roll(d, actor, p):
    return {"roll": randrange(d, p["n"])}


def h_boom(d, actor, p):
    d["provinces"]["P1"]["cultivation"] = 99
    d["officers"][actor]["bp"] = 0
    raise CoreError("BOOM", "after mutation")


def make_world(seed=12345):
    w = new_world(seed)
    add_province(w, "P1", 10)
    add_province(w, "P2", 10)
    add_officer(w, "O1", "P1", (50, 50, 50, 50, 50))
    return w


def make_bus(seed=12345):
    return Bus(make_world(seed), {"test.roll": h_roll, "test.boom": h_boom})


def cmd(rid, rev, kind="cultivate", params=None, actor="O1"):
    return {"request_id": rid, "actor_officer_id": actor, "expected_world_revision": rev,
            "kind": kind, "params": params if params is not None else {"province_id": "P1"}}


def code_of(fn):
    try:
        fn()
    except CoreError as e:
        return e.code
    return None


class RngTests(unittest.TestCase):
    def test_splitmix64_golden_seed0(self):
        d = {"rng_state": 0}
        self.assertEqual(next_u64(d), 0xE220A8397B1DCDAF)

    def test_deterministic_and_seed_sensitive(self):
        a, b, c = {"rng_state": 7}, {"rng_state": 7}, {"rng_state": 8}
        sa = [next_u64(a) for _ in range(5)]
        self.assertEqual(sa, [next_u64(b) for _ in range(5)])
        self.assertNotEqual(sa, [next_u64(c) for _ in range(5)])

    def test_randrange_bounds_and_arg_check(self):
        d = {"rng_state": 1}
        self.assertTrue(all(0 <= randrange(d, 6) < 6 for _ in range(200)))
        self.assertEqual(code_of(lambda: randrange(d, 0)), "OUT_OF_RANGE")
        self.assertEqual(code_of(lambda: randrange(d, 2.0)), "NOT_INT")


class PreviewTests(unittest.TestCase):
    def test_preview_mutates_nothing_including_rng(self):
        bus = make_bus()
        before = world_hash(bus.data)
        before_rng = bus.data["rng_state"]
        q = bus.preview(cmd("a", 0, "test.roll", {"n": 6}))
        self.assertEqual(world_hash(bus.data), before)
        self.assertEqual(bus.data["rng_state"], before_rng)
        q2 = bus.preview(cmd("a", 0, "cultivate"))
        self.assertEqual(world_hash(bus.data), before)
        self.assertEqual(q["result"]["roll"], bus.preview(cmd("a", 0, "test.roll", {"n": 6}))["result"]["roll"])
        self.assertEqual(q2["result"]["cultivation_after"], 11)

    def test_commit_advances_rng_and_matches_preview(self):
        bus = make_bus()
        q = bus.preview(cmd("a", 0, "test.roll", {"n": 1000}))
        r = bus.commit(cmd("a", 0, "test.roll", {"n": 1000}), q["quote_id"])
        self.assertEqual(r["result"], q["result"])
        self.assertNotEqual(bus.data["rng_state"], 12345)

    def test_same_seed_same_world_after_same_commands(self):
        a, b = make_bus(5), make_bus(5)
        for bus in (a, b):
            bus.commit(cmd("a", 0, "test.roll", {"n": 100}))
            bus.commit(cmd("b", 1, "cultivate"))
        self.assertEqual(world_hash(a.data), world_hash(b.data))
        c = make_bus(6)
        c.commit(cmd("a", 0, "test.roll", {"n": 100}))
        c.commit(cmd("b", 1, "cultivate"))
        self.assertNotEqual(world_hash(a.data), world_hash(c.data))

    def test_stale_quote_rejected(self):
        bus = make_bus()
        q = bus.preview(cmd("a", 0, "test.roll", {"n": 6}))
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 0, "test.roll", {"n": 7}), q["quote_id"])), "STALE_QUOTE")
        self.assertEqual(bus.data["revision"], 0)


class EnvelopeTests(unittest.TestCase):
    def test_duplicate_id_rejected_commit_and_preview(self):
        bus = make_bus()
        bus.commit(cmd("a", 0))
        h = world_hash(bus.data)
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 1))), "DUPLICATE_ID")
        self.assertEqual(code_of(lambda: bus.preview(cmd("a", 1))), "DUPLICATE_ID")
        self.assertEqual(world_hash(bus.data), h)

    def test_stale_revision_rejected(self):
        bus = make_bus()
        bus.commit(cmd("a", 0))
        h = world_hash(bus.data)
        self.assertEqual(code_of(lambda: bus.commit(cmd("b", 0))), "STALE_REVISION")
        self.assertEqual(code_of(lambda: bus.commit(cmd("b", 5))), "STALE_REVISION")
        self.assertEqual(world_hash(bus.data), h)

    def test_float_bool_and_out_of_range_rejected(self):
        bus = make_bus()
        h = world_hash(bus.data)
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 0.0))), "NOT_INT")
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", True))), "NOT_INT")
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 0, "test.roll", {"n": 1.5}))), "NOT_INT")
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 2 ** 70))), "OUT_OF_RANGE")
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", -1))), "OUT_OF_RANGE")
        self.assertEqual(world_hash(bus.data), h)

    def test_bad_envelopes(self):
        bus = make_bus()
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 0, "nope"))), "UNKNOWN_KIND")
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 0, actor="ghost"))), "UNKNOWN_ACTOR")
        extra = cmd("a", 0)
        extra["bonus"] = 1
        self.assertEqual(code_of(lambda: bus.commit(extra)), "BAD_ENVELOPE")
        self.assertEqual(code_of(lambda: bus.commit(cmd("", 0))), "BAD_ENVELOPE")
        self.assertEqual(code_of(lambda: bus.commit([1])), "BAD_ENVELOPE")

    def test_atomic_rollback_on_handler_failure(self):
        bus = make_bus()
        h = world_hash(bus.data)
        self.assertEqual(code_of(lambda: bus.commit(cmd("a", 0, "test.boom"))), "BOOM")
        self.assertEqual(world_hash(bus.data), h)
        self.assertNotIn("a", bus.data["processed_ids"])
        bus.commit(cmd("a", 0))
        self.assertEqual(bus.data["revision"], 1)


class CultivateTests(unittest.TestCase):
    def test_commit_effects_and_journal(self):
        bus = make_bus()
        r = bus.commit(cmd("r1", 0))
        d = bus.data
        self.assertEqual(r["world_revision"], 1)
        self.assertEqual(d["provinces"]["P1"]["cultivation"], 11)
        self.assertEqual(d["officers"]["O1"]["bp"], 48)
        self.assertEqual(d["officers"]["O1"]["xp"], 1)
        self.assertEqual(d["journal"], [{"seq": 1, "id": "cmd:r1", "kind": "cultivate", "revision": 1}])
        self.assertEqual(d["processed_ids"], ["r1"])

    def test_bp_exhaustion_wrong_province_cap_unknown(self):
        w = make_world()
        w["officers"]["O1"]["bp"] = 10  # deliberately exhausted partial-BP fixture; new default is Politics
        bus = Bus(w)
        for i in range(5):
            bus.commit(cmd("r" + str(i), i))
        self.assertEqual(bus.data["officers"]["O1"]["bp"], 0)
        h = world_hash(bus.data)
        self.assertEqual(code_of(lambda: bus.commit(cmd("x", 5))), "NO_BP")
        self.assertEqual(world_hash(bus.data), h)
        bus2 = make_bus()
        self.assertEqual(code_of(lambda: bus2.commit(cmd("x", 0, params={"province_id": "P2"}))), "WRONG_PROVINCE")
        self.assertEqual(code_of(lambda: bus2.commit(cmd("x", 0, params={"province_id": "PX"}))), "UNKNOWN_PROVINCE")
        self.assertEqual(code_of(lambda: bus2.commit(cmd("x", 0, params={}))), "BAD_PARAMS")
        w = make_world()
        w["provinces"]["P1"]["cultivation"] = 100
        self.assertEqual(code_of(lambda: Bus(w).commit(cmd("x", 0))), "AT_CAP")


class WorldValidationTests(unittest.TestCase):
    def test_float_bool_out_of_range_in_state(self):
        for mutate, code in (
            (lambda w: w["provinces"]["P1"].__setitem__("cultivation", 1.5), "NOT_INT"),
            (lambda w: w["provinces"]["P1"].__setitem__("cultivation", True), "NOT_INT"),
            (lambda w: w["provinces"]["P1"].__setitem__("cultivation", 101), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["P1"].__setitem__("cultivation", -1), "OUT_OF_RANGE"),
            (lambda w: w["officers"]["O1"].__setitem__("bp", 201), "OUT_OF_RANGE"),
            (lambda w: w.__setitem__("rng_state", 2 ** 64), "OUT_OF_RANGE"),
            (lambda w: w.__setitem__("extra", 1), "BAD_SHAPE"),
        ):
            w = make_world()
            mutate(w)
            self.assertEqual(code_of(lambda: validate_world(w)), code)
            self.assertEqual(code_of(lambda: Bus(w)), code)


class SaveTests(unittest.TestCase):
    def played(self):
        bus = make_bus()
        bus.commit(cmd("a", 0, "test.roll", {"n": 50}))
        bus.commit(cmd("b", 1))
        return bus.data

    def test_roundtrip_hash_equal_and_file(self):
        d = self.played()
        self.assertEqual(world_hash(load_save(dump_save(d))), world_hash(d))
        with tempfile.TemporaryDirectory() as t:
            p = os.path.join(t, "s.json")
            write_save(p, d)
            self.assertFalse(os.path.exists(p + ".tmp"))
            self.assertEqual(world_hash(read_save(p)), world_hash(d))

    def test_reload_keeps_processed_ids_no_replay(self):
        d = load_save(dump_save(self.played()))
        bus = Bus(d, {"test.roll": h_roll})
        self.assertEqual(code_of(lambda: bus.commit(cmd("b", 2))), "DUPLICATE_ID")

    def test_save_is_canonical(self):
        d = self.played()
        self.assertEqual(dump_save(d), dump_save(copy.deepcopy(d)))

    def test_bad_saves_rejected(self):
        raw = dump_save(self.played())
        doc = json.loads(raw)

        def enc(x):
            return json.dumps(x).encode()
        tampered = copy.deepcopy(doc)
        tampered["world"]["provinces"]["P1"]["cultivation"] = 50
        wrong_ver = copy.deepcopy(doc)
        wrong_ver["schema_version"] = 99
        extra = copy.deepcopy(doc)
        extra["note"] = "x"
        cases = [
            (raw[:-10], "BAD_SAVE"),
            (b"\xff\xfe", "BAD_SAVE"),
            (enc(tampered), "HASH_MISMATCH"),
            (enc(wrong_ver), "UNKNOWN_SCHEMA"),
            (enc(extra), "BAD_SAVE"),
            (raw.replace(b'"revision":2', b'"revision":2.0'), "FLOAT"),
            (raw.replace(b'"revision":2', b'"revision":2,"revision":2'), "DUP_KEY"),
            (raw.replace(b'"revision":2', b'"revision":NaN'), "BAD_CONST"),
        ]
        for blob, code in cases:
            self.assertEqual(code_of(lambda: load_save(blob)), code, code)

    def test_valid_hash_but_invalid_world_rejected(self):
        d = self.played()
        d["provinces"]["P1"]["cultivation"] = 500
        doc = {"schema_version": 1, "sha256": hashlib.sha256(canonical_bytes(d)).hexdigest(), "world": d}
        self.assertEqual(code_of(lambda: load_save(canonical_bytes(doc))), "OUT_OF_RANGE")

    def test_dump_refuses_invalid_world(self):
        d = self.played()
        d["officers"]["O1"]["xp"] = 0.5
        self.assertEqual(code_of(lambda: dump_save(d)), "NOT_INT")


if __name__ == "__main__":
    unittest.main()
