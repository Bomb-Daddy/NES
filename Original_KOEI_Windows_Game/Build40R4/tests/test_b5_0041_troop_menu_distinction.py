"""Director-locked Troop Assignments is not a second Recruit/Move Army menu."""
from third_branch.troop_assignments_ui import ACTIONS,build_command


def test_troop_assignments_only_existing_soldier_transfer_and_disband():
    assert ACTIONS == ('Transfer Between Generals','Disband to Population')
    assert build_command({},'p14',ACTIONS[0],'O14','O13',10) == ('assign_troops',{'target_officer':'O13','count':10})
    assert build_command({},'p14',ACTIONS[1],'O14','O13',10) == ('disband_troops',{'count':10})
