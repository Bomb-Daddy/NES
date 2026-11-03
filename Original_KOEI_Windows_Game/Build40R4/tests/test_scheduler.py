import copy
import os
import tempfile
import unittest

from og.core import scheduler as S
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import load_save, dump_save
from og.core.world import add_officer, add_province, new_world, world_hash


def make_bus(seed=7):
    w = new_world(seed)
    add_province(w, "p1", 10)
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
    return Bus(w)


def code_of(fn, *a, **k):
    try:
        fn(*a, **k)
    except CoreError as e:
        return e.code
    return None


class TableTests(unittest.TestCase):
    def test_ids_0_to_12_in_order_unique_keys(self):
        self.assertEqual([p[0] for p in S.PHASE_TABLE], list(range(13)))
        self.assertEqual(len({p[1] for p in S.PHASE_TABLE}), 13)
        self.assertEqual(set(S.PLACEHOLDER_HANDLERS), {p[1] for p in S.PHASE_TABLE})
        self.assertTrue(S.PHASE_IDENTITY_SETTLED)
        self.assertEqual(len(S.PLACEHOLDER_HANDLERS_REMAINING), 7)


class MonthTests(unittest.TestCase):
    def test_month_runs_13_phases_and_advances(self):
        b = make_bus()
        ids = S.run_month(b)
        self.assertEqual(len(ids), 13)
        self.assertEqual(b.data["revision"], 13)
        self.assertEqual(S.cursor(b.data), (2, 0))

    def test_golden_uninterrupted_equals_cold_loaded_midmonth(self):
        a = make_bus()
        S.run_month(a)
        S.run_month(a)
        full_hash, full_ledger = world_hash(a.data), copy.deepcopy(a.data["journal"])
        b = make_bus()
        S.run_month(b)
        for ph in range(0, 5):  # mid-month 2
            S.run_phase(b, 2, ph, b.data["revision"])
        b2 = Bus(load_save(dump_save(b.data)))  # cold load
        self.assertEqual(S.cursor(b2.data), (2, 5))
        for ph in range(5, 13):
            S.run_phase(b2, 2, ph, b2.data["revision"])
        self.assertEqual(world_hash(b2.data), full_hash)
        self.assertEqual(b2.data["journal"], full_ledger)

    def test_completed_phase_not_rerun(self):
        b = make_bus()
        S.run_phase(b, 1, 0, 0)
        h = world_hash(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 0, 1), "DUPLICATE_ID")
        self.assertEqual(code_of(S.run_phase, b, 1, 0, 0), "DUPLICATE_ID")
        self.assertEqual(world_hash(b.data), h)

    def test_stale_revision_and_order_rejected(self):
        b = make_bus()
        h = world_hash(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 0, 5), "STALE_REVISION")
        self.assertEqual(code_of(S.run_phase, b, 1, 3, 0), "PHASE_ORDER")
        self.assertEqual(code_of(S.run_phase, b, 2, 0, 0), "PHASE_ORDER")
        self.assertEqual(code_of(S.run_phase, b, 1, 13, 0), "BAD_PHASE")
        self.assertEqual(code_of(S.run_phase, b, 0, 0, 0), "BAD_PHASE")
        self.assertEqual(world_hash(b.data), h)

    def test_fault_at_each_phase_leaves_zero_partial_state(self):
        for ph in range(13):
            b = make_bus()
            for p in range(ph):
                S.run_phase(b, 1, p, b.data["revision"])
            before = world_hash(b.data)
            self.assertEqual(code_of(S.run_phase, b, 1, ph, b.data["revision"], None, ph), "FAULT")
            self.assertEqual(world_hash(b.data), before, "partial state at phase %d" % ph)
            S.run_phase(b, 1, ph, b.data["revision"])  # retry succeeds
            self.assertEqual(S.cursor(b.data), (1, ph + 1) if ph < 12 else (2, 0))

    def test_handler_exception_in_staged_copy_does_not_leak(self):
        def bad(world, month):
            world["provinces"]["p1"]["cultivation"] = 99
            raise CoreError("BOOM", "x")
        b = make_bus()
        before = world_hash(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 0, 0, {"freeze": bad}), "BOOM")
        self.assertEqual(world_hash(b.data), before)

    def test_invalid_staged_world_rejected_before_swap(self):
        def corrupt(world, month):
            world["provinces"]["p1"]["cultivation"] = 500
        b = make_bus()
        before = world_hash(b.data)
        self.assertEqual(code_of(S.run_phase, b, 1, 0, 0, {"freeze": corrupt}), "OUT_OF_RANGE")
        self.assertEqual(world_hash(b.data), before)

    def test_only_weather_phase_consumes_rng(self):
        b = make_bus()
        seed = b.data["rng_state"]
        S.run_phase(b, 1, 0, 0)
        self.assertEqual(b.data["rng_state"], seed)
        S.run_phase(b, 1, 1, 1)
        self.assertNotEqual(b.data["rng_state"], seed)
        after_weather = b.data["rng_state"]
        for ph in range(2, 13):
            S.run_phase(b, 1, ph, b.data["revision"])
        self.assertEqual(b.data["rng_state"], after_weather)

    def test_deterministic_across_runs(self):
        a, c = make_bus(), make_bus()
        for _ in range(3):
            S.run_month(a)
            S.run_month(c)
        self.assertEqual(world_hash(a.data), world_hash(c.data))
        self.assertNotEqual(world_hash(a.data), world_hash(make_bus(8).data))

    def test_commands_still_work_between_months(self):
        b = make_bus()
        S.run_month(b)
        cmd = {"request_id": "c1", "actor_officer_id": "o1", "expected_world_revision": b.data["revision"],
               "kind": "cultivate", "params": {"province_id": "p1"}}
        b.commit(cmd)
        self.assertEqual(S.cursor(b.data), (2, 0))
        S.run_month(b)
        self.assertEqual(S.cursor(b.data), (3, 0))


if __name__ == "__main__":
    unittest.main()
