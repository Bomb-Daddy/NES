"""REBUILT (SUP-NOB-0046): edits re-derived by Nobunaga on 2026-10-28; the originals were never published.
March frozen-tax-base golden (SUP-ECON-0044). All coefficients are SYNTHETIC fixtures (og.core.world2.FIXTURE_V1),
not Director-approved balance."""
import copy
import unittest

from og.core import scheduler as S
from og.core import world2 as W2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import add_officer, add_province, canonical_bytes, new_world, validate_world, world_hash


# REBUILT: Q added a real phase-2 disaster handler that draws RNG; these tests predate it and assert exact economics,
# so the disaster handler is swapped for a no-op for the whole module (disasters are covered by test_features_q).
_ORIG_DISASTER = None


def setUpModule():
    global _ORIG_DISASTER
    _ORIG_DISASTER = S.PLACEHOLDER_HANDLERS["disaster"]
    S.PLACEHOLDER_HANDLERS["disaster"] = lambda world, month: None


def tearDownModule():
    S.PLACEHOLDER_HANDLERS["disaster"] = _ORIG_DISASTER


def code_of(fn, *a, **k):
    try:
        fn(*a, **k)
    except CoreError as e:
        return e.code
    return None


def v1_world(seed=7):
    w = new_world(seed)
    add_province(w, "p1", 10)
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
    return w


def v2_bus(start_month=3, wealth=40000, rate=10, seed=7):
    """Month 1 of the run is calendar month `start_month` of year 1 (3 = March)."""
    w = W2.migrate_v1_to_v2(v1_world(seed), start_year=1, start_month=start_month)
    p = w["provinces"]["p1"]
    p["workers"] = {"farmers": 100, "town": 50, "artisans": 20, "metalworkers": 10, "resources": 10, "unassigned": 10, "land": 0, "flood": 0}
    p["wealth_milli"] = wealth
    p["support_ticks"] = 100000
    p["tax_rate_percent"] = rate
    validate_world(w)
    return Bus(w)


def run_phases(bus, month, upto):
    for ph in range(0, upto + 1):
        S.run_phase(bus, month, ph, bus.data["revision"])


class MigrationTests(unittest.TestCase):
    def test_v1_untouched_and_v2_valid(self):
        w1 = v1_world()
        before = canonical_bytes(w1)
        w2 = W2.migrate_v1_to_v2(w1, start_year=1, start_month=3)
        self.assertEqual(canonical_bytes(w1), before)
        self.assertEqual(w1["schema_version"], 1)
        self.assertEqual(w2["schema_version"], 2)
        validate_world(w2)
        self.assertEqual(w2["provinces"]["p1"]["cultivation"], 10)
        self.assertEqual(w2["provinces"]["p1"]["march_tax_frozen"], {})
        self.assertEqual(w2["ledger"], [])

    def test_v1_save_loads_as_v1_never_silently_migrated(self):
        raw = dump_save(v1_world())
        self.assertEqual(load_save(raw)["schema_version"], 1)

    def test_v2_save_roundtrip_and_envelope_mismatch(self):
        b = v2_bus()
        raw = dump_save(b.data)
        self.assertEqual(world_hash(load_save(raw)), world_hash(b.data))
        self.assertIn(b'"schema_version":2', raw)
        bad = raw.replace(b'{"schema_version":2,"sha256"', b'{"schema_version":1,"sha256"')
        self.assertEqual(code_of(load_save, bad), "BAD_SAVE")

    def test_v2_field_validation(self):
        cases = [
            (lambda p: p.__setitem__("wealth_milli", 100001), "OUT_OF_RANGE"),
            (lambda p: p.__setitem__("wealth_milli", 1.5), "NOT_INT"),
            (lambda p: p.__setitem__("support_ticks", 5000001), "OUT_OF_RANGE"),
            (lambda p: p.__setitem__("tax_rate_percent", 101), "OUT_OF_RANGE"),
            (lambda p: p["workers"].__setitem__("farmers", -1), "OUT_OF_RANGE"),
            (lambda p: p["workers"].__setitem__("extra", 1), "BAD_SHAPE"),
            (lambda p: p.__setitem__("march_tax_frozen", {"year": 1}), "BAD_SHAPE"),
            (lambda p: p.__setitem__("bogus", 1), "BAD_SHAPE"),
        ]
        for mutate, code in cases:
            w = v2_bus().data
            mutate(w["provinces"]["p1"])
            self.assertEqual(code_of(validate_world, w), code)

    def test_calendar_year_rollover(self):
        w = v2_bus(start_month=11).data
        self.assertEqual(W2.calendar_of(w, 1), (1, 11))
        self.assertEqual(W2.calendar_of(w, 2), (1, 12))
        self.assertEqual(W2.calendar_of(w, 3), (2, 1))
        self.assertEqual(W2.calendar_of(w, 15), (3, 1))


class MarchGolden(unittest.TestCase):
    def test_phase0_freezes_phase3_changes_phase6_uses_frozen(self):
        b = v2_bus()
        run_phases(b, 1, 0)
        fz = b.data["provinces"]["p1"]["march_tax_frozen"]
        self.assertEqual(fz, {"year": 1, "month": 3, "eligible_civilians": 200, "wealth_milli": 40000,
                              "support_ticks": 100000, "tax_rate_percent": 10})
        run_phases_from = lambda lo, hi: [S.run_phase(b, 1, ph, b.data["revision"]) for ph in range(lo, hi + 1)]
        run_phases_from(1, 3)
        p = b.data["provinces"]["p1"]
        self.assertEqual(p["wealth_milli"], 41000)
        self.assertEqual(p["march_tax_frozen"]["wealth_milli"], 40000)
        run_phases_from(4, 5)
        self.assertEqual(p["treasury_gold_cents"], 0)
        run_phases_from(6, 6)
        p = b.data["provinces"]["p1"]
        self.assertEqual(p["treasury_gold_cents"], 6400)  # frozen 40000, NOT live 41000 (would be 6560)
        self.assertEqual(p["wealth_milli"], 41000)
        self.assertEqual(p["march_tax_frozen"], {})
        self.assertEqual(b.data["ledger"], [{"seq": 1, "id": "tax:p1:1", "kind": "TaxCollected",
                                             "province_id": "p1", "year": 1, "amount_cents": 6400}])

    def test_replay_uninterrupted_equals_save_cold_load_at_phase0_and_phase3(self):
        a = v2_bus()
        S.run_month(a)
        for cut in (0, 3):
            b = v2_bus()
            run_phases(b, 1, cut)
            c = Bus(load_save(dump_save(b.data)))
            for ph in range(cut + 1, 13):
                S.run_phase(c, 1, ph, c.data["revision"])
            self.assertEqual(world_hash(c.data), world_hash(a.data), "cut at phase %d" % cut)
            self.assertEqual(c.data["ledger"], a.data["ledger"])
            self.assertEqual(c.data["journal"], a.data["journal"])
            self.assertEqual(c.data["provinces"]["p1"]["treasury_gold_cents"], 6400)

    def test_duplicate_same_year_collection_rejected(self):
        b = v2_bus()
        S.run_month(b)
        staged = copy.deepcopy(b.data)
        staged["provinces"]["p1"]["march_tax_frozen"] = {"year": 1, "month": 3, "eligible_civilians": 200,
                                                           "wealth_milli": 40000, "support_ticks": 100000,
                                                           "tax_rate_percent": 10}
        before = canonical_bytes(staged)
        self.assertEqual(code_of(S.PLACEHOLDER_HANDLERS["fiscal"], staged, 1), "DUPLICATE_TAX")
        self.assertEqual(canonical_bytes(staged), before)
        self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"]), "DUPLICATE_ID")

    def test_failure_at_phase6_preserves_byte_identical_state_then_retry_matches(self):
        a = v2_bus()
        S.run_month(a)
        b = v2_bus()
        run_phases(b, 1, 5)
        before = canonical_bytes(b.data)
        rng, rev, jr = b.data["rng_state"], b.data["revision"], copy.deepcopy(b.data["journal"])
        self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"], None, 6), "FAULT")
        self.assertEqual(canonical_bytes(b.data), before)
        self.assertEqual((b.data["rng_state"], b.data["revision"], b.data["journal"]), (rng, rev, jr))
        self.assertEqual(b.data["provinces"]["p1"]["treasury_gold_cents"], 0)
        self.assertEqual(b.data["ledger"], [])
        for ph in range(6, 13):
            S.run_phase(b, 1, ph, b.data["revision"])
        self.assertEqual(world_hash(b.data), world_hash(a.data))

    def test_invalid_or_missing_snapshot_rejected_state_untouched(self):
        for corrupt, code in (
            (lambda fz: fz.__setitem__("year", 2), "BAD_SNAPSHOT"),
            (lambda fz: fz.__setitem__("extra", 1), "BAD_SNAPSHOT"),
            (lambda fz: fz.pop("wealth_milli"), "BAD_SNAPSHOT"),
            (lambda fz: fz.clear(), "MISSING_SNAPSHOT"),
        ):
            b = v2_bus()
            run_phases(b, 1, 5)
            corrupt(b.data["provinces"]["p1"]["march_tax_frozen"])
            before = canonical_bytes(b.data)
            self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"]), code)
            self.assertEqual(canonical_bytes(b.data), before)

    def test_non_march_month_freezes_and_taxes_nothing(self):
        b = v2_bus(start_month=2)
        S.run_month(b)
        p = b.data["provinces"]["p1"]
        self.assertEqual(p["march_tax_frozen"], {})
        self.assertEqual(p["treasury_gold_cents"], 0)
        self.assertEqual(b.data["ledger"], [])
        self.assertEqual(p["wealth_milli"], 41000)  # phase-3 wealth change applies every month (synthetic)

    def test_second_march_collects_again_new_year(self):
        b = v2_bus()
        for _ in range(13):  # months 1..13 -> March year 1 .. April year 2 ; month 13 is March of year 2
            S.run_month(b)
        ids = [e["id"] for e in b.data["ledger"]]
        self.assertEqual(ids, ["tax:p1:1", "tax:p1:2"])

    def test_wealth_capped_at_100(self):
        b = v2_bus(wealth=99800)
        S.run_month(b)
        self.assertEqual(b.data["provinces"]["p1"]["wealth_milli"], 100000)

    def test_tax_uses_frozen_rate_and_civilians_not_live(self):
        b = v2_bus()
        run_phases(b, 1, 3)
        p = b.data["provinces"]["p1"]
        p["tax_rate_percent"] = 50
        p["workers"]["farmers"] = 0
        S.run_phase(b, 1, 4, b.data["revision"])
        S.run_phase(b, 1, 5, b.data["revision"])
        S.run_phase(b, 1, 6, b.data["revision"])
        self.assertEqual(b.data["provinces"]["p1"]["treasury_gold_cents"], 6400)


if __name__ == "__main__":
    unittest.main()
