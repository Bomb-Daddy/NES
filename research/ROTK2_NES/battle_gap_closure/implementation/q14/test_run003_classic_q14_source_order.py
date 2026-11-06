"""Run003 actual Classic engine Q14 regression against source-order variants.

Port-only fixture (not original NES emulator or physical controller).
Use with PYTHONPATH=src:src/tests from extracted EXACT Run002+Run003 source.
"""
from rotk2.battle import engine as bt
from rotk2.state.models import Province, Officer, GameState
from rotk2.state.rng import RNG
from test_bpf_q14_release_destination import captured


def add_owned(s, pid, neighbors=()):
    s.provinces[pid] = Province(pid, 1, 3+pid, 2, owner=2,
                                 rice=120, gold=20, neighbors=list(neighbors))
    s.officers[pid+20] = Officer(pid+20, f'Officer {pid}', 70, 65, 60,
                                 ruler_id=2, province_id=pid, soldiers=20)


def test_attacked_friendly_neighbor_is_second_pass_not_first():
    s,b=captured()
    add_owned(s,4,neighbors=[2]);add_owned(s,5,neighbors=[2])
    # Original six-direction source order visits 4 before 5.
    s.provinces[2].neighbors.extend([4,5])
    s.provinces[4].under_attack=True
    assert bt.ruler_release_destination(s,b,2)==5
    cold=GameState.from_dict(s.to_dict())
    assert cold.provinces[4].under_attack is True
    assert bt.ruler_release_destination(cold,cold.battle,2)==5


def test_attacked_friendly_precedes_unowned_on_second_pass():
    s,b=captured()
    add_owned(s,4,neighbors=[2]);s.provinces[2].neighbors.append(4)
    s.provinces[4].under_attack=True
    s.provinces[3].owner=None
    # All available friendly neighbors attacked; first original neighbor 3
    # is unowned and precedes 4 in this fixture.
    assert bt.ruler_release_destination(s,b,2)==3
    s.provinces[2].neighbors=[1,4,3]
    assert bt.ruler_release_destination(s,b,2)==4
    assert GameState.from_dict(s.to_dict()).state_hash()==s.state_hash()


def test_explicit_owned_list_head_beats_sorted_id_and_officer_location():
    s,b=captured()
    add_owned(s,4);add_owned(s,6)
    s.rulers[2].q14_owned_head=6  # recovered from original linked ruler list
    assert s.officers[2].province_id==2
    assert bt.ruler_release_destination(s,b,2)==6
    assert bt.prisoner_options(s,b,2)==('release','behead')
    bt.choose_prisoner(s,b,2,'release')
    bt.resolve_into_campaign(s,[],RNG(101))
    assert s.officers[2].alive and s.officers[2].province_id==6
    assert s.provinces[6].owner==2
    assert s.validate()==[]
    cold=GameState.from_dict(s.to_dict())
    assert cold.rulers[2].q14_owned_head==6
    assert cold.state_hash()==s.state_hash()


def test_stale_explicit_head_is_rejected_without_settlement_mutation():
    s,b=captured()
    add_owned(s,4)
    s.rulers[2].q14_owned_head=3  # actually held by ruler 1
    before=s.state_hash()
    try:
        bt.ruler_release_destination(s,b,2)
    except ValueError as error:
        assert str(error)=='BROKEN_RULER_OWNED_HEAD'
    else:
        raise AssertionError('accepted invalid ruler-owned linked head')
    assert s.state_hash()==before
    assert not b.prisoner_choices


def test_lost_head_pointer_invalidated_at_campaign_conquest_commit():
    s,b=captured(officer_id=4)
    s.rulers[2].q14_owned_head=2
    s.provinces[2].under_attack=True
    bt.choose_prisoner(s,b,4,'release')
    bt.resolve_into_campaign(s,[],RNG(9))
    assert s.provinces[2].owner==1
    assert s.rulers[2].q14_owned_head is None
    assert s.provinces[2].under_attack is False
    assert s.validate()==[]
    cold=GameState.from_dict(s.to_dict())
    assert cold.state_hash()==s.state_hash()
