"""Director-ordered 200 BP scale, Politics-based start and monthly rollover.
These are actual core assertions, not isolated fake behavior or mock commands.
"""
import unittest
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import MAX_BP, add_officer, add_province, new_world, validate_world, world_hash
from og.core import scheduler
from og.core.bp_policy import recovery_amount


def fixture(politics=80, initial=None):
    w = new_world(7)
    add_province(w, 'p1', 10)
    add_officer(w, 'o1', 'p1', (politics, 50, 50, 50, 50), **({} if initial is None else {'bp': initial}))
    return Bus(w)


class BP200Tests(unittest.TestCase):
    def test_max_200_not_10(self):
        self.assertEqual(MAX_BP, 200)
        for bp in (0, 10, 199, 200):
            b = fixture(60, bp)
            self.assertEqual(b.data['officers']['o1']['bp'], bp)
            validate_world(b.data)
        for bp in (201,-1):
            with self.assertRaises(CoreError): fixture(60,bp)

    def test_initial_bp_matches_politics(self):
        for politics in (0, 1, 10, 30, 50, 75, 100):
            b=fixture(politics)
            self.assertEqual(b.data['officers']['o1']['bp'], politics)

    def test_recovery_rounds_down(self):
        self.assertEqual([(p,recovery_amount(p)) for p in (0,1,10,33,60,80,100)],
                         [(0,0),(1,0),(10,3),(33,9),(60,18),(80,24),(100,30)])

    def test_recovery_at_rollover_and_cap(self):
        for initial, expected in [(0,24),(100,124),(180,200),(200,200)]:
            b=fixture(80,initial)
            scheduler.run_month(b)
            self.assertEqual(b.data['officers']['o1']['bp'],expected)

    def test_first_month_no_double_initial_award(self):
        b=fixture(75)
        self.assertEqual(b.data['officers']['o1']['bp'],75)
        # First month actions begin at 75; after ending month, next-month BP is 97.
        scheduler.run_month(b)
        self.assertEqual(b.data['officers']['o1']['bp'],97)

    def test_no_double_award_and_stale_phase(self):
        b=fixture(100,120)
        scheduler.run_month(b)
        self.assertEqual(b.data['officers']['o1']['bp'],150)
        h=world_hash(b.data)
        with self.assertRaises(CoreError):scheduler.run_phase(b,1,12,b.data['revision'])
        self.assertEqual(world_hash(b.data),h)

    def test_fault_at_rollover_rolls_back(self):
        b=fixture(100,0)
        for phase in range(12):
            scheduler.run_phase(b,1,phase,b.data['revision'])
        h=world_hash(b.data)
        with self.assertRaises(CoreError):scheduler.run_phase(b,1,12,b.data['revision'],fault_at=12)
        self.assertEqual(world_hash(b.data),h)
        scheduler.run_phase(b,1,12,b.data['revision'])
        self.assertEqual(b.data['officers']['o1']['bp'],30)

    def test_cold_load_and_replay_same_rollovers(self):
        b=fixture(60,0)
        for _ in range(6):scheduler.run_month(b)
        original=world_hash(b.data)
        fresh=Bus(load_save(dump_save(b.data)))
        self.assertEqual(world_hash(fresh.data),original)
        for _ in range(18):
            scheduler.run_month(b)
            scheduler.run_month(fresh)
            self.assertEqual(world_hash(b.data),world_hash(fresh.data))
        self.assertEqual(b.data['officers']['o1']['bp'],200)

    def test_no_change_to_existing_bp_deduction(self):
        b=fixture(70,70)
        rid={'request_id':'r1','actor_officer_id':'o1','expected_world_revision':0,'kind':'cultivate','params':{'province_id':'p1'}}
        pre=b.preview(rid)
        self.assertEqual(b.data['officers']['o1']['bp'],70)
        b.commit(rid,pre['quote_id'])
        self.assertEqual(b.data['officers']['o1']['bp'],68)
        scheduler.run_month(b)
        self.assertEqual(b.data['officers']['o1']['bp'],89)

if __name__ == '__main__':unittest.main()
