"""REBUILT (SUP-NOB-0046): edits re-derived by Nobunaga on 2026-10-28; the originals were never published.
Two-province 12-month run. Expected values come from an independent reference model written in this file
(plain integer arithmetic from the SYNTHETIC coefficients), not from og.core.economy. Coefficients are SYNTHETIC."""
import copy
import unittest
from unittest import mock

from og.core import economy
from og.core import scheduler as S
from og.core import world2 as W2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import add_officer, add_province, canonical_bytes, new_world, validate_world, world_hash

PARAMS = {
    "p1": dict(cult=10, workers=(101, 50, 21, 10, 10, 10, 0, 0), wealth=40000, rate=10, cap=30, spec=5, treasury=777,
               support=100000),
    "p2": dict(cult=55, workers=(37, 20, 64, 3, 5, 0, 0, 0), wealth=99500, rate=25, cap=1000, spec=0, treasury=0,
               support=50000),
}
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


def build(seed=7, start_month=1):
    w = new_world(seed)
    for pid, q in PARAMS.items():
        add_province(w, pid, q["cult"])
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
    w = W2.migrate_v1_to_v2(w, start_year=1, start_month=start_month)
    for pid, q in PARAMS.items():
        p = w["provinces"][pid]
        p["workers"] = dict(zip(KEYS, q["workers"]))
        p["wealth_milli"] = q["wealth"]
        p["support_ticks"] = q["support"]
        p["tax_rate_percent"] = q["rate"]
        p["warehouse_capacity_units"] = q["cap"]
        p["specialty_units"] = q["spec"]
        p["treasury_gold_cents"] = q["treasury"]
    validate_world(w)
    return Bus(w)


def reference(q):
    """Independent expected end-of-year state for one province (Jan..Dec year 1), from first principles."""
    wealth = q["wealth"]
    frozen = None
    for month in range(1, 13):
        if month == 3:
            frozen = wealth  # snapshot is taken before this month's wealth change
        wealth = min(100000, wealth + 1000)
    civilians = sum(q["workers"])
    civilians += civilians * 2 // 100  # REBUILT: January growth (+2%, floor) precedes the March freeze
    compliance = 80 + (q["support"] // 50000) // 5
    tax = civilians * frozen * q["rate"] * 10 * compliance // 10000000
    farm = q["workers"][0] * (500 + 10 * q["cult"])
    art = q["workers"][2] * 2500
    food, farm_rem = farm // 1000, farm % 1000
    spec, art_rem = art // 1000, art % 1000
    accepted = min(spec, max(0, q["cap"] - q["spec"]))
    return dict(wealth=wealth, tax=tax, treasury=q["treasury"] + tax, food=food, farm_rem=farm_rem,
                art_rem=art_rem, specialty=q["spec"] + accepted, accepted=accepted, overflow=spec - accepted)


def year(seed=7):
    b = build(seed)
    for _ in range(12):
        S.run_month(b)
    return b


class TwoProvinceYear(unittest.TestCase):
    def test_each_province_matches_independent_reference(self):
        b = year()
        for pid, q in PARAMS.items():
            r, p = reference(q), b.data["provinces"][pid]
            self.assertEqual(p["wealth_milli"], r["wealth"], pid)
            self.assertEqual(p["treasury_gold_cents"], r["treasury"], pid)
            self.assertEqual(p["food_units"], r["food"], pid)
            self.assertEqual(p["farm_credit_milli"], r["farm_rem"], pid)
            self.assertEqual(p["artisan_credit_milli"], r["art_rem"], pid)
            self.assertEqual(p["specialty_units"], r["specialty"], pid)
            self.assertEqual(p["last_september_settled_year"], 1, pid)
            self.assertEqual(p["march_tax_frozen"], {}, pid)
        self.assertEqual(reference(PARAMS["p2"])["wealth"], 100000)  # p2 hit the cap; reference agrees
        self.assertEqual(reference(PARAMS["p1"])["overflow"], 27)
        self.assertEqual(reference(PARAMS["p2"])["overflow"], 0)

    def test_ledgers_have_one_event_per_province_in_province_order(self):
        b = year()
        self.assertEqual([(e["id"], e["amount_cents"]) for e in b.data["ledger"]],
                         [("tax:p1:1", reference(PARAMS["p1"])["tax"]), ("tax:p2:1", reference(PARAMS["p2"])["tax"])])
        self.assertEqual([(e["seq"], e["id"], e["kind"], e["units"]) for e in b.data["stock_ledger"]], [
            (1, "sep:p1:1:food", "SeptemberPayout", reference(PARAMS["p1"])["food"]),
            (2, "sep:p1:1:specialty", "SeptemberPayout", 25),
            (3, "sep:p1:1:overflow", "OverflowDiscarded", 27),
            (4, "sep:p2:1:food", "SeptemberPayout", reference(PARAMS["p2"])["food"]),
            (5, "sep:p2:1:specialty", "SeptemberPayout", 160),
        ])

    def test_year_identical_across_seeds_and_midyear_cold_load(self):
        a = year(7)
        for seed in (8, 9):
            o = year(seed)
            for pid in PARAMS:
                for k in ("treasury_gold_cents", "food_units", "specialty_units", "wealth_milli"):
                    self.assertEqual(o.data["provinces"][pid][k], a.data["provinces"][pid][k])
            self.assertEqual(o.data["ledger"], a.data["ledger"])
            self.assertEqual(o.data["stock_ledger"], a.data["stock_ledger"])
        for cut_month, cut_phase in ((2, 11), (3, 3), (9, 2), (9, 5), (9, 6)):
            b = build()
            for _ in range(cut_month - 1):
                S.run_month(b)
            for ph in range(cut_phase + 1):
                S.run_phase(b, cut_month, ph, b.data["revision"])
            c = Bus(load_save(dump_save(b.data)))
            for ph in range(cut_phase + 1, 13):
                S.run_phase(c, cut_month, ph, c.data["revision"])
            for _ in range(12 - cut_month):
                S.run_month(c)
            self.assertEqual(world_hash(c.data), world_hash(a.data), "cut %r" % ((cut_month, cut_phase),))

    def test_failure_in_second_province_leaves_first_province_unpaid(self):
        b = build(start_month=9)
        b.data["provinces"]["p2"]["last_september_settled_year"] = 1  # p2 payout will be rejected as a duplicate
        for ph in range(6):
            S.run_phase(b, 1, ph, b.data["revision"])
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"]), "DUPLICATE_PAYOUT")
        self.assertEqual(canonical_bytes(b.data), before)
        self.assertEqual(b.data["provinces"]["p1"]["food_units"], 0)
        self.assertEqual(b.data["stock_ledger"], [])

    def test_injected_failure_during_p2_payout_from_valid_state_rolls_back_then_retry_matches(self):
        """SUP-QA-0034: both provinces start unsettled (valid state). A deterministic CoreError is raised while p2's
        September payout is being written, after p1 was already processed on the staged copy."""
        a = build(start_month=9)
        S.run_month(a)  # uninterrupted control
        b = build(start_month=9)
        self.assertEqual([b.data["provinces"][p]["last_september_settled_year"] for p in ("p1", "p2")], [0, 0])
        self.assertEqual(b.data["stock_ledger"], [])
        for ph in range(6):
            S.run_phase(b, 1, ph, b.data["revision"])
        before = canonical_bytes(b.data)
        revision, rng, journal = b.data["revision"], b.data["rng_state"], copy.deepcopy(b.data["journal"])
        original = economy._stock_event
        seen = []

        def failing(world, pid, year, tag, kind, good, units):
            seen.append((pid, tag))
            if pid == "p2" and tag == "food":
                raise CoreError("INJECTED", "p2 payout failed")
            return original(world, pid, year, tag, kind, good, units)

        with mock.patch.object(economy, "_stock_event", failing):
            self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"]), "INJECTED")
        self.assertEqual(seen, [("p1", "food"), ("p1", "specialty"), ("p1", "overflow"), ("p2", "food")])  # p1 was processed first
        self.assertEqual(canonical_bytes(b.data), before)  # entire world byte-identical
        self.assertEqual((b.data["revision"], b.data["rng_state"], b.data["journal"]), (revision, rng, journal))
        for pid in ("p1", "p2"):
            p = b.data["provinces"][pid]
            self.assertEqual((p["food_units"], p["farm_credit_milli"], p["artisan_credit_milli"],
                              p["last_september_settled_year"]), (0, PARAMS[pid]["workers"][0] * (500 + 10 * PARAMS[pid]["cult"]), PARAMS[pid]["workers"][2] * 2500, 0))
        self.assertEqual((b.data["ledger"], b.data["stock_ledger"]), ([], []))
        for ph in range(6, 13):  # injection removed: retry
            S.run_phase(b, 1, ph, b.data["revision"])
        self.assertEqual(world_hash(b.data), world_hash(a.data))
        self.assertEqual(b.data["stock_ledger"], a.data["stock_ledger"])
        self.assertEqual(len(b.data["stock_ledger"]), 5)

    def test_fault_at_phase_6_in_march_leaves_both_provinces_untaxed(self):
        a = build(start_month=3)
        S.run_month(a)
        b = build(start_month=3)
        for ph in range(6):
            S.run_phase(b, 1, ph, b.data["revision"])
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 6, b.data["revision"], None, 6), "FAULT")
        self.assertEqual(canonical_bytes(b.data), before)
        for ph in range(6, 13):
            S.run_phase(b, 1, ph, b.data["revision"])
        self.assertEqual(world_hash(b.data), world_hash(a.data))


if __name__ == "__main__":
    unittest.main()
