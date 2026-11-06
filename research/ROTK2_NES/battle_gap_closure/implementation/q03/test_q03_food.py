from q03_rice_behavior import FoodTick, consume_daily_rice
import pytest

def test_resident_count():
    assert consume_daily_rice(45000,[19000],defending=True,resident_troops100=[1000]) == FoodTick(45000,44334,20000,666,False)

def test_no_resident():
    assert consume_daily_rice(45000,[19000],defending=True,resident_troops100=[0]).consumed_rice100 == 633

def test_two_human():
    assert consume_daily_rice(45000,[13000],defending=True,resident_troops100=[7000]).rice_after100 == 44334
    assert consume_daily_rice(30000,[12000],defending=False).rice_after100 == 29600

def test_no_undrawn_residents_on_attacker():
    with pytest.raises(ValueError):
        consume_daily_rice(30000,[12000],defending=False,resident_troops100=[1000])

def test_minimum_and_exhaustion():
    assert consume_daily_rice(5,[],defending=False).consumed_rice100 == 1
    assert consume_daily_rice(1,[10000],defending=True).rice_after100 == 0

def test_bad_input_rejected():
    with pytest.raises(ValueError):
        consume_daily_rice(-1,[100],defending=True)
