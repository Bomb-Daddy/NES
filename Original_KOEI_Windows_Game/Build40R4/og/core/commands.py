"""Command handlers. Each runs ONLY on a staged copy: handler(staged, actor_id, params) -> result dict.

PARAMS_V1 values are PROVISIONAL fixtures, not Director-approved balance numbers.
"""
from .fixed import CoreError, check_tree
from .world2 import FIXTURE_V1, WORKER_KEYS

def _cultivation_cap(d):
    return 1000 if d.get("balance_policy") in ("OG_1000_SLOW_GROWTH_V1_CANDIDATE","OG_1000_DIRECTOR_V2") else CULT_MAX
from .world import CULT_MAX, XP_MAX

PARAMS_V1 = {"cultivate_bp_cost": 2, "cultivate_step": 1, "cultivate_xp": 1}


def h_cultivate(d, actor, p):
    """MG1 single officer/BP/cultivation command against the same world."""
    check_tree(p)
    if set(p) != {"province_id"} or not isinstance(p["province_id"], str):
        raise CoreError("BAD_PARAMS", "cultivate takes exactly province_id")
    pid = p["province_id"]
    if pid not in d["provinces"]:
        raise CoreError("UNKNOWN_PROVINCE", pid)
    o = d["officers"][actor]
    if o["province_id"] != pid:
        raise CoreError("WRONG_PROVINCE", actor + " is not in " + pid)
    cost = PARAMS_V1["cultivate_bp_cost"]
    if o["bp"] < cost:
        raise CoreError("NO_BP", "need " + str(cost) + " have " + str(o["bp"]))
    prov = d["provinces"][pid]
    if prov["cultivation"] >= _cultivation_cap(d):
        raise CoreError("AT_CAP", pid)
    o["bp"] = o["bp"] - cost
    before = prov["cultivation"]
    prov["cultivation"] = min(_cultivation_cap(d), before + PARAMS_V1["cultivate_step"])
    o["xp"] = min(XP_MAX, o["xp"] + PARAMS_V1["cultivate_xp"])
    return {"province_id": pid, "cultivation_before": before, "cultivation_after": prov["cultivation"], "bp_after": o["bp"]}


def h_allocate_workers(d, actor, p):
    """A general moves `count` workers between two job buckets of a province he is in. Total is conserved."""
    check_tree(p)
    if d["schema_version"] != 2:
        raise CoreError("BAD_VERSION", "allocate_workers needs a v2 world")
    if set(p) != {"province_id", "source", "target", "count"} or not isinstance(p["province_id"], str):
        raise CoreError("BAD_PARAMS", "allocate_workers takes exactly province_id, source, target, count")
    if p["source"] not in WORKER_KEYS or p["target"] not in WORKER_KEYS or p["source"] == p["target"]:
        raise CoreError("BAD_PARAMS", "source/target must be two different worker buckets")
    if not isinstance(p["count"], int) or isinstance(p["count"], bool) or p["count"] < 1:
        raise CoreError("BAD_PARAMS", "count must be an int >= 1")
    pid = p["province_id"]
    o, cost = _officer_in(d, actor, pid)
    w = d["provinces"][pid]["workers"]
    if w[p["source"]] < p["count"]:
        raise CoreError("NOT_ENOUGH_WORKERS", "%s has %d" % (p["source"], w[p["source"]]))
    o["bp"] -= cost
    w[p["source"]] -= p["count"]
    w[p["target"]] += p["count"]
    return {"province_id": pid, "source_after": w[p["source"]], "target_after": w[p["target"]], "bp_after": o["bp"]}


def _officer_in(d, actor, pid):
    if pid not in d["provinces"]:
        raise CoreError("UNKNOWN_PROVINCE", pid)
    o = d["officers"][actor]
    if o["province_id"] != pid:
        raise CoreError("WRONG_PROVINCE", actor + " is not in " + pid)
    cost = FIXTURE_V1["allocate_bp_cost"]
    if o["bp"] < cost:
        raise CoreError("NO_BP", "need " + str(cost) + " have " + str(o["bp"]))
    return o, cost


def _check_amount(d, p, keys, amount_key):
    check_tree(p)
    if d["schema_version"] != 2:
        raise CoreError("BAD_VERSION", "command needs a v2 world")
    if set(p) != set(keys) or not isinstance(p["province_id"], str):
        raise CoreError("BAD_PARAMS", "takes exactly " + ", ".join(keys))
    if not isinstance(p[amount_key], int) or isinstance(p[amount_key], bool) or p[amount_key] < 1:
        raise CoreError("BAD_PARAMS", amount_key + " must be an int >= 1")


def _distribute(d, actor, p, stock_key, ticks_key, err):
    _check_amount(d, p, ("province_id", "units"), "units")
    if p["units"] > 100:  # same ceiling as the approved ECON-MASTER v2.1 override (hostile-QA fix 2026-11-02)
        raise CoreError("BAD_PARAMS", "Distribute maximum is 100 units")
    o, cost = _officer_in(d, actor, p["province_id"])
    prov = d["provinces"][p["province_id"]]
    if prov[stock_key] < p["units"]:
        raise CoreError(err, "%s has %d" % (stock_key, prov[stock_key]))
    top = FIXTURE_V1["support_max_ticks"]
    # Charisma scales the legal distribution effect (same 1000+2*charisma factor as the approved
    # override, normalised so charisma 50 is neutral). Hostile-QA fix 2026-11-02.
    gain = p["units"] * FIXTURE_V1[ticks_key] * (top - prov["support_ticks"]) // top
    gain = gain * (1000 + 2 * o["charisma"]) // 1100
    o["bp"] -= cost
    prov[stock_key] -= p["units"]
    prov["support_ticks"] = min(top, prov["support_ticks"] + gain)
    return {"province_id": p["province_id"], "support_gain_ticks": gain, "support_ticks_after": prov["support_ticks"],
            "bp_after": o["bp"]}


def h_distribute_food(d, actor, p):
    """Give food to the population: support rises with diminishing returns toward the ceiling."""
    return _distribute(d, actor, p, "food_units", "distribute_food_ticks_per_unit", "NOT_ENOUGH_FOOD")


def h_distribute_goods(d, actor, p):
    """Give artisan goods to the population (higher support per unit than food)."""
    return _distribute(d, actor, p, "specialty_units", "distribute_goods_ticks_per_unit", "NOT_ENOUGH_GOODS")


def h_recruit(d, actor, p):
    """Turn `count` unassigned workers into troops; each costs support."""
    _check_amount(d, p, ("province_id", "count"), "count")
    o, cost = _officer_in(d, actor, p["province_id"])
    prov = d["provinces"][p["province_id"]]
    if prov["workers"]["unassigned"] < p["count"]:
        raise CoreError("NOT_ENOUGH_WORKERS", "unassigned has %d" % prov["workers"]["unassigned"])
    o["bp"] -= cost
    prov["workers"]["unassigned"] -= p["count"]
    prov["troops"] += p["count"]
    prov["support_ticks"] = max(0, prov["support_ticks"] - p["count"] * FIXTURE_V1["recruit_support_loss_ticks_per_troop"])
    return {"province_id": p["province_id"], "troops_after": prov["troops"], "support_ticks_after": prov["support_ticks"],
            "bp_after": o["bp"]}


HANDLERS = {"cultivate": h_cultivate, "allocate_workers": h_allocate_workers, "distribute_food": h_distribute_food,
            "distribute_goods": h_distribute_goods, "recruit": h_recruit}
