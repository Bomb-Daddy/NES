"""Faithful original combat Q03 rice accounting reference; not an installed port."""
from __future__ import annotations
from dataclasses import dataclass
from typing import Sequence

@dataclass(frozen=True)
class FoodTick:
    rice_before100: int
    rice_after100: int
    counted_troops100: int
    consumed_rice100: int
    forced_exhaustion: bool

def consume_daily_rice(rice_before100: int, deployed_troops100: Sequence[int], *,
                       defending: bool, resident_troops100: Sequence[int] = ()) -> FoodTick:
    if type(rice_before100) is not int or not 0 <= rice_before100 <= 3_000_000:
        raise ValueError("BAD_BATTLE_RICE")
    if type(defending) is not bool:
        raise ValueError("BAD_SIDE")
    if any(type(n) is not int or n < 0 for n in (*deployed_troops100, *resident_troops100)):
        raise ValueError("BAD_FIELD_TROOPS")
    if not defending and resident_troops100:
        raise ValueError("UNEXPECTED_ATTACKER_RESIDENT_LIST")
    counted = sum(deployed_troops100) + (sum(resident_troops100) if defending else 0)
    cost = max(1, counted // 30)
    after = max(0, rice_before100 - cost)
    return FoodTick(rice_before100, after, counted, rice_before100 - after, after == 0)
