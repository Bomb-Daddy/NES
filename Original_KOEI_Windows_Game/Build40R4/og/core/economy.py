"""ECON-R1 phase handlers implemented so far: March frozen tax base (phases 0, 3, 6), the September
farm/artisan credit (phase 3) with its single yearly payout (phase 6), disasters (phase 2), land/flood crews (phase 3),
March tax compliance and troop pay (phase 6), January growth (phase 8).

Applies to schema v2 worlds only; on a v1 world every handler here is a no-op.
Coefficients come from world2.FIXTURE_V1 and are SYNTHETIC, not Director-approved balance.
Each handler runs on the STAGED copy; any CoreError discards the whole phase (see scheduler.run_phase).
"""
from .fixed import CoreError, check_int
from .rng import randrange
from .world2 import FIXTURE_V1, FROZEN_KEYS, WORKER_KEYS, calendar_of

MARCH = 3
SEPTEMBER = 9
UNIT_MILLI = 1000


def _is_v2(world):
    return world["schema_version"] == 2


def h_freeze(world, month):
    """Phase 0: capture the March tax base BEFORE any Wealth change this month."""
    if not _is_v2(world):
        return
    year, moy = calendar_of(world, month)
    if moy != MARCH:
        return
    for pid in sorted(world["provinces"]):
        p = world["provinces"][pid]
        if p["march_tax_frozen"]:
            raise CoreError("BAD_SNAPSHOT", "snapshot already present at freeze: " + pid)
        p["march_tax_frozen"] = {
            "year": year, "month": moy,
            "eligible_civilians": sum(p["workers"][k] for k in WORKER_KEYS),
            "wealth_milli": p["wealth_milli"], "support_ticks": p["support_ticks"],
            "tax_rate_percent": p["tax_rate_percent"],
        }


def h_civilian(world, month):
    """Phase 3: synthetic Wealth change (every month). Does not touch the frozen snapshot."""
    if not _is_v2(world):
        return
    year, moy = calendar_of(world, month)
    for pid in sorted(world["provinces"]):
        p = world["provinces"][pid]
        p["wealth_milli"] = min(FIXTURE_V1["wealth_max_milli"],
                                p["wealth_milli"] + FIXTURE_V1["wealth_delta_milli_per_month"])
        if moy == SEPTEMBER:  # current-month work accrues here, before the phase-6 payout
            per_farmer = (FIXTURE_V1["farm_credit_base_milli_per_farmer"]
                          + FIXTURE_V1["farm_credit_milli_per_cultivation_point_per_farmer"] * p["cultivation"])
            p["farm_credit_milli"] += p["workers"]["farmers"] * per_farmer
            p["artisan_credit_milli"] += p["workers"]["artisans"] * FIXTURE_V1["artisan_credit_milli_per_artisan"]
        # Crew work lands AFTER the September credit so this month's gain only affects later months.
        _crew(p, "land", "cultivation", "land_credit_milli")
        _crew(p, "flood", "flood_control", "flood_credit_milli")


def _crew(p, bucket, value_key, credit_key):
    """Workers in `bucket` accrue progress; every 1000 progress is +1 to `value_key`, capped; surplus at the cap is discarded.
    Crews never change support."""
    credit = p[credit_key] + p["workers"][bucket] * FIXTURE_V1["crew_progress_milli_per_worker"]
    gain, rest = divmod(credit, UNIT_MILLI)
    value = min(FIXTURE_V1["crew_value_max"], p[value_key] + gain)
    if value >= FIXTURE_V1["crew_value_max"]:
        rest = 0
    p[value_key] = value
    p[credit_key] = rest


def h_fiscal(world, month):
    """Phase 6: collect March tax from the FROZEN copy only; never reads live Wealth, rate or workers."""
    if not _is_v2(world):
        return
    year, moy = calendar_of(world, month)
    if moy == SEPTEMBER:
        _pay_september(world, year)
        return
    if moy != MARCH:
        return
    ids = {e["id"] for e in world["ledger"]}
    for pid in sorted(world["provinces"]):
        p = world["provinces"][pid]
        fz = p["march_tax_frozen"]
        if not fz:
            raise CoreError("MISSING_SNAPSHOT", pid)
        if set(fz) != FROZEN_KEYS or fz["year"] != year or fz["month"] != MARCH:
            raise CoreError("BAD_SNAPSHOT", pid)
        tid = "tax:%s:%d" % (pid, year)
        if tid in ids:
            raise CoreError("DUPLICATE_TAX", tid)
        for k in FROZEN_KEYS:
            check_int(fz[k], 0, 1 << 62, "frozen." + k)
        points = fz["support_ticks"] // FIXTURE_V1["support_ticks_per_point"]
        compliance = FIXTURE_V1["tax_compliance_base_percent"] + points // FIXTURE_V1["support_points_per_compliance_percent"]
        amount = (fz["eligible_civilians"] * fz["wealth_milli"] * fz["tax_rate_percent"]
                  * FIXTURE_V1["tax_cents_per_civilian_per_wealth_point_at_100pct"] * compliance) // (1000 * 100 * 100)
        world["ledger"].append({"seq": len(world["ledger"]) + 1, "id": tid, "kind": "TaxCollected",
                                "province_id": pid, "year": year, "amount_cents": amount})
        ids.add(tid)
        p["treasury_gold_cents"] += amount
        # Support follows the rate that was actually collected (the frozen one): below the fair rate raises it,
        # above lowers it, clamped to the validator range.
        delta = (FIXTURE_V1["support_fair_tax_percent"] - fz["tax_rate_percent"]) * FIXTURE_V1["support_ticks_per_tax_point"]
        p["support_ticks"] = max(0, min(FIXTURE_V1["support_max_ticks"], p["support_ticks"] + delta))
        p["march_tax_frozen"] = {}
        _pay_troops(world, pid, year)


def _event(world, pid, year, month, kind, tag, amount):
    world["event_ledger"].append({"seq": len(world["event_ledger"]) + 1, "id": "%s:%s:%d:%02d" % (tag, pid, year, month),
                                  "kind": kind, "province_id": pid, "year": year, "month": month, "amount": amount})


def _pay_troops(world, pid, year):
    """March, after tax: 1 gold per troop from the province treasury. Troops that cannot be paid leave and return to
    `unassigned`; whole troops only (a partial gold amount pays nobody)."""
    p = world["provinces"][pid]
    pay = FIXTURE_V1["troop_pay_cents"]
    paid = min(p["troops"], p["treasury_gold_cents"] // pay)
    left = p["troops"] - paid
    p["treasury_gold_cents"] -= paid * pay
    p["troops"] = paid
    if left:
        p["workers"]["unassigned"] += left
        world["event_ledger"].append({"seq": len(world["event_ledger"]) + 1, "id": "troopsleft:%s:%d" % (pid, year),
                                      "kind": "TroopsLeft",
                                      "province_id": pid, "year": year, "month": MARCH, "amount": left})


def _lose_workers(p, percent, severity):
    """Each worker bucket loses n * percent * severity // 10000 (floor). Returns the total lost."""
    lost = 0
    for k in WORKER_KEYS:
        n = p["workers"][k] * percent * severity // 10000
        p["workers"][k] -= n
        lost += n
    return lost


def h_disaster(world, month):
    """Phase 2. Per province in sorted order: a flood check (June..August only), then a disease check (any month).
    One randrange(1000) draw per check. PLACEHOLDER rates; Romance II's rates are not established."""
    if not _is_v2(world):
        return
    year, moy = calendar_of(world, month)
    ids = {e["id"] for e in world["event_ledger"]}
    for pid in sorted(world["provinces"]):
        p = world["provinces"][pid]
        flood_season = moy in FIXTURE_V1["flood_months"]
        fid = "flood:%s:%d:%02d" % (pid, year, moy)
        did = "disease:%s:%d:%02d" % (pid, year, moy)
        if (flood_season and fid in ids) or did in ids:
            raise CoreError("DUPLICATE_EVENT", pid)
        if flood_season:
            chance = max(FIXTURE_V1["flood_min_chance_permille"], p["cultivation"] * (100 - p["flood_control"]) // 100)
            if randrange(world, 1000) < chance:
                sev = (200 - p["flood_control"]) // 2
                p["farm_credit_milli"] -= p["farm_credit_milli"] * FIXTURE_V1["flood_credit_loss_percent"] * sev // 10000
                lost = _lose_workers(p, FIXTURE_V1["flood_worker_loss_percent"], sev)
                p["flood_control"] = max(0, p["flood_control"] - FIXTURE_V1["flood_control_loss"] * sev // 100)
                p["support_ticks"] = max(0, p["support_ticks"] - FIXTURE_V1["flood_support_loss_ticks"] * sev // 100)
                _event(world, pid, year, moy, "Flood", "flood", lost)
                ids.add(fid)
        if randrange(world, 1000) < FIXTURE_V1["disease_chance_permille"]:
            lost = _lose_workers(p, FIXTURE_V1["disease_worker_loss_percent"], 100)
            p["support_ticks"] = max(0, p["support_ticks"] - FIXTURE_V1["disease_support_loss_ticks"])
            _event(world, pid, year, moy, "Disease", "disease", lost)
            ids.add(did)


def battle_food_cost(troops, days):
    """Rice for troops fighting `days` days: 1 per troop per 30 days, rounded up. Used only when a battle exists
    (no battle system yet; nothing in the monthly phases calls this)."""
    per = FIXTURE_V1["battle_days_per_ration"]
    return (troops * days + per - 1) // per


def consume_battle_food(world, pid, days):
    p = world["provinces"][pid]
    cost = battle_food_cost(p["troops"], days)
    if p["food_units"] < cost:
        raise CoreError("NOT_ENOUGH_FOOD", "need %d have %d" % (cost, p["food_units"]))
    p["food_units"] -= cost
    return cost


def h_demographics(world, month):
    """Phase 8: ONE yearly civilian growth step in the growth month (January). New people are added to `unassigned`
    so they can be allocated by command; floor division, no remainder carried. No other bucket changes."""
    if not _is_v2(world):
        return
    year, moy = calendar_of(world, month)
    if moy != FIXTURE_V1["pop_growth_month"]:
        return
    for pid in sorted(world["provinces"]):
        p = world["provinces"][pid]
        civilians = sum(p["workers"][k] for k in WORKER_KEYS)
        p["workers"]["unassigned"] += civilians * FIXTURE_V1["pop_growth_percent_per_year"] // 100


def _stock_event(world, pid, year, tag, kind, good, units):
    world["stock_ledger"].append({"seq": len(world["stock_ledger"]) + 1, "id": "sep:%s:%d:%s" % (pid, year, tag),
                                  "kind": kind, "province_id": pid, "year": year, "good": good, "units": units})


def _pay_september(world, year):
    """Phase 6 (September): ONE payout per province per year. Whole units are paid, the sub-unit remainder stays in
    the credit balance, specialty above warehouse capacity is discarded with an auditable event, and nothing is bought."""
    ids = {e["id"] for e in world["stock_ledger"]}
    for pid in sorted(world["provinces"]):
        p = world["provinces"][pid]
        if p["last_september_settled_year"] == year or ("sep:%s:%d:food" % (pid, year)) in ids:
            raise CoreError("DUPLICATE_PAYOUT", "%s %d" % (pid, year))
        food, p["farm_credit_milli"] = divmod(p["farm_credit_milli"], UNIT_MILLI)
        spec, p["artisan_credit_milli"] = divmod(p["artisan_credit_milli"], UNIT_MILLI)
        accepted = min(spec, max(0, p["warehouse_capacity_units"] - p["specialty_units"]))
        overflow = spec - accepted
        p["food_units"] += food
        p["specialty_units"] += accepted
        p["last_september_settled_year"] = year
        _stock_event(world, pid, year, "food", "SeptemberPayout", "food", food)
        _stock_event(world, pid, year, "specialty", "SeptemberPayout", "specialty", accepted)
        if overflow:
            _stock_event(world, pid, year, "overflow", "OverflowDiscarded", "specialty", overflow)
