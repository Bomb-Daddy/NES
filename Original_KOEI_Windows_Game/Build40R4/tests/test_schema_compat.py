"""SUP-NOB-0046 task B: pre-Q schema-v2 saves must still load (explicit backward read), current v2 must round-trip,
and invalid saves must be rejected. The legacy fixture is written out LITERALLY below (not generated from og.core
constants) so it cannot drift with the code under test. Coefficients are SYNTHETIC-NOT-APPROVED."""
import copy
import hashlib
import json
import unittest

from og.core import scheduler as S
from og.core import world2 as W2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import canonical_bytes, validate_world, world_hash


def legacy_world():
    """Exact pre-Q v2 shape: 6 worker keys, no troops/flood_control/*_credit_milli on provinces, no event_ledger."""
    prov = {
        "cultivation": 10, "wealth_milli": 40000, "support_ticks": 100000, "tax_rate_percent": 10,
        "workers": {"farmers": 101, "town": 50, "artisans": 21, "metalworkers": 10, "resources": 10, "unassigned": 10},
        "treasury_gold_cents": 777, "food_units": 0, "march_tax_frozen": {}, "farm_credit_milli": 0,
        "artisan_credit_milli": 0, "last_september_settled_year": 0, "specialty_units": 5,
        "warehouse_capacity_units": 30,
    }
    return {"schema_version": 2, "revision": 0, "rng_state": 7, "processed_ids": [], "journal": [],
            "provinces": {"p1": prov},
            "officers": {"o1": {"province_id": "p1", "xp": 0, "bp": 10, "politics": 50, "war": 50, "leadership": 50,
                                 "intelligence": 50, "charisma": 50}},
            "calendar": {"start_year": 1, "start_month": 3}, "ledger": [], "stock_ledger": []}


def envelope(world, version=2, sha=None):
    sha = sha if sha is not None else hashlib.sha256(canonical_bytes(world)).hexdigest()
    return canonical_bytes({"schema_version": version, "sha256": sha, "world": world})


def code_of(fn, *a):
    try:
        fn(*a)
    except CoreError as e:
        return e.code
    return None


class OldV2ColdLoad(unittest.TestCase):
    def test_legacy_fixture_is_actually_rejected_by_current_validator_unupgraded(self):
        self.assertEqual(code_of(validate_world, legacy_world()), "BAD_SHAPE")  # proves the regression existed

    def test_old_v2_cold_loads_with_defaults_and_validates(self):
        raw = envelope(legacy_world())
        w = load_save(raw)
        validate_world(w)
        self.assertEqual(w["schema_version"], 2)
        p = w["provinces"]["p1"]
        self.assertEqual((p["workers"]["land"], p["workers"]["flood"]), (0, 0))
        self.assertEqual((p["troops"], p["flood_control"], p["land_credit_milli"], p["flood_credit_milli"]), (0, 0, 0, 0))
        self.assertEqual(w["event_ledger"], [])
        # every pre-existing value is preserved exactly
        old = legacy_world()["provinces"]["p1"]
        for k, v in old.items():
            if k == "workers":
                for wk, wv in v.items():
                    self.assertEqual(p["workers"][wk], wv, wk)
            else:
                self.assertEqual(p[k], v, k)

    def test_old_v2_loads_to_same_world_as_current_construction(self):
        a = load_save(envelope(legacy_world()))
        b = copy.deepcopy(legacy_world())
        cur = W2.upgrade_legacy_v2(b)
        self.assertEqual(world_hash(a), world_hash(cur))
        self.assertEqual(canonical_bytes(a), canonical_bytes(cur))

    def test_input_not_mutated_and_upgrade_idempotent(self):
        w = legacy_world()
        before = canonical_bytes(w)
        up = W2.upgrade_legacy_v2(w)
        self.assertEqual(canonical_bytes(w), before)
        self.assertIs(W2.upgrade_legacy_v2(up), up)  # already current: returned unchanged

    def test_loaded_old_save_runs_a_month_and_resaves(self):
        b = Bus(load_save(envelope(legacy_world())))
        S.run_month(b)  # March: freeze (phase 0) and collect (phase 6)
        self.assertEqual(len(b.data["ledger"]), 1)
        c = Bus(load_save(dump_save(b.data)))
        self.assertEqual(world_hash(c.data), world_hash(b.data))


class CurrentV2RoundTrip(unittest.TestCase):
    def test_current_world_roundtrip_hash_and_bytes_identical(self):
        w = W2.upgrade_legacy_v2(legacy_world())
        raw = dump_save(w)
        w2 = load_save(raw)
        self.assertEqual(world_hash(w2), world_hash(w))
        self.assertEqual(dump_save(w2), raw)
        self.assertIn(b'"schema_version":2', raw)
        self.assertFalse(W2.is_legacy_v2_shape(w))


class InvalidSaveRejection(unittest.TestCase):
    def expect(self, raw, code):
        self.assertEqual(code_of(load_save, raw), code)

    def test_bad_sha_on_legacy_save(self):
        self.expect(envelope(legacy_world(), sha="0" * 64), "HASH_MISMATCH")

    def test_unknown_schema_and_bool_version(self):
        self.expect(envelope(legacy_world(), version=99), "UNKNOWN_SCHEMA")
        self.expect(envelope(legacy_world(), version=True), "UNKNOWN_SCHEMA")

    def test_envelope_version_must_match_world(self):
        w = legacy_world()
        w["schema_version"] = 1
        self.expect(envelope(w, version=2), "BAD_SAVE")

    def test_half_upgraded_world_rejected(self):
        w = legacy_world()
        w["provinces"]["p1"]["workers"]["land"] = 0
        w["provinces"]["p1"]["workers"]["flood"] = 0  # workers current, province fields still legacy
        self.expect(envelope(w), "BAD_SHAPE")
        w2 = legacy_world()
        w2["event_ledger"] = []  # top level current, provinces legacy
        self.expect(envelope(w2), "BAD_SHAPE")

    def test_legacy_with_unknown_extra_key_rejected(self):
        w = legacy_world()
        w["provinces"]["p1"]["bogus"] = 1
        self.expect(envelope(w), "BAD_SHAPE")
        w = legacy_world()
        w["provinces"]["p1"]["workers"]["bogus"] = 1
        self.expect(envelope(w), "BAD_SHAPE")

    def test_out_of_range_value_still_rejected_after_upgrade(self):
        w = legacy_world()
        w["provinces"]["p1"]["wealth_milli"] = 100001
        self.expect(envelope(w), "OUT_OF_RANGE")
        w = legacy_world()
        w["provinces"]["p1"]["workers"]["farmers"] = -1
        self.expect(envelope(w), "OUT_OF_RANGE")

    def test_float_and_garbage_rejected(self):
        raw = envelope(legacy_world()).replace(b'"wealth_milli":40000', b'"wealth_milli":40000.0')
        self.expect(raw, "FLOAT")
        self.expect(b"not json", "BAD_SAVE")
        self.expect(json.dumps({"schema_version": 2}).encode(), "BAD_SAVE")

    def test_dump_save_refuses_unupgraded_legacy_world(self):
        self.assertEqual(code_of(dump_save, legacy_world()), "BAD_SHAPE")


if __name__ == "__main__":
    unittest.main()
