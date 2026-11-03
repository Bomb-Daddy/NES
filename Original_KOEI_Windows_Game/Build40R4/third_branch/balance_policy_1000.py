"""Original third-branch experimental economic policy (never installed in NES Classic/DOS).
Every owned-only phase works on the same persistent world; no unowned government taxes.
This is a proposal subject to numerical QA and the Director's release authority.
"""
from og.core import economy_slow1000 as economy
from og.core import economy as original_economy
from og.core.world2 import WORKER_KEYS

POLICY_ID = 'OG_1000_SLOW_GROWTH_V1_CANDIDATE'
CIVILIANS_PER_MONTHLY_FOOD = 120
METAL_DIVISOR = 35
FORGE_DIVISOR = 25
EXTRA_TROOP_MONTHLY_WAGE_CENTS = 10

def _governed(original, owned):
    def handler(world, month):
        provinces = world['provinces']
        # Stage-only temporary view. Always restore all 41 before validation/commit.
        world['provinces'] = {pid: p for pid, p in provinces.items() if pid in owned}
        try: original(world, month)
        finally: world['provinces'] = provinces
    return handler

def handlers(owners):
    governed = frozenset(pid for pid, f in owners.items() if f is not None)
    return {name: _governed(fn, governed) for name, fn in (
        ('freeze', original_economy.h_freeze), ('civilian', economy.h_civilian),
        ('fiscal', economy.h_fiscal), ('disaster', economy.h_disaster),
        ('demographics', original_economy.h_demographics))}

def post_month(staged):
    """Finite resources consumed only from actual province stocks; no negative state."""
    for pid, owner in sorted(staged['owners'].items()):
        if owner is None: continue
        p = staged['core']['provinces'][pid]
        x = staged['extras'][pid]
        # Slower nondepleting pilot: partial replacement until source-linked mines are modeled.
        extracted = p['workers']['resources'] // METAL_DIVISOR
        x['metal'] += extracted
        forged = min(x['metal'], p['workers']['metalworkers']//FORGE_DIVISOR)
        x['metal'] -= forged
        x['arms'] += forged
        # Civilian subsistence is a real stock sink. Shortfall damages public support.
        civ = sum(p['workers'][k] for k in WORKER_KEYS)
        civilians_need = (civ + CIVILIANS_PER_MONTHLY_FOOD - 1)//CIVILIANS_PER_MONTHLY_FOOD
        troops_need = (p['troops']+59)//60
        need = civilians_need + troops_need
        taken = min(need, p['food_units'])
        p['food_units'] -= taken
        lack = need - taken
        if lack:
            p['support_ticks'] = max(0, p['support_ticks']-lack*3000)
        # Monthly soldier pay is additional to already-implemented annual wages.
        payroll = min(p['troops']*EXTRA_TROOP_MONTHLY_WAGE_CENTS,p['treasury_gold_cents'])
        p['treasury_gold_cents'] -= payroll
        if payroll < p['troops']*EXTRA_TROOP_MONTHLY_WAGE_CENTS:
            p['support_ticks'] = max(0, p['support_ticks']-1500)


def post_month_director_v2(staged):
    """Director-authorized post-month policy, separate from legacy saves.

    Civilians do not consume government stores. Metal does not deplete, but
    extraction requires an explicitly documented positive site designation.
    Unverified sites yield zero rather than inventing geography. No additional
    fiscal sinks or merchant reserves are added.
    """
    sites=staged.get('metal_deposits',{})
    for pid,owner in sorted(staged['owners'].items()):
        if owner is None:continue
        p=staged['core']['provinces'][pid];x=staged['extras'][pid]
        if sites.get(pid) is True:
            x['metal']+=p['workers']['resources']//METAL_DIVISOR
        forged=min(x['metal'],p['workers']['metalworkers']//FORGE_DIVISOR)
        x['metal']-=forged;x['arms']+=forged
        need=(p['troops']+59)//60
        taken=min(need,p['food_units'])
        p['food_units']-=taken
        # Troop ration shortage is not a civilian subsistence failure.
        # Do not add a new unapproved recurring military cash tax.
