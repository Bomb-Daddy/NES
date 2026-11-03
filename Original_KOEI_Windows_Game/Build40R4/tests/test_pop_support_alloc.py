"""REBUILT (SUP-NOB-0046): edits re-derived by Nobunaga on 2026-10-28; the originals were never published.
Population growth, March support change and worker allocation command.
Coefficients are SYNTHETIC (og.core.world2.FIXTURE_V1): 2% yearly growth in January (the 2% figure comes from a Nobunaga's
Ambition guide, not from Romance II, whose rate is documented as not established); fair tax 30%; 500 support ticks per
tax point; 1 BP per allocation. Expected values below are written out by hand, not read from og.core."""
import unittest

from og.core import scheduler as S
from og.core import world2 as W2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.world import add_officer, add_province, canonical_bytes, new_world, validate_world

KEYS = ("farmers", "town", "artisans", "metalworkers", "resources", "unassigned", "land", "flood")


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


def build(start_month=1, workers=(100, 50, 20, 10, 10, 12, 0, 0), rate=10, support=100000, bp=10):
    w = new_world(7)
    add_province(w, "p1", 10)
    add_province(w, "p2", 10)
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50), bp=bp)
    w = W2.migrate_v1_to_v2(w, start_year=1, start_month=start_month)
    for pid in ("p1", "p2"):
        p = w["provinces"][pid]
        p["workers"] = dict(zip(KEYS, workers))
        p["wealth_milli"] = 40000
        p["support_ticks"] = support
        p["tax_rate_percent"] = rate
    validate_world(w)
    return Bus(w)


def total(p):
    return sum(p["workers"].values())


def cmd(rid, rev, **params):
    return {"request_id": rid, "actor_officer_id": "o1", "expected_world_revision": rev, "kind": "allocate_workers",
            "params": params}


class PopulationGrowth(unittest.TestCase):
    def test_growth_once_per_year_in_january_goes_to_unassigned(self):
        b = build(start_month=12)  # month 1 = December, month 2 = January
        S.run_month(b)
        self.assertEqual(total(b.data["provinces"]["p1"]), 202)  # December: no growth
        S.run_month(b)  # January: 202 * 2 // 100 = 4
        for pid in ("p1", "p2"):
            w = b.data["provinces"][pid]["workers"]
            self.assertEqual(w["unassigned"], 16)
            self.assertEqual([w[k] for k in KEYS[:5]], [100, 50, 20, 10, 10])
        for _ in range(11):
            S.run_month(b)  # Feb..Dec: no further growth
        self.assertEqual(total(b.data["provinces"]["p1"]), 206)

    def test_two_januaries_compound(self):
        b = build(start_month=1)
        for _ in range(13):
            S.run_month(b)
        # Jan year 1: 202 -> 206 (+4); Jan year 2: 206 * 2 // 100 = 4 -> 210
        self.assertEqual(total(b.data["provinces"]["p1"]), 210)

    def test_tiny_population_floors_to_zero_growth(self):
        b = build(start_month=1, workers=(10, 10, 10, 10, 5, 4, 0, 0))  # 49 * 2 // 100 = 0
        S.run_month(b)
        self.assertEqual(total(b.data["provinces"]["p1"]), 49)

    def test_march_tax_uses_post_growth_population(self):
        b = build(start_month=1)
        for _ in range(3):
            S.run_month(b)
        # frozen at March phase 0: 206 civilians, wealth 40000 + 2 * 1000 (Jan, Feb phase 3), rate 10
        self.assertEqual(b.data["ledger"][0]["amount_cents"], 206 * 42000 * 10 * 10 * 80 // 10000000)

    def test_v1_world_is_not_touched_by_growth_phase(self):
        w = new_world(7)
        add_province(w, "p1", 10)
        b = Bus(w)
        before = canonical_bytes(b.data)
        S.run_month(b)
        self.assertEqual(b.data["provinces"], {"p1": {"cultivation": 10}})
        self.assertNotEqual(canonical_bytes(b.data), before)  # only revision/journal moved


class SupportAtMarchTax(unittest.TestCase):
    def march(self, rate, support):
        b = build(start_month=3, rate=rate, support=support)
        S.run_month(b)
        return b.data["provinces"]["p1"]["support_ticks"]

    def test_below_fair_rate_raises_support(self):
        self.assertEqual(self.march(10, 100000), 100000 + 20 * 500)

    def test_at_fair_rate_no_change(self):
        self.assertEqual(self.march(30, 100000), 100000)

    def test_above_fair_rate_lowers_support(self):
        self.assertEqual(self.march(50, 100000), 100000 - 20 * 500)

    def test_clamped_at_zero_and_at_validator_ceiling(self):
        self.assertEqual(self.march(100, 5000), 0)
        self.assertEqual(self.march(0, 4999000), 5000000)

    def test_uses_frozen_rate_not_rate_changed_after_freeze(self):
        b = build(start_month=3, rate=10)
        S.run_phase(b, 1, 0, 0)
        b._data["provinces"]["p1"]["tax_rate_percent"] = 90
        for ph in range(1, 13):
            S.run_phase(b, 1, ph, b.data["revision"])
        self.assertEqual(b.data["provinces"]["p1"]["support_ticks"], 100000 + 20 * 500)

    def test_other_months_leave_support_alone(self):
        for start in (1, 2, 4, 9, 12):
            b = build(start_month=start, rate=10)
            S.run_month(b)
            self.assertEqual(b.data["provinces"]["p1"]["support_ticks"], 100000, start)

    def test_each_province_uses_its_own_rate(self):
        b = build(start_month=3, rate=10)
        b._data["provinces"]["p2"]["tax_rate_percent"] = 50
        S.run_month(b)
        self.assertEqual([b.data["provinces"][p]["support_ticks"] for p in ("p1", "p2")], [110000, 90000])


class AllocateWorkers(unittest.TestCase):
    def test_moves_workers_conserves_total_and_spends_one_bp(self):
        b = build()
        r = b.commit(cmd("a1", 0, province_id="p1", source="unassigned", target="farmers", count=7))
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["workers"]["unassigned"], p["workers"]["farmers"]), (5, 107))
        self.assertEqual(total(p), 202)
        self.assertEqual(b.data["officers"]["o1"]["bp"], 9)
        self.assertEqual(r["world_revision"], 1)
        self.assertEqual(total(b.data["provinces"]["p2"]), 202)  # other province untouched

    def test_changed_allocation_changes_september_food(self):
        a, b = build(start_month=9), build(start_month=9)
        b.commit(cmd("a1", 0, province_id="p1", source="town", target="farmers", count=10))
        S.run_month(a)
        S.run_month(b)
        # farm credit per farmer = 500 + 10 * 10 = 600 milli; 100 farmers = 60000 -> 60 food; 110 farmers -> 66
        self.assertEqual(a.data["provinces"]["p1"]["food_units"], 60)
        self.assertEqual(b.data["provinces"]["p1"]["food_units"], 66)

    def test_preview_changes_nothing(self):
        b = build()
        before = canonical_bytes(b.data)
        b.preview(cmd("a1", 0, province_id="p1", source="town", target="farmers", count=3))
        self.assertEqual(canonical_bytes(b.data), before)

    def test_rejections_leave_world_byte_identical(self):
        cases = [
            (dict(province_id="p1", source="town", target="town", count=1), "BAD_PARAMS"),
            (dict(province_id="p1", source="town", target="farmers", count=0), "BAD_PARAMS"),
            (dict(province_id="p1", source="town", target="farmers", count=-1), "BAD_PARAMS"),
            (dict(province_id="p1", source="town", target="farmers", count=True), "NOT_INT"),
            (dict(province_id="p1", source="town", target="farmers", count=51), "NOT_ENOUGH_WORKERS"),
            (dict(province_id="p1", source="nobody", target="farmers", count=1), "BAD_PARAMS"),
            (dict(province_id="p1", source="town", target="nobody", count=1), "BAD_PARAMS"),
            (dict(province_id="zz", source="town", target="farmers", count=1), "UNKNOWN_PROVINCE"),
            (dict(province_id="p2", source="town", target="farmers", count=1), "WRONG_PROVINCE"),
            (dict(province_id="p1", source="town", target="farmers"), "BAD_PARAMS"),
            (dict(province_id="p1", source="town", target="farmers", count=1, extra=1), "BAD_PARAMS"),
        ]
        for params, code in cases:
            b = build()
            before = canonical_bytes(b.data)
            self.assertEqual(code_of(b.commit, cmd("x", 0, **params)), code, params)
            self.assertEqual(canonical_bytes(b.data), before, params)

    def test_no_bp_rejected(self):
        b = build(bp=0)
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(b.commit, cmd("a", 0, province_id="p1", source="town", target="farmers", count=1)), "NO_BP")
        self.assertEqual(canonical_bytes(b.data), before)

    def test_can_move_every_worker_out_of_a_bucket(self):
        b = build()
        b.commit(cmd("a", 0, province_id="p1", source="town", target="metalworkers", count=50))
        w = b.data["provinces"]["p1"]["workers"]
        self.assertEqual((w["town"], w["metalworkers"]), (0, 60))

    def test_rejected_on_v1_world(self):
        w = new_world(7)
        add_province(w, "p1", 10)
        add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
        b = Bus(w)
        self.assertEqual(code_of(b.commit, cmd("a", 0, province_id="p1", source="town", target="farmers", count=1)),
                         "BAD_VERSION")

    def test_replay_of_same_request_rejected(self):
        b = build()
        b.commit(cmd("a1", 0, province_id="p1", source="town", target="farmers", count=1))
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(b.commit, cmd("a1", 1, province_id="p1", source="town", target="farmers", count=1)),
                         "DUPLICATE_ID")
        self.assertEqual(canonical_bytes(b.data), before)


if __name__ == "__main__":
    unittest.main()
