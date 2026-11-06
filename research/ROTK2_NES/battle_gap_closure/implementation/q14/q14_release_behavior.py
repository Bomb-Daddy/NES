"""Portable, side-neutral ROTK II Q14 visible-behavior contract.

Authority: shared original combat Q14 (Nov 1 matrix) as corrected by Nov 5
source findings: sengo $8436, $83A4, $8462-$8465 and callers $84A7/$85FE.
This is original-rule reference implementation, NOT a copied engine or installed
Classic/Enhanced Windows game code. Ordered six neighbors are provided by the
canonical province adjacency adapter, not sorted by this module.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Mapping, Sequence


class InvalidBattleState(ValueError):
    """The supplied normalized campaign state contradicts its own ownership."""


@dataclass(frozen=True)
class Province:
    owner: str | None
    under_attack: bool = False


@dataclass(frozen=True)
class Query:
    ruler_id: str
    slot_province: str
    contested_province: str
    adjacent_in_source_order: Sequence[str]
    provinces: Mapping[str, Province]
    ruler_owned_head: str | None


@dataclass(frozen=True)
class Destination:
    province: str
    reason: str


def _validate(query: Query) -> None:
    """Validate adapter state, without imposing a new rule on lawful NES play."""
    if not query.ruler_id:
        raise InvalidBattleState('EMPTY_RULER')
    if query.slot_province not in query.provinces or query.contested_province not in query.provinces:
        raise InvalidBattleState('MISSING_BATTLE_PROVINCE')
    n = tuple(query.adjacent_in_source_order)
    if len(n) > 6 or len(n) != len(set(n)) or query.contested_province in n:
        raise InvalidBattleState('BAD_ADJACENCY')
    if any(x not in query.provinces for x in n):
        raise InvalidBattleState('UNKNOWN_NEIGHBOR')
    if query.ruler_owned_head is not None:
        head = query.provinces.get(query.ruler_owned_head)
        if head is None or head.owner != query.ruler_id:
            raise InvalidBattleState('BROKEN_RULER_OWNED_HEAD')


def release_destination(query: Query) -> Destination | None:
    """Source-equivalent Q14 destination selection for a valid battle state.

    1. Supply slot province if unowned or owned by released officer's ruler.
    2. First source-order adjacent friendly province that is not under attack.
    3. First source-order adjacent friendly (even under attack) or unowned.
    4. Ruler's first linked owned province, which may be nonadjacent.
    5. No destination returns None, never a made-up province.

    The subsequent officer relocation/troop normalization and succession are
    intentionally left to the campaign adapter, which must commit atomically.
    """
    _validate(query)
    slot = query.provinces[query.slot_province]
    if slot.owner in (None, query.ruler_id):
        return Destination(query.slot_province, 'SLOT_PROVINCE')
    for province_id in query.adjacent_in_source_order:
        p = query.provinces[province_id]
        if p.owner == query.ruler_id and not p.under_attack:
            return Destination(province_id, 'ADJACENT_SAFE_FRIENDLY')
    for province_id in query.adjacent_in_source_order:
        p = query.provinces[province_id]
        if p.owner in (None, query.ruler_id):
            return Destination(province_id, 'ADJACENT_SECOND_PASS')
    if query.ruler_owned_head is not None:
        return Destination(query.ruler_owned_head, 'RULER_OWNED_HEAD')
    return None


def prisoner_menu(*, is_ruler: bool, destination: Destination | None) -> tuple[str, ...]:
    """Original Q14 player-visible prisoner choices; never invent ruler Recruit.

    An ordinary captured officer can be released even with no refuge; the
    original *no-destination* branch handles the officer as unaffiliated. A
    defeated ruler with no destination receives only Behead.
    """
    if is_ruler:
        return ('release', 'behead') if destination else ('behead',)
    return ('recruit', 'release', 'behead')


def ordinary_release_outcome(destination: Destination | None) -> str:
    """Classification for the source-backed subsequent relocation adapter."""
    return 'DESTINATION_INSERTION' if destination else 'UNAFFILIATED_FREE_LIST'
