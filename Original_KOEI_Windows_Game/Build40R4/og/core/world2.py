"""World schema v2 (ECON-R1 minimum state contract, SUP-ECON-0044 section 2).

Field names are REVERSIBLE ENGINEERING NAMES, not Director-approved balance. FIXTURE_V1 values are SYNTHETIC test
coefficients and must never be treated as approved rules. A v1 world is never mutated or silently migrated:
migrate_v1_to_v2 returns a new v2 world and is called explicitly.
"""
import copy

from .fixed import CoreError, check_int
from .rng import MASK

SCHEMA_VERSION_V2 = 2
BIG = 1 << 62

FIXTURE_V1 = {
    "label": "SYNTHETIC-NOT-APPROVED",
    "version": 1,
    "wealth_delta_milli_per_month": 1000,
    "wealth_max_milli": 100000,
    "tax_cents_per_civilian_per_wealth_point_at_100pct": 10,
    "farm_credit_base_milli_per_farmer": 500,
    "farm_credit_milli_per_cultivation_point_per_farmer": 10,
    "artisan_credit_milli_per_artisan": 2500,
    "pop_growth_percent_per_year": 2,
    "pop_growth_month": 1,
    "support_fair_tax_percent": 30,
    "support_ticks_per_tax_point": 500,
    "support_max_ticks": 5000000,
    "allocate_bp_cost": 1,
    # Disasters (phase 2). Flood: months June..August only. NOT from Romance II (its rates are not established).
    "flood_months": (6, 7, 8),
    "flood_min_chance_permille": 5,
    "flood_credit_loss_percent": 30,
    "flood_worker_loss_percent": 1,
    "flood_control_loss": 10,
    "flood_support_loss_ticks": 250000,
    "disease_chance_permille": 5,
    "disease_worker_loss_percent": 2,
    "disease_support_loss_ticks": 100000,
    # Distribution commands: support gain shrinks linearly to 0 as support nears support_max_ticks.
    "distribute_food_ticks_per_unit": 200,
    "distribute_goods_ticks_per_unit": 600,
    # Land / flood crews: progress per worker per month; 1000 progress = +1 point; both values cap at 100.
    "crew_progress_milli_per_worker": 10,
    "crew_value_max": 100,
    # Military.
    "recruit_support_loss_ticks_per_troop": 500,
    "troop_pay_cents": 100,
    "battle_days_per_ration": 30,
    # March tax compliance: 80% at zero support, +1 percent per 5 support points (50,000 ticks each), 100% at 100.
    "tax_compliance_base_percent": 80,
    "support_ticks_per_point": 50000,
    "support_points_per_compliance_percent": 5,
}

WORKER_KEYS = ("farmers", "town", "artisans", "metalworkers", "resources", "unassigned", "land", "flood")
FROZEN_KEYS = {"year", "month", "eligible_civilians", "wealth_milli", "support_ticks", "tax_rate_percent"}
PROVINCE_KEYS_V2 = {"cultivation", "wealth_milli", "support_ticks", "tax_rate_percent", "workers",
                    "treasury_gold_cents", "food_units", "march_tax_frozen", "farm_credit_milli",
                    "artisan_credit_milli", "last_september_settled_year", "specialty_units",
                    "warehouse_capacity_units", "troops", "flood_control", "land_credit_milli", "flood_credit_milli"}
EVENT_KEYS = {"seq", "id", "kind", "province_id", "year", "month", "amount"}
EVENT_KINDS = {"Flood", "Disease", "TroopsLeft"}
LEDGER_KEYS = {"seq", "id", "kind", "province_id", "year", "amount_cents"}
STOCK_KEYS = {"seq", "id", "kind", "province_id", "year", "good", "units"}
STOCK_KINDS = {"SeptemberPayout", "OverflowDiscarded"}
STOCK_GOODS = {"food", "specialty"}


def calendar_of(world, month):
    """Map scheduler month index (1-based) to (year, month_of_year 1..12)."""
    cal = world["calendar"]
    idx = (month - 1) + (cal["start_month"] - 1)
    return cal["start_year"] + idx // 12, idx % 12 + 1


def migrate_v1_to_v2(w1, start_year=1, start_month=1):
    """Explicit migration. Returns a NEW world; w1 is not modified. Defaults are zero (no approved tax default)."""
    check_int(start_year, 1, 1000000, "start_year")
    check_int(start_month, 1, 12, "start_month")
    if w1.get("schema_version") != 1:
        raise CoreError("BAD_VERSION", "migrate_v1_to_v2 needs a v1 world")
    w = copy.deepcopy(w1)
    w["schema_version"] = SCHEMA_VERSION_V2
    for p in w["provinces"].values():
        p.update({"wealth_milli": 0, "support_ticks": 0, "tax_rate_percent": 0,
                  "workers": {k: 0 for k in WORKER_KEYS}, "treasury_gold_cents": 0, "food_units": 0,
                  "march_tax_frozen": {}, "farm_credit_milli": 0, "artisan_credit_milli": 0,
                  "last_september_settled_year": 0, "specialty_units": 0, "warehouse_capacity_units": 0,
                  "troops": 0, "flood_control": 0, "land_credit_milli": 0, "flood_credit_milli": 0})
    w["calendar"] = {"start_year": start_year, "start_month": start_month}
    w["ledger"] = []
    w["stock_ledger"] = []
    w["event_ledger"] = []
    from .world import validate_world
    validate_world(w)
    return w


def validate_v2(d):
    from .world import TOP_KEYS, validate_world
    extended = d.get("balance_policy") in ("OG_1000_SLOW_GROWTH_V1_CANDIDATE","OG_1000_DIRECTOR_V2")
    expected = TOP_KEYS | {"calendar", "ledger", "stock_ledger", "event_ledger"}
    if extended: expected = expected | {"balance_policy"}
    if set(d) != expected:
        raise CoreError("BAD_SHAPE", "v2 top-level keys")
    provinces = d["provinces"]
    if not isinstance(provinces, dict):
        raise CoreError("BAD_SHAPE", "provinces")
    for pid, p in provinces.items():
        if not isinstance(p, dict) or set(p) != PROVINCE_KEYS_V2:
            raise CoreError("BAD_SHAPE", "v2 province " + pid)
    # Reuse every v1 invariant (revision/journal/processed_ids/officers/cultivation) on a v1-shaped view.
    view = {k: d[k] for k in TOP_KEYS}
    view["schema_version"] = 1
    if extended: view["balance_policy"] = d["balance_policy"]
    view["provinces"] = {pid: {"cultivation": p["cultivation"]} for pid, p in provinces.items()}
    validate_world(view)
    cal = d["calendar"]
    if not isinstance(cal, dict) or set(cal) != {"start_year", "start_month"}:
        raise CoreError("BAD_SHAPE", "calendar")
    check_int(cal["start_year"], 1, 1000000, "start_year")
    check_int(cal["start_month"], 1, 12, "start_month")
    for pid, p in provinces.items():
        check_int(p["wealth_milli"], 0, 1000000 if extended else 100000, "wealth_milli")
        check_int(p["support_ticks"], 0, 5000000, "support_ticks")
        check_int(p["tax_rate_percent"], 0, 100, "tax_rate_percent")
        w = p["workers"]
        if not isinstance(w, dict) or set(w) != set(WORKER_KEYS):
            raise CoreError("BAD_SHAPE", "workers " + pid)
        for k in WORKER_KEYS:
            check_int(w[k], 0, BIG, "workers." + k)
        check_int(p["treasury_gold_cents"], 0, BIG, "treasury_gold_cents")
        check_int(p["food_units"], 0, BIG, "food_units")
        check_int(p["farm_credit_milli"], 0, BIG, "farm_credit_milli")
        check_int(p["artisan_credit_milli"], 0, BIG, "artisan_credit_milli")
        check_int(p["last_september_settled_year"], 0, 1000000, "last_september_settled_year")
        check_int(p["specialty_units"], 0, BIG, "specialty_units")
        check_int(p["warehouse_capacity_units"], 0, BIG, "warehouse_capacity_units")
        check_int(p["troops"], 0, BIG, "troops")
        check_int(p["flood_control"], 0, 1000 if extended else 100, "flood_control")
        check_int(p["land_credit_milli"], 0, BIG, "land_credit_milli")
        check_int(p["flood_credit_milli"], 0, BIG, "flood_credit_milli")
        fz = p["march_tax_frozen"]
        if not isinstance(fz, dict) or (fz and set(fz) != FROZEN_KEYS):
            raise CoreError("BAD_SHAPE", "march_tax_frozen " + pid)
        if fz:
            check_int(fz["year"], 1, 1000000, "frozen.year")
            check_int(fz["month"], 1, 12, "frozen.month")
            check_int(fz["eligible_civilians"], 0, BIG, "frozen.eligible_civilians")
            check_int(fz["wealth_milli"], 0, 1000000 if extended else 100000, "frozen.wealth_milli")
            check_int(fz["support_ticks"], 0, 5000000, "frozen.support_ticks")
            check_int(fz["tax_rate_percent"], 0, 100, "frozen.tax_rate_percent")
    ledger = d["ledger"]
    if not isinstance(ledger, list):
        raise CoreError("BAD_SHAPE", "ledger")
    seen = set()
    for idx, ev in enumerate(ledger):
        if not isinstance(ev, dict) or set(ev) != LEDGER_KEYS or ev["seq"] != idx + 1:
            raise CoreError("BAD_SHAPE", "ledger event " + str(idx))
        if not isinstance(ev["id"], str) or ev["id"] in seen or ev["kind"] != "TaxCollected":
            raise CoreError("BAD_SHAPE", "ledger id/kind " + str(idx))
        seen.add(ev["id"])
        if ev["province_id"] not in provinces:
            raise CoreError("BAD_SHAPE", "ledger province " + str(idx))
        check_int(ev["year"], 1, 1000000, "ledger.year")
        check_int(ev["amount_cents"], 0, BIG, "ledger.amount_cents")
    stock = d["stock_ledger"]
    if not isinstance(stock, list):
        raise CoreError("BAD_SHAPE", "stock_ledger")
    seen = set()
    for idx, ev in enumerate(stock):
        if not isinstance(ev, dict) or set(ev) != STOCK_KEYS or ev["seq"] != idx + 1:
            raise CoreError("BAD_SHAPE", "stock event " + str(idx))
        if not isinstance(ev["id"], str) or ev["id"] in seen or ev["kind"] not in STOCK_KINDS or ev["good"] not in STOCK_GOODS:
            raise CoreError("BAD_SHAPE", "stock id/kind/good " + str(idx))
        seen.add(ev["id"])
        if ev["province_id"] not in provinces:
            raise CoreError("BAD_SHAPE", "stock province " + str(idx))
        check_int(ev["year"], 1, 1000000, "stock.year")
        check_int(ev["units"], 0, BIG, "stock.units")
    events = d["event_ledger"]
    if not isinstance(events, list):
        raise CoreError("BAD_SHAPE", "event_ledger")
    seen = set()
    for idx, ev in enumerate(events):
        if not isinstance(ev, dict) or set(ev) != EVENT_KEYS or ev["seq"] != idx + 1:
            raise CoreError("BAD_SHAPE", "event " + str(idx))
        if not isinstance(ev["id"], str) or ev["id"] in seen or ev["kind"] not in EVENT_KINDS:
            raise CoreError("BAD_SHAPE", "event id/kind " + str(idx))
        seen.add(ev["id"])
        if ev["province_id"] not in provinces:
            raise CoreError("BAD_SHAPE", "event province " + str(idx))
        check_int(ev["year"], 1, 1000000, "event.year")
        check_int(ev["month"], 1, 12, "event.month")
        check_int(ev["amount"], 0, BIG, "event.amount")
    check_int(d["rng_state"], 0, MASK, "rng_state")


# ---- Backward read of pre-Q schema-v2 saves (SUP-NOB-0046 task B) -------------------------------------------------
# Q extended v2 in place (workers gained land/flood; provinces gained troops/flood_control/land_credit_milli/
# flood_credit_milli; top level gained event_ledger). A save written before that has the same schema_version (2) but the
# older shape. upgrade_legacy_v2 recognises EXACTLY that older shape and returns a NEW current-shape world; anything else
# (including a half-upgraded or otherwise malformed v2 world) is returned unchanged so validate_v2 rejects it.
LEGACY_V2_WORKER_KEYS = frozenset(WORKER_KEYS) - {"land", "flood"}
LEGACY_V2_NEW_PROVINCE_KEYS = {"troops", "flood_control", "land_credit_milli", "flood_credit_milli"}
LEGACY_V2_PROVINCE_KEYS = frozenset(PROVINCE_KEYS_V2) - LEGACY_V2_NEW_PROVINCE_KEYS


def is_legacy_v2_shape(d):
    from .world import TOP_KEYS
    if not isinstance(d, dict) or d.get("schema_version") != SCHEMA_VERSION_V2:
        return False
    if set(d) != TOP_KEYS | {"calendar", "ledger", "stock_ledger"}:
        return False
    provinces = d.get("provinces")
    if not isinstance(provinces, dict):
        return False
    for p in provinces.values():
        if not isinstance(p, dict) or set(p) != LEGACY_V2_PROVINCE_KEYS:
            return False
        if not isinstance(p.get("workers"), dict) or set(p["workers"]) != LEGACY_V2_WORKER_KEYS:
            return False
    return True


def upgrade_legacy_v2(d):
    if not is_legacy_v2_shape(d):
        return d
    w = copy.deepcopy(d)
    for p in w["provinces"].values():
        p["workers"]["land"] = 0
        p["workers"]["flood"] = 0
        p["troops"] = 0
        p["flood_control"] = 0
        p["land_credit_milli"] = 0
        p["flood_credit_milli"] = 0
    w["event_ledger"] = []
    return w
