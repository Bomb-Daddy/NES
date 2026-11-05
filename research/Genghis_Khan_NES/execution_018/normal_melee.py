"""Genghis Khan NES normal-melee numeric kernel, EXEC0018.

Authored from the supplied approved Shared-BP ROM's bank-5 routines and
new native captures. No ROM bytes or copyrighted assets are embedded.
This resolves casualty arithmetic, not target legality, UI, AI, or terminal
battle outcomes. Call after legal-target selection and required animation.
The RNG callback must implement the selected game's rand(n), in call order.
"""
from __future__ import annotations
from dataclasses import dataclass
from typing import Callable

# Numeric coefficients, indexed by original type/terrain IDs.
MATCHUP = ((3, 5, 5), (4, 3, 3), (4, 4, 3))
TERRAIN = ((1, 1, 3, 2, 1, 5), (1, 2, 3, 4, 5, 5), (1, 2, 3, 4, 5, 5))


def u16(value: int) -> int:
    return value & 0xFFFF


def wide_div(a: int, b: int, divisor: int) -> int:
    """Original wide multiply/divide helper: word inputs and word result."""
    a, b, divisor = u16(a), u16(b), u16(divisor)
    return 0xFFFF if divisor == 0 else u16((a * b) // divisor)


def percent(a: int, b: int) -> int:
    return wide_div(a, b, 100)


@dataclass(frozen=True)
class Fighter:
    soldiers: int
    arms: int
    skill: int
    leadership: int
    judgment: int
    body: int
    battle: int
    role: int
    unit_type: int  # 0 cavalry, 1 infantry, 2 archers
    terrain: int   # 2 plain, 3 forest, 6 castle; other labels not assumed

    def __post_init__(self) -> None:
        for name in ('soldiers', 'arms', 'skill', 'leadership', 'judgment', 'body', 'battle'):
            value = getattr(self, name)
            if type(value) is not int or not 0 <= value <= 65535:
                raise ValueError(f'{name} must be an unsigned 16-bit integer')
        if type(self.role) is not int or not 0 <= self.role <= 255:
            raise ValueError('role must be a byte')
        if self.unit_type not in range(3) or self.terrain not in range(7):
            raise ValueError('unit_type must be 0..2 and terrain 0..6')


def role_bonus(role: int) -> int:
    return 2 if role == 100 else 1 if role == 101 else 0


def strength(f: Fighter) -> int:
    """Bank 5 $8061: population, skill, arms and specific leader attributes."""
    skill_bucket = f.skill // 10 + 1
    arms_bucket = f.arms // 5 + 1
    trained = u16(percent(f.soldiers, 100 + skill_bucket)
                  + percent(f.soldiers, 100 + arms_bucket)) >> 1
    leader_factor = 1090 if f.role == 102 else u16(
        1000 + f.battle + (f.leadership >> 1) + (f.body >> 2) + (f.judgment >> 1))
    led = wide_div(f.soldiers, leader_factor, 1000)
    return u16(trained + led) >> 1


def share(attacker_power: int, defender_power: int) -> int:
    """Bank 5 $80D4: wrapped powers -> attacker percentage clamped to 1..99."""
    a, b = u16(attacker_power), u16(defender_power)
    if a == b == 0:
        return 50
    return min(99, max(1, wide_div(a, 100, u16(a + b))))


def casualty(soldiers: int, own_share: int, random24: int) -> int:
    """Bank 5 $811E: actual number removed, with saturation at current count."""
    if type(soldiers) is not int or not 0 <= soldiers <= 65535:
        raise ValueError('soldiers must be a word')
    if not 1 <= own_share <= 99 or not 0 <= random24 < 24:
        raise ValueError('own_share must be 1..99 and random24 0..23')
    other = 100 - own_share
    scaled = percent(soldiers, 87 + random24)
    ratio = wide_div(scaled, other * other, own_share * own_share + other * other)
    requested = u16((ratio >> 1) + other // own_share + 1)
    return min(soldiers, requested)


def normal_melee(attacker: Fighter, defender: Fighter,
                 rand: Callable[[int], int], *,
                 defending_country_defense: int | None = None) -> dict:
    """Resolve one legal normal attack, including the castle prelude.

    For terrain 6 supply the ORIGINAL DEFENDING SIDE'S country Defense,
    not necessarily the country belonging to the currently defending unit.
    Removal/leader-capture handlers must consume the zero-count flags outside
    this numeric kernel. Do not silently treat a zero count as battle victory.
    """
    if attacker.soldiers == 0 or defender.soldiers == 0:
        raise ValueError('normal attack requires two live units')
    draws: list[dict[str, int]] = []
    def draw(n: int) -> int:
        value = rand(n)
        if type(value) is not int or not 0 <= value < n:
            raise ValueError(f'rand({n}) returned invalid value {value!r}')
        draws.append({'bound': n, 'value': value})
        return value

    fort_after = defending_country_defense
    fort_loss = 0
    if defender.terrain == 6:
        if type(fort_after) is not int or not 0 <= fort_after <= 65535:
            raise ValueError('castle defense requires an unsigned country Defense word')
        arms_bucket = attacker.arms // 5 + 1
        requested = u16(5 + draw(4) + percent(attacker.soldiers, 100 + arms_bucket) // 30)
        fort_loss = min(fort_after, requested)
        fort_after -= fort_loss
        defense_multiplier = 3 + draw(fort_after // 7 + 1) + role_bonus(defender.role)
    else:
        defense_multiplier = TERRAIN[defender.unit_type][defender.terrain] + role_bonus(defender.role)
    attack_multiplier = MATCHUP[attacker.unit_type][defender.unit_type] + role_bonus(attacker.role)
    a_strength, d_strength = strength(attacker), strength(defender)
    a_share = share(u16(a_strength * attack_multiplier), u16(d_strength * defense_multiplier))
    a_loss = casualty(attacker.soldiers, a_share, draw(24))
    d_loss = casualty(defender.soldiers, 100 - a_share, draw(24))
    return {
        'attacker_strength': a_strength, 'defender_strength': d_strength,
        'attack_multiplier': attack_multiplier, 'defense_multiplier': defense_multiplier,
        'attacker_share': a_share, 'defender_share': 100 - a_share,
        'attacker_loss': a_loss, 'defender_loss': d_loss,
        'attacker_remaining': attacker.soldiers - a_loss,
        'defender_remaining': defender.soldiers - d_loss,
        'attacker_zero': attacker.soldiers == a_loss,
        'defender_zero': defender.soldiers == d_loss,
        'country_defense_before': defending_country_defense,
        'country_defense_after': fort_after, 'country_defense_loss': fort_loss,
        'draws': draws,
    }
