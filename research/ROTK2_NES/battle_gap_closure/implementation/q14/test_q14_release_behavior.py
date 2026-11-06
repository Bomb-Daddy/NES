"""Standalone original-rule positive/negative tests; no commercial ROM."""
from dataclasses import replace
import pytest
from q14_release_behavior import (
    Destination, InvalidBattleState, Province, Query,
    ordinary_release_outcome, prisoner_menu, release_destination,
)


def query(*, slot='P20', contested='P20', adjacent=('P19','P21','P29'),
          head='P41', overrides=None):
    provinces={
        'P20':Province('LiuBiao'),
        'P19':Province('YuanShu'),
        'P21':Province('YuanShu',True),
        'P29':Province(None),
        'P41':Province('YuanShu'),
        'P22':Province('Opponent'),
    }
    if overrides:
        provinces.update(overrides)
    return Query('YuanShu',slot,contested,adjacent,provinces,head)


def test_slot_unowned_priority():
    q=query(slot='P29')
    assert release_destination(q)==Destination('P29','SLOT_PROVINCE')


def test_slot_owned_by_released_ruler_priority():
    q=query(slot='P19')
    assert release_destination(q)==Destination('P19','SLOT_PROVINCE')


def test_first_safe_friendly_priority_over_earlier_unowned():
    q=query(adjacent=('P29','P21','P19'))
    assert release_destination(q)==Destination('P19','ADJACENT_SAFE_FRIENDLY')


def test_second_pass_attacked_friendly_precedes_unowned_in_source_order():
    q=query(adjacent=('P21','P29','P22'),overrides={'P19':Province('Opponent')})
    assert release_destination(q)==Destination('P21','ADJACENT_SECOND_PASS')


def test_second_pass_accepts_unowned():
    q=query(adjacent=('P22','P29','P21'),overrides={'P19':Province('Opponent')})
    assert release_destination(q)==Destination('P29','ADJACENT_SECOND_PASS')


def test_nonadjacent_linked_owned_head_fallback():
    q=query(adjacent=('P22',),head='P41')
    assert release_destination(q)==Destination('P41','RULER_OWNED_HEAD')


def test_no_hit_no_ruler_list_returns_none_not_fabricated_province():
    q=query(adjacent=('P22',),head=None)
    assert release_destination(q) is None


def test_source_neighbor_order_retained_not_alphabetic():
    q=query(adjacent=('P29','P21'),overrides={'P19':Province('Opponent')})
    assert release_destination(q).province=='P29'


def test_under_attack_safe_pass_vs_second_pass():
    q=query(adjacent=('P21','P19'))
    assert release_destination(q)==Destination('P19','ADJACENT_SAFE_FRIENDLY')


def test_normal_prisoner_release_offered_even_with_no_destination():
    assert prisoner_menu(is_ruler=False,destination=None)==('recruit','release','behead')
    assert ordinary_release_outcome(None)=='UNAFFILIATED_FREE_LIST'


def test_ruler_menu_branch_depends_on_helper_return_not_province_count():
    assert prisoner_menu(is_ruler=True,destination=Destination('P41','RULER_OWNED_HEAD'))==('release','behead')
    assert prisoner_menu(is_ruler=True,destination=None)==('behead',)


def test_corrupt_head_and_neighbors_fail_closed():
    with pytest.raises(InvalidBattleState,match='BROKEN_RULER_OWNED_HEAD'):
        release_destination(query(head='P22'))
    with pytest.raises(InvalidBattleState,match='BAD_ADJACENCY'):
        release_destination(query(adjacent=('P19','P19')))
    with pytest.raises(InvalidBattleState,match='UNKNOWN_NEIGHBOR'):
        release_destination(query(adjacent=('P888',)))


def test_source_order_can_change_result_with_two_second_pass_candidates():
    a=query(adjacent=('P21','P29'),overrides={'P19':Province('Opponent')})
    b=replace(a,adjacent_in_source_order=('P29','P21'))
    assert release_destination(a).province=='P21'
    assert release_destination(b).province=='P29'


def test_legacy_adjacent_only_negative_model_fails_original_head_case():
    """Demonstrates the old algorithm's defect; not an actual port red run."""
    q=query(adjacent=('P22',),head='P41')
    legacy=next((p for p in q.adjacent_in_source_order if q.provinces[p].owner==q.ruler_id),None)
    assert legacy is None
    assert release_destination(q).province=='P41'
