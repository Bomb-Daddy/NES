"""Disasters, food/goods distribution, recruiting, troop pay, land/flood crews, support-based tax compliance,
battle-food helper. Decisions Q1-Q9 (Dave, 2026-10-28). All coefficients are SYNTHETIC (og.core.world2.FIXTURE_V1);
no figure here comes from Romance II (its disaster rates are not established). Expected values are written out by hand,
not read from og.core.

Support scale used in the hand arithmetic: 50,000 ticks = 1 support point; ceiling 5,000,000 ticks."""
import unittest
from unittest import mock

from og.core import economy as E
from og.core import scheduler as S
from og.core import world2 as W2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.world import add_officer, add_province, canonical_bytes, new_world, validate_world, world_hash

BASE_WORKERS = {"farmers": 1000, "town": 300, "artisans": 200, "metalworkers": 100, "resources": 100,
                "unassigned": 250, "land": 50, "flood": 0}


def code_of(fn, *a, **k):
    try:
        fn(*a, **k)
    except CoreError as e:
        return e.code
    return None


def make(start_month=1, provinces=("p1",), cult=50, flood_control=0, workers=None, support=1000000, rate=10,
         bp=10, **fields):
    w = new_world(7)
    for pid in provinces:
        add_province(w, pid, cult)
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50), bp=bp)
    w = W2.migrate_v1_to_v2(w, start_year=1, start_month=start_month)
    for pid in provinces:
        p = w["provinces"][pid]
        p["workers"] = dict(workers or BASE_WORKERS)
        p["flood_control"] = flood_control
        p["support_ticks"] = support
        p["tax_rate_percent"] = rate
        p["wealth_milli"] = 40000
        p.update(fields)
    validate_world(w)
    return w


def scripted(values):
    left = list(values)

    def fake(world, n):
        assert n == 1000, n
        return left.pop(0)
    fake.left = left
    return fake


def run_disaster(world, month, rolls):
    fake = scripted(rolls)
    with mock.patch("og.core.economy.randrange", side_effect=fake):
        E.h_disaster(world, month)
    assert fake.left == [], "unused rolls %r" % (fake.left,)


def quiet_month(b):
    """Run one month with every disaster roll forced to 999 (never a hit), so non-disaster expectations are exact."""
    with mock.patch("og.core.economy.randrange", side_effect=lambda w, n: 999):
        S.run_month(b)


def cmd(rid, rev, kind, **params):
    return {"request_id": rid, "actor_officer_id": "o1", "expected_world_revision": rev, "kind": kind, "params": params}


class Schema(unittest.TestCase):
    def test_migration_gives_new_fields_zeroed(self):
        w1 = new_world(7)
        add_province(w1, "p1", 10)
        w = W2.migrate_v1_to_v2(w1)
        p = w["provinces"]["p1"]
        self.assertEqual((p["troops"], p["flood_control"], p["land_credit_milli"], p["flood_credit_milli"]), (0, 0, 0, 0))
        self.assertEqual((p["workers"]["land"], p["workers"]["flood"]), (0, 0))
        self.assertEqual(w["event_ledger"], [])

    def test_bad_values_rejected(self):
        cases = [
            (lambda w: w["provinces"]["p1"].__setitem__("troops", -1), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["p1"].__setitem__("flood_control", 101), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["p1"].__setitem__("land_credit_milli", -1), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["p1"].__setitem__("flood_credit_milli", -1), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["p1"]["workers"].__setitem__("land", -1), "OUT_OF_RANGE"),
            (lambda w: w["provinces"]["p1"]["workers"].pop("flood"), "BAD_SHAPE"),
            (lambda w: w.pop("event_ledger"), "BAD_SHAPE"),
            (lambda w: w["event_ledger"].append({"seq": 2, "id": "a", "kind": "Flood", "province_id": "p1", "year": 1, "month": 7, "amount": 0}), "BAD_SHAPE"),
            (lambda w: w["event_ledger"].append({"seq": 1, "id": "a", "kind": "Meteor", "province_id": "p1", "year": 1, "month": 7, "amount": 0}), "BAD_SHAPE"),
            (lambda w: w["event_ledger"].append({"seq": 1, "id": "a", "kind": "Flood", "province_id": "zz", "year": 1, "month": 7, "amount": 0}), "BAD_SHAPE"),
            (lambda w: w["event_ledger"].extend([
                {"seq": 1, "id": "a", "kind": "Flood", "province_id": "p1", "year": 1, "month": 7, "amount": 0},
                {"seq": 2, "id": "a", "kind": "Disease", "province_id": "p1", "year": 1, "month": 7, "amount": 0}]), "BAD_SHAPE"),
            (lambda w: w["event_ledger"].append({"seq": 1, "id": "a", "kind": "Flood", "province_id": "p1", "year": 1, "month": 13, "amount": 0}), "OUT_OF_RANGE"),
            (lambda w: w["event_ledger"].append({"seq": 1, "id": "a", "kind": "Flood", "province_id": "p1", "year": 1, "month": 7, "amount": -1}), "OUT_OF_RANGE"),
        ]
        for mutate, code in cases:
            w = make()
            mutate(w)
            self.assertEqual(code_of(validate_world, w), code)


class Flood(unittest.TestCase):
    def test_flood_at_zero_protection_full_damage(self):
        w = make(farm_credit_milli=20000)
        run_disaster(w, 7, [49, 999])  # chance = max(5, 50*100//100) = 50 per mille; 49 < 50 hits; disease 999 misses
        p = w["provinces"]["p1"]
        # severity 100%: each bucket loses n*1*100//10000 = n//100
        self.assertEqual(p["workers"], {"farmers": 990, "town": 297, "artisans": 198, "metalworkers": 99,
                                        "resources": 99, "unassigned": 248, "land": 50, "flood": 0})
        self.assertEqual(p["farm_credit_milli"], 14000)  # 30% of 20000 = 6000 lost
        self.assertEqual(p["flood_control"], 0)  # 10 lost, floored at 0
        self.assertEqual(p["support_ticks"], 750000)  # 250000 lost
        self.assertEqual(w["event_ledger"], [{"seq": 1, "id": "flood:p1:1:07", "kind": "Flood", "province_id": "p1",
                                              "year": 1, "month": 7, "amount": 19}])

    def test_chance_boundary_roll_equal_to_chance_misses(self):
        w = make()
        before = canonical_bytes(w)
        run_disaster(w, 7, [50, 999])
        self.assertEqual(canonical_bytes(w), before)

    def test_protection_lowers_chance_and_severity(self):
        w = make(flood_control=60, farm_credit_milli=20000)
        run_disaster(w, 7, [19, 999])  # chance = max(5, 50*40//100) = 20; 19 hits
        p = w["provinces"]["p1"]
        # severity (200-60)//2 = 70%: n*70//10000
        self.assertEqual(p["workers"], {"farmers": 993, "town": 298, "artisans": 199, "metalworkers": 100,
                                        "resources": 100, "unassigned": 249, "land": 50, "flood": 0})
        self.assertEqual(p["farm_credit_milli"], 15800)  # 20000*30*70//10000 = 4200 lost
        self.assertEqual(p["flood_control"], 53)  # 10*70//100 = 7 lost
        self.assertEqual(p["support_ticks"], 825000)  # 250000*70//100 = 175000 lost
        self.assertEqual(w["event_ledger"][0]["amount"], 11)  # 7+2+1+0+0+1

    def test_chance_boundary_with_protection(self):
        w = make(flood_control=60)
        before = canonical_bytes(w)
        run_disaster(w, 7, [20, 999])
        self.assertEqual(canonical_bytes(w), before)

    def test_floor_chance_never_zero(self):
        hit = make(cult=0, flood_control=100)
        run_disaster(hit, 7, [4, 999])  # chance = max(5, 0) = 5; 4 hits
        self.assertEqual(len(hit["event_ledger"]), 1)
        self.assertEqual(hit["provinces"]["p1"]["flood_control"], 95)  # severity 50% -> 5 lost
        self.assertEqual(hit["provinces"]["p1"]["support_ticks"], 875000)  # 125000 lost
        miss = make(cult=0, flood_control=100)
        run_disaster(miss, 7, [5, 999])
        self.assertEqual(miss["event_ledger"], [])

    def test_flood_months_are_june_july_august_only(self):
        for month, draws in ((5, 1), (6, 2), (7, 2), (8, 2), (9, 1)):
            w = make()
            rolls = [999] * draws
            run_disaster(w, month, rolls)  # asserts every scripted roll was consumed exactly
            self.assertEqual(w["event_ledger"], [], month)

    def test_flood_never_hits_outside_season_even_with_hit_roll(self):
        w = make()
        before = canonical_bytes(w)
        run_disaster(w, 9, [0])  # one draw only (disease, chance 5): 0 would hit disease, so use a non-hit province
        # roll 0 < 5 is a disease hit, never a flood hit
        self.assertEqual([e["kind"] for e in w["event_ledger"]], ["Disease"])
        self.assertNotEqual(canonical_bytes(w), before)

    def test_flood_ignores_troops_and_other_provinces(self):
        w = make(provinces=("p1", "p2"), troops=40)
        run_disaster(w, 7, [49, 999, 999, 999])
        self.assertEqual(w["provinces"]["p1"]["troops"], 40)
        self.assertEqual(w["provinces"]["p2"]["workers"], BASE_WORKERS)
        self.assertEqual(w["provinces"]["p2"]["support_ticks"], 1000000)
        self.assertEqual(len(w["event_ledger"]), 1)

    def test_provinces_draw_in_sorted_order_flood_then_disease(self):
        w = make(provinces=("p2", "p1"))
        run_disaster(w, 7, [999, 999, 49, 999])  # p1 first: miss, miss; p2: flood hit, disease miss
        self.assertEqual([e["province_id"] for e in w["event_ledger"]], ["p2"])

    def test_flood_credit_loss_is_one_time(self):
        w = make(farm_credit_milli=20000)
        run_disaster(w, 7, [49, 999])
        run_disaster(w, 8, [999, 999])  # August, no hit: credit is not damaged again
        self.assertEqual(w["provinces"]["p1"]["farm_credit_milli"], 14000)

    def test_support_floors_at_zero(self):
        w = make(support=100000)
        run_disaster(w, 7, [49, 999])
        self.assertEqual(w["provinces"]["p1"]["support_ticks"], 0)

    def test_same_month_repeat_is_rejected(self):
        w = make()
        run_disaster(w, 7, [49, 999])
        before = canonical_bytes(w)
        fake = scripted([49, 999])
        with mock.patch("og.core.economy.randrange", side_effect=fake):
            self.assertEqual(code_of(E.h_disaster, w, 7), "DUPLICATE_EVENT")


class Disease(unittest.TestCase):
    def test_disease_any_month_two_percent_no_severity_scaling(self):
        w = make(flood_control=100, farm_credit_milli=20000)
        run_disaster(w, 1, [4])  # January: one draw (disease, 5 per mille); 4 hits
        p = w["provinces"]["p1"]
        self.assertEqual(p["workers"], {"farmers": 980, "town": 294, "artisans": 196, "metalworkers": 98,
                                        "resources": 98, "unassigned": 245, "land": 49, "flood": 0})
        self.assertEqual(p["support_ticks"], 900000)  # 100000 lost
        self.assertEqual(p["farm_credit_milli"], 20000)  # disease never touches stored harvest
        self.assertEqual(p["flood_control"], 100)
        self.assertEqual(w["event_ledger"], [{"seq": 1, "id": "disease:p1:1:01", "kind": "Disease", "province_id": "p1",
                                              "year": 1, "month": 1, "amount": 40}])  # 20+6+4+2+2+5+1

    def test_disease_boundary(self):
        w = make()
        before = canonical_bytes(w)
        run_disaster(w, 1, [5])
        self.assertEqual(canonical_bytes(w), before)

    def test_flood_then_disease_same_month_use_post_flood_workers(self):
        w = make(farm_credit_milli=20000)
        run_disaster(w, 7, [49, 0])
        p = w["provinces"]["p1"]
        # flood leaves farmers 990, then disease 2% of 990 = 19
        self.assertEqual(p["workers"]["farmers"], 971)
        self.assertEqual([e["id"] for e in w["event_ledger"]], ["flood:p1:1:07", "disease:p1:1:07"])
        self.assertEqual([e["seq"] for e in w["event_ledger"]], [1, 2])
        self.assertEqual(p["support_ticks"], 650000)  # 1,000,000 - 250,000 - 100,000

    def test_v1_world_is_untouched(self):
        w = new_world(7)
        add_province(w, "p1", 50)
        before = canonical_bytes(w)
        E.h_disaster(w, 7)  # must not draw or change anything
        self.assertEqual(canonical_bytes(w), before)


class DisasterPhase(unittest.TestCase):
    def test_phase_two_runs_it_and_replay_is_deterministic(self):
        def run():
            b = Bus(make(start_month=7, farm_credit_milli=20000))
            fake = scripted([49, 999])
            with mock.patch("og.core.economy.randrange", side_effect=fake):
                S.run_month(b)
            return b

        a, b = run(), run()
        self.assertEqual(world_hash(a.data), world_hash(b.data))
        self.assertEqual(a.data["event_ledger"][0]["id"], "flood:p1:1:07")
        self.assertEqual(a.data["provinces"]["p1"]["support_ticks"], 750000)

    def test_real_rng_replay_same_seed_same_world(self):
        def run():
            b = Bus(make(start_month=1))
            for _ in range(24):
                S.run_month(b)
            return b

        self.assertEqual(world_hash(run().data), world_hash(run().data))

    def test_disaster_is_no_longer_a_placeholder(self):
        self.assertNotIn("disaster", S.PLACEHOLDER_HANDLERS_REMAINING)
        self.assertIs(S.PLACEHOLDER_HANDLERS["disaster"], E.h_disaster)


class DistributeFood(unittest.TestCase):
    def bus(self, **kw):
        return Bus(make(provinces=("p1", "p2"), food_units=500, specialty_units=50, **kw))

    def test_spends_food_and_support_gain_shrinks_near_ceiling(self):
        b = self.bus()
        b.commit(cmd("a", 0, "distribute_food", province_id="p1", units=100))
        p = b.data["provinces"]["p1"]
        # 100 * 200 * (5,000,000 - 1,000,000) // 5,000,000 = 16,000
        self.assertEqual((p["food_units"], p["support_ticks"]), (400, 1016000))
        self.assertEqual(b.data["officers"]["o1"]["bp"], 9)
        b.commit(cmd("b", 1, "distribute_food", province_id="p1", units=100))
        # second: 20,000 * (5,000,000 - 1,016,000) // 5,000,000 = 15,936
        self.assertEqual(p["support_ticks"] if False else b.data["provinces"]["p1"]["support_ticks"], 1031936)

    def test_at_ceiling_gain_is_zero_but_food_is_spent(self):
        b = Bus(make(food_units=500, support=5000000))
        b.commit(cmd("a", 0, "distribute_food", province_id="p1", units=10))
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["food_units"], p["support_ticks"]), (490, 5000000))

    def test_gain_never_pushes_support_past_the_ceiling(self):
        # Director-approved ceiling: at most 100 units per command (ECON-MASTER v2.1), so the
        # old 100000-unit overshoot case is now a rejection and the ceiling is checked near the top.
        b = Bus(make(food_units=100000, support=4999999))
        b.commit(cmd("a", 0, "distribute_food", province_id="p1", units=100))
        self.assertLessEqual(b.data["provinces"]["p1"]["support_ticks"], 5000000)
        b = Bus(make(food_units=100000))
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(b.commit, cmd("a", 0, "distribute_food", province_id="p1", units=100000)), "BAD_PARAMS")
        self.assertEqual(canonical_bytes(b.data), before)

    def test_other_province_untouched(self):
        b = self.bus()
        b.commit(cmd("a", 0, "distribute_food", province_id="p1", units=100))
        q = b.data["provinces"]["p2"]
        self.assertEqual((q["food_units"], q["support_ticks"]), (500, 1000000))

    def test_rejections_leave_world_byte_identical(self):
        cases = [
            (dict(province_id="p1", units=501), "BAD_PARAMS"),
            (dict(province_id="p1", units=101), "BAD_PARAMS"),
            (dict(province_id="p1", units=0), "BAD_PARAMS"),
            (dict(province_id="p1", units=-3), "BAD_PARAMS"),
            (dict(province_id="p1", units=True), "NOT_INT"),
            (dict(province_id="p1"), "BAD_PARAMS"),
            (dict(province_id="p1", units=1, extra=1), "BAD_PARAMS"),
            (dict(province_id="zz", units=1), "UNKNOWN_PROVINCE"),
            (dict(province_id="p2", units=1), "WRONG_PROVINCE"),
        ]
        for params, code in cases:
            b = self.bus()
            before = canonical_bytes(b.data)
            self.assertEqual(code_of(b.commit, cmd("x", 0, "distribute_food", **params)), code, params)
            self.assertEqual(canonical_bytes(b.data), before, params)

    def test_not_enough_food_rejected_without_mutation(self):
        b = Bus(make(food_units=50))
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(b.commit, cmd("a", 0, "distribute_food", province_id="p1", units=60)), "NOT_ENOUGH_FOOD")
        self.assertEqual(canonical_bytes(b.data), before)

    def test_no_bp_rejected(self):
        b = Bus(make(food_units=500, bp=0))
        before = canonical_bytes(b.data)
        self.assertEqual(code_of(b.commit, cmd("a", 0, "distribute_food", province_id="p1", units=1)), "NO_BP")
        self.assertEqual(canonical_bytes(b.data), before)

    def test_rejected_on_v1_world(self):
        w = new_world(7)
        add_province(w, "p1", 10)
        add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
        self.assertEqual(code_of(Bus(w).commit, cmd("a", 0, "distribute_food", province_id="p1", units=1)), "BAD_VERSION")


class DistributeGoods(unittest.TestCase):
    def test_spends_goods_and_gain_is_three_times_food_rate(self):
        b = Bus(make(food_units=500, specialty_units=50))
        b.commit(cmd("a", 0, "distribute_goods", province_id="p1", units=10))
        p = b.data["provinces"]["p1"]
        # 10 * 600 * 4,000,000 // 5,000,000 = 4,800
        self.assertEqual((p["specialty_units"], p["food_units"], p["support_ticks"]), (40, 500, 1004800))
        self.assertEqual(b.data["officers"]["o1"]["bp"], 9)

    def test_rejections(self):
        cases = [
            (dict(province_id="p1", units=51), "NOT_ENOUGH_GOODS"),
            (dict(province_id="p1", units=0), "BAD_PARAMS"),
            (dict(province_id="p1", units=1, extra=1), "BAD_PARAMS"),
            (dict(province_id="zz", units=1), "UNKNOWN_PROVINCE"),
        ]
        for params, code in cases:
            b = Bus(make(specialty_units=50))
            before = canonical_bytes(b.data)
            self.assertEqual(code_of(b.commit, cmd("x", 0, "distribute_goods", **params)), code, params)
            self.assertEqual(canonical_bytes(b.data), before, params)


class Recruit(unittest.TestCase):
    def test_moves_unassigned_to_troops_costs_support_and_bp(self):
        b = Bus(make())
        b.commit(cmd("a", 0, "recruit", province_id="p1", count=5))
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["troops"], p["workers"]["unassigned"]), (5, 245))
        self.assertEqual(p["support_ticks"], 1000000 - 5 * 500)
        self.assertEqual(b.data["officers"]["o1"]["bp"], 9)

    def test_support_floors_at_zero(self):
        b = Bus(make(support=1000))
        b.commit(cmd("a", 0, "recruit", province_id="p1", count=5))
        self.assertEqual(b.data["provinces"]["p1"]["support_ticks"], 0)

    def test_rejections(self):
        cases = [
            (dict(province_id="p1", count=251), "NOT_ENOUGH_WORKERS"),
            (dict(province_id="p1", count=0), "BAD_PARAMS"),
            (dict(province_id="p1", count=1, extra=1), "BAD_PARAMS"),
            (dict(province_id="zz", count=1), "UNKNOWN_PROVINCE"),
        ]
        for params, code in cases:
            b = Bus(make())
            before = canonical_bytes(b.data)
            self.assertEqual(code_of(b.commit, cmd("x", 0, "recruit", **params)), code, params)
            self.assertEqual(canonical_bytes(b.data), before, params)

    def test_only_unassigned_workers_can_be_recruited(self):
        b = Bus(make(workers={"farmers": 100, "town": 0, "artisans": 0, "metalworkers": 0, "resources": 0,
                              "unassigned": 0, "land": 0, "flood": 0}))
        self.assertEqual(code_of(b.commit, cmd("a", 0, "recruit", province_id="p1", count=1)), "NOT_ENOUGH_WORKERS")


class TroopPay(unittest.TestCase):
    def march(self, **fields):
        b = Bus(make(start_month=3, rate=0, **fields))
        quiet_month(b)
        return b

    def test_full_pay_costs_one_gold_per_troop(self):
        b = self.march(troops=10, treasury_gold_cents=2500)
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["troops"], p["treasury_gold_cents"]), (10, 1500))
        self.assertEqual([e for e in b.data["event_ledger"] if e["kind"] == "TroopsLeft"], [])

    def test_unpaid_troops_leave_to_unassigned(self):
        b = self.march(troops=10, treasury_gold_cents=700)
        p = b.data["provinces"]["p1"]
        # 700 cents pays 7 troops; 3 leave
        self.assertEqual((p["troops"], p["treasury_gold_cents"], p["workers"]["unassigned"]), (7, 0, 253))
        ev = [e for e in b.data["event_ledger"] if e["kind"] == "TroopsLeft"]
        self.assertEqual(ev, [{"seq": 1, "id": "troopsleft:p1:1", "kind": "TroopsLeft", "province_id": "p1",
                               "year": 1, "month": 3, "amount": 3}])

    def test_partial_gold_below_one_troop_pays_nobody(self):
        b = self.march(troops=4, treasury_gold_cents=99)
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["troops"], p["treasury_gold_cents"], p["workers"]["unassigned"]), (0, 99, 254))

    def test_tax_collected_this_march_pays_the_troops(self):
        # rate 10: 1250 civilians (1000+300+200+100+100+250+50+0 = 2000?) computed below by hand
        b = Bus(make(start_month=3, rate=10, troops=10, treasury_gold_cents=0, support=5000000))
        quiet_month(b)
        p = b.data["provinces"]["p1"]
        # civilians 2000, wealth 40000, rate 10, base = 2000*40000*10*10 // 100000 = 80000; support 100 pts -> 100%
        self.assertEqual(p["treasury_gold_cents"], 80000 - 1000)
        self.assertEqual(p["troops"], 10)

    def test_no_troops_no_event_no_cost(self):
        b = self.march(troops=0, treasury_gold_cents=500)
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["troops"], p["treasury_gold_cents"]), (0, 500))
        self.assertEqual(b.data["event_ledger"], [])

    def test_not_charged_in_other_months(self):
        b = Bus(make(start_month=4, rate=0, troops=10, treasury_gold_cents=2500))
        quiet_month(b)
        self.assertEqual(b.data["provinces"]["p1"]["treasury_gold_cents"], 2500)

    def test_each_province_pays_from_its_own_treasury(self):
        b = Bus(make(start_month=3, rate=0, provinces=("p1", "p2"), troops=10))
        b.data["provinces"]["p1"]["treasury_gold_cents"] = 1000
        b.data["provinces"]["p2"]["treasury_gold_cents"] = 300
        quiet_month(b)
        self.assertEqual((b.data["provinces"]["p1"]["troops"], b.data["provinces"]["p2"]["troops"]), (10, 3))


class TaxCompliance(unittest.TestCase):
    def tax(self, support):
        b = Bus(make(start_month=3, rate=10, support=support, treasury_gold_cents=0))
        quiet_month(b)
        return b.data["provinces"]["p1"]["treasury_gold_cents"]

    # civilians 2000, wealth 40000, rate 10: base = 2000*40000*10*10 // 100000 = 80000 cents at 100%
    def test_zero_support_collects_80_percent(self):
        self.assertEqual(self.tax(0), 64000)

    def test_half_support_collects_90_percent(self):
        self.assertEqual(self.tax(2500000), 72000)

    def test_full_support_collects_everything(self):
        self.assertEqual(self.tax(5000000), 80000)

    def test_step_is_one_percent_per_five_support_points(self):
        self.assertEqual(self.tax(249999), 64000)  # 4 points
        self.assertEqual(self.tax(250000), 64800)  # 5 points -> 81%
        self.assertEqual(self.tax(299999), 64800)  # 5 points
        self.assertEqual(self.tax(500000), 65600)  # 10 points -> 82%

    def test_uses_frozen_support_not_support_changed_after_freeze(self):
        b = Bus(make(start_month=3, rate=10, support=0, treasury_gold_cents=0))
        S.run_phase(b, 1, 0, 0)  # freeze
        b.data["provinces"]["p1"]["support_ticks"] = 5000000
        with mock.patch("og.core.economy.randrange", side_effect=lambda w, n: 999):
            for ph in range(1, 13):
                S.run_phase(b, 1, ph, b.data["revision"])
        self.assertEqual(b.data["provinces"]["p1"]["treasury_gold_cents"], 64000)


class Crews(unittest.TestCase):
    def month(self, start_month, **kw):
        b = Bus(make(start_month=start_month, rate=0, **kw))
        quiet_month(b)
        return b.data["provinces"]["p1"]

    def workers(self, land=0, flood=0):
        w = dict(BASE_WORKERS)
        w.update(land=land, flood=flood)
        return w

    def test_ten_progress_per_worker_per_month_thousand_makes_a_point(self):
        p = self.month(4, cult=10, workers=self.workers(land=250))
        self.assertEqual((p["cultivation"], p["land_credit_milli"]), (12, 500))  # 2500 milli -> +2, remainder 500

    def test_remainder_carries(self):
        b = Bus(make(start_month=4, rate=0, cult=10, workers=self.workers(land=250)))
        quiet_month(b)
        quiet_month(b)
        p = b.data["provinces"]["p1"]
        self.assertEqual((p["cultivation"], p["land_credit_milli"]), (15, 0))  # 500 + 2500 = 3000 -> +3

    def test_flood_crew_raises_flood_control_separately(self):
        p = self.month(4, cult=10, flood_control=40, workers=self.workers(land=0, flood=130))
        self.assertEqual((p["flood_control"], p["flood_credit_milli"], p["cultivation"]), (41, 300, 10))

    def test_cap_at_one_hundred_and_surplus_discarded(self):
        p = self.month(4, cult=99, workers=self.workers(land=250))
        self.assertEqual((p["cultivation"], p["land_credit_milli"]), (100, 0))
        q = self.month(4, cult=100, workers=self.workers(land=250), land_credit_milli=700)
        self.assertEqual((q["cultivation"], q["land_credit_milli"]), (100, 0))
        r = self.month(4, flood_control=100, workers=self.workers(flood=250))
        self.assertEqual((r["flood_control"], r["flood_credit_milli"]), (100, 0))

    def test_crews_never_change_support(self):
        p = self.month(4, workers=self.workers(land=500, flood=500))
        self.assertEqual(p["support_ticks"], 1000000)

    def test_artisans_and_production_never_change_support(self):
        # September payout month with artisans and farmers working: support untouched (only tax month, disasters,
        # recruiting and distribution commands move it)
        b = Bus(make(start_month=9, rate=0))
        before = b.data["provinces"]["p1"]["support_ticks"]
        with mock.patch("og.core.economy.randrange", side_effect=scripted([999])):
            S.run_month(b)
        self.assertEqual(b.data["provinces"]["p1"]["support_ticks"], before)
        self.assertGreater(b.data["provinces"]["p1"]["food_units"], 0)

    def test_september_credit_uses_cultivation_from_before_this_months_crew_gain(self):
        w = {"farmers": 100, "town": 0, "artisans": 0, "metalworkers": 0, "resources": 0, "unassigned": 0,
             "land": 1000, "flood": 0}
        b = Bus(make(start_month=9, rate=0, cult=10, workers=w))
        with mock.patch("og.core.economy.randrange", side_effect=scripted([999])):
            S.run_month(b)
        p = b.data["provinces"]["p1"]
        # farm credit per farmer 500 + 10*10 = 600 -> 60,000 milli = 60 food (not 70: gain of +10 lands after the credit)
        self.assertEqual((p["food_units"], p["cultivation"]), (60, 20))

    def test_allocation_into_crew_buckets(self):
        b = Bus(make())
        b.commit(cmd("a", 0, "allocate_workers", province_id="p1", source="unassigned", target="land", count=100))
        w = b.data["provinces"]["p1"]["workers"]
        self.assertEqual((w["unassigned"], w["land"]), (150, 150))


class BattleFood(unittest.TestCase):
    def test_cost_is_one_rice_per_troop_per_thirty_days_rounded_up(self):
        self.assertEqual(E.battle_food_cost(100, 30), 100)
        self.assertEqual(E.battle_food_cost(100, 15), 50)
        self.assertEqual(E.battle_food_cost(100, 1), 4)  # 3.33 rounds up
        self.assertEqual(E.battle_food_cost(0, 30), 0)
        self.assertEqual(E.battle_food_cost(100, 0), 0)

    def test_consume_deducts_province_food(self):
        w = make(troops=100, food_units=500)
        E.consume_battle_food(w, "p1", 30)
        self.assertEqual(w["provinces"]["p1"]["food_units"], 400)

    def test_not_enough_food_raises_and_changes_nothing(self):
        w = make(troops=100, food_units=99)
        before = canonical_bytes(w)
        self.assertEqual(code_of(E.consume_battle_food, w, "p1", 30), "NOT_ENOUGH_FOOD")
        self.assertEqual(canonical_bytes(w), before)

    def test_no_monthly_consumption_outside_battle(self):
        b = Bus(make(start_month=4, rate=0, troops=100, food_units=500, treasury_gold_cents=10000))
        quiet_month(b)
        self.assertEqual(b.data["provinces"]["p1"]["food_units"], 500)

    def test_civilians_never_starve(self):
        b = Bus(make(start_month=4, rate=0, food_units=0))
        quiet_month(b)
        self.assertEqual(sum(b.data["provinces"]["p1"]["workers"].values()), 2000)


if __name__ == "__main__":
    unittest.main()
