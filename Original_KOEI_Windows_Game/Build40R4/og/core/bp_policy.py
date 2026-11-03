"""Director decision 2026-10-29: Nobunaga's Ambition II BP scale.

Authoritative choice: max 200, initial BP = Politics, each monthly rollover
credit floor(Politics*30/100), saturated at 200. Action BP costs remain
independently provisional and need their own balance/authority gate.
"""
from .world import MAX_BP


def recovery_amount(politics):
    return politics * 30 // 100


def recover_next_month(world, month):
    """Credited at phase12 on transition to next turn; phase-marker ensures once/month.

    Pure arithmetic, no RNG; the command bus scheduler stages the entire state.
    """
    for oid in sorted(world["officers"]):
        officer = world["officers"][oid]
        officer["bp"] = min(MAX_BP, officer["bp"] + recovery_amount(officer["politics"]))
