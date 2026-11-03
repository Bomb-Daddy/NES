"""REBUILT (SUP-NOB-0046): edits re-derived by Nobunaga on 2026-10-28; the originals were never published.
September once-per-year payout + 12-month one-province run (SUP-NOB-0045 B2). Coefficients are SYNTHETIC
(og.core.world2.FIXTURE_V1), not Director-approved balance."""
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


def v2_bus(start_month=9, farmers=101, artisans=21, cultivation=10, capacity=30, specialty=5, seed=7, wealth=40000,
           rate=10):
    """Month 1 of the run is calendar month `start_month` of year 1 (9 = September)."""
    w = new_world(seed)
    add_province(w, "p1", cultivation)
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
    w = W2.migrate_v1_to_v2(w, start_year=1, start_month=start_month)
    p = w["provinces"]["p1"]
    p["workers"] = {"farmers": farmers, "town": 50, "artisans": artisans, "metalworkers": 10, "resources": 10,
                    "unassigned": 10, "land": 0, "flood": 0}
    p["wealth_milli"] = wealth
    p["support_ticks"] = 100000
    p["tax_rate_percent"] = rate
    p["warehouse_capacity_units"] = capacity
    p["specialty_units"] = specialty
    p["treasury_gold_cents"] = 777
    validate_world(w)
    return Bus(w)


def upto(bus, month, last_phase, first=0):
    for ph in range(first, last_phase + 1):
        S.run_phase(bus, month, ph, bus.data["revision"])


class SeptemberGolden(unittest.TestCase):
    def test_phase3_accrues_phase6_pays_once_with_remainder_and_overflow(self):
        b = v2_bus()
        upto(b, 1, 2)
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["farm_credit_milli"], p["artisan_credit_milli"]), (0, 0))
        upto(b, 1, 3, first=3)
        p = b.data["provinces"]["p1"]
        # farmers 101 x (500 + 10 x cultivation 10) = 60600 ; artisans 21 x 2500 = 52500
        self.assertEqual((p["farm_credit_milli"], p["artisan_credit_milli"]), (60600, 52500))
        self.assertEqual((p["food_units"], p["specialty_units"]), (0, 5))  # nothing paid before phase 6
        upto(b, 1, 5, first=4)
        self.assertEqual(b.data["provinces"]["p1"]["food_units"], 0)
        upto(b, 1, 6, first=6)
        p = b.data["provinces"]["p1"]
        self.assertEqual(p["food_units"], 60)
        self.assertEqual(p["farm_credit_milli"], 600)  # persisted sub-unit remainder
        self.assertEqual(p["artisan_credit_milli"], 500)
        self.assertEqual(p["specialty_units"], 30)  # 5 + 25 accepted, capacity 30
        self.assertEqual(p["last_september_settled_year"], 1)
        self.assertEqual(p["treasury_gold_cents"], 777)  # no government import purchase, no spending
        self.assertEqual([(e["id"], e["kind"], e["good"], e["units"]) for e in b.data["stock_ledger"]], [
            ("sep:p1:1:food", "SeptemberPayout", "food", 60),
            ("sep:p1:1:specialty", "SeptemberPayout", "specialty", 25),
            ("sep:p1:1:overflow", "OverflowDiscarded", "specialty", 27),
        ])
        self.assertEqual(b.data["ledger"], [])  # no tax outside March

    def test_replay_across_save_boundary_is_identical(self):
        a = v2_bus()
        S.run_month(a)
        for cut in (0, 2, 3, 5):
            b = v2_bus()
            upto(b, 1, cut)
            c = Bus(load_save(dump_save(b.data)))
            upto(c, 1, 12, first=cut + 1)
            self.assertEqual(world_hash(c.data), world_hash(a.data), "cut %d" % cut)
            self.assertEqual(c.data["stock_ledger"], a.data["stock_ledger"])
            self.assertEqual(c.data["provinces"]["p1"]["food_units"], 60)

    def test_duplicate_same_year_payout_rejected_world_untouched(self):
        b = v2_bus()
        S.run_month(b)
        staged = copy.deepcopy(b.data)
        staged["provinces"]["p1"]["farm_credit_milli"] = 99000
        before = canonical_bytes(staged)
        self.assertEqual(code_of(S.PLACEHOLDER_HANDLERS["fiscal"], staged, 1), "DUPLICATE_PAYOUT")
        self.assertEqual(canonical_bytes(staged), before)
        self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"]), "DUPLICATE_ID")

    def test_failure_at_phase6_preserves_byte_identical_state_then_retry_matches(self):
        a = v2_bus()
        S.run_month(a)
        b = v2_bus()
        upto(b, 1, 5)
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"], None, 6), "FAULT")
        self.assertEqual(canonical_bytes(b.data), before)
        upto(b, 1, 12, first=6)
        self.assertEqual(world_hash(b.data), world_hash(a.data))

    def test_non_september_months_accrue_and_pay_nothing(self):
        b = v2_bus(start_month=8)
        S.run_month(b)
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["farm_credit_milli"], p["artisan_credit_milli"], p["food_units"]), (0, 0, 0))
        self.assertEqual(b.data["stock_ledger"], [])

    def test_second_september_carries_remainder(self):
        b = v2_bus()
        for _ in range(13):  # month 1 = Sept yr1 ... month 13 = Sept yr2
            S.run_month(b)
        p = b.data["provinces"]["p1"]
        # yr2: remainder 600 + 60600 = 61200 -> 61 units, remainder 200 ; food 60 + 61
        self.assertEqual(p["food_units"], 121)
        self.assertEqual(p["farm_credit_milli"], 200)
        self.assertEqual(p["last_september_settled_year"], 2)
        self.assertEqual([e["id"] for e in b.data["stock_ledger"] if e["kind"] == "SeptemberPayout" and e["good"] == "food"],
                         ["sep:p1:1:food", "sep:p1:2:food"])

    def test_no_overflow_event_when_stock_fits(self):
        b = v2_bus(capacity=1000)
        S.run_month(b)
        self.assertEqual(b.data["provinces"]["p1"]["specialty_units"], 5 + 52)
        self.assertNotIn("OverflowDiscarded", [e["kind"] for e in b.data["stock_ledger"]])

    def test_exactly_full_warehouse_discards_all_new_specialty(self):
        b = v2_bus(capacity=5, specialty=5)
        S.run_month(b)
        self.assertEqual(b.data["provinces"]["p1"]["specialty_units"], 5)
        ev = [e for e in b.data["stock_ledger"] if e["kind"] == "OverflowDiscarded"]
        self.assertEqual([e["units"] for e in ev], [52])

    def test_credit_depends_on_cultivation(self):
        lo, hi = v2_bus(cultivation=10), v2_bus(cultivation=50)
        upto(lo, 1, 3)
        upto(hi, 1, 3)
        self.assertEqual(lo.data["provinces"]["p1"]["farm_credit_milli"], 101 * (500 + 100))
        self.assertEqual(hi.data["provinces"]["p1"]["farm_credit_milli"], 101 * (500 + 500))

    def test_september_field_validation(self):
        cases = [
            (lambda w: w["provinces"]["p1"].__setitem__("specialty_units", -1), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["p1"].__setitem__("warehouse_capacity_units", 1.5), "NOT_INT"),
            (lambda w: w["stock_ledger"].append({"seq": 1, "id": "x", "kind": "Bogus", "province_id": "p1",
                                                 "year": 1, "good": "food", "units": 1}), "BAD_SHAPE"),
            (lambda w: w["stock_ledger"].append({"seq": 2, "id": "x", "kind": "SeptemberPayout",
                                                 "province_id": "p1", "year": 1, "good": "food", "units": 1}),
             "BAD_SHAPE"),
        ]
        for mutate, code in cases:
            w = v2_bus().data
            mutate(w)
            self.assertEqual(code_of(validate_world, w), code)


class TwelveMonthRun(unittest.TestCase):
    def run_year(self, seed):
        b = v2_bus(start_month=1, seed=seed)  # Jan year 1 .. Dec year 1
        for _ in range(12):
            S.run_month(b)
        return b

    def test_one_province_twelve_months_march_and_september_each_once(self):
        b = self.run_year(7)
        p = b.data["provinces"]["p1"]
        self.assertEqual(b.data["revision"], 12 * 13)
        self.assertEqual(S.cursor(b.data), (13, 0))
        # March (month 3): opening wealth 40000 + 2 x 1000 (Jan, Feb) = 42000 frozen ; 201+... see below
        self.assertEqual([e["id"] for e in b.data["ledger"]], ["tax:p1:1"])
        civilians = 101 + 50 + 21 + 10 + 10 + 10
        civilians += civilians * 2 // 100  # REBUILT: January growth (+2%, floor) precedes the March freeze
        self.assertEqual(b.data["ledger"][0]["amount_cents"], civilians * 42000 * 10 * 10 * 80 // 10000000)
        self.assertEqual(p["treasury_gold_cents"], 777 + b.data["ledger"][0]["amount_cents"])
        self.assertEqual(p["march_tax_frozen"], {})
        self.assertEqual(p["wealth_milli"], 52000)  # 40000 + 12 x 1000
        self.assertEqual(p["food_units"], 60)
        self.assertEqual(p["farm_credit_milli"], 600)
        self.assertEqual(p["specialty_units"], 30)
        self.assertEqual(p["last_september_settled_year"], 1)
        self.assertEqual(len([e for e in b.data["stock_ledger"] if e["kind"] == "SeptemberPayout"]), 2)

    def test_twelve_months_same_economy_across_three_seeds_and_replay(self):
        econ = []
        for seed in (7, 8, 9):
            b = self.run_year(seed)
            p = b.data["provinces"]["p1"]
            econ.append((p["treasury_gold_cents"], p["food_units"], p["specialty_units"], p["wealth_milli"],
                         b.data["ledger"], b.data["stock_ledger"]))
        self.assertEqual(econ[0], econ[1])
        self.assertEqual(econ[0], econ[2])
        a = self.run_year(7)
        b = v2_bus(start_month=1, seed=7)
        for _ in range(5):
            S.run_month(b)
        upto(b, 6, 4)  # mid-month 6, then save and cold-load, then finish the year
        c = Bus(load_save(dump_save(b.data)))
        upto(c, 6, 12, first=5)
        for _ in range(6):
            S.run_month(c)
        self.assertEqual(world_hash(c.data), world_hash(a.data))


if __name__ == "__main__":
    unittest.main()
