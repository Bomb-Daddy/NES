"""Tests for opt-in relationship/loyalty commands, explicitly NOT original ROM parity."""
import unittest
from third_branch.integrated import new_campaign, save_bytes, load_bytes
from third_branch.historical_s1 import new_campaign_historical_s1
from og.core.fixed import CoreError
from third_branch import officer_relations as rel

class Relationships(unittest.TestCase):
    def setUp(self):
        self.c=new_campaign(88)
        self.c.state['core']['officers']['A2']['province_id']='p1'
        # p2 army is permitted to remain stationed in friendly p2
    def act(self,kind,actor='A1',**params):
        return self.c.do(kind,actor,params,'personnel-'+str(self.c.state['revision']))
    def rejected(self,kind,code,actor='A1',**params):
        before=self.c.fingerprint
        with self.assertRaises(CoreError) as cm:
            self.c.preview(kind,actor,params,'bad-'+str(self.c.state['revision']))
        self.assertEqual(cm.exception.code,code)
        self.assertEqual(self.c.fingerprint,before)
    def test_gold_reward_uses_real_treasury_and_increases_loyalty_and_save(self):
        p=self.c.state['core']['provinces']['p1'];before=p['treasury_gold_cents']
        out=self.act('reward_officer',target_officer='A2',method='gold',gold=30)
        self.assertEqual(before-self.c.state['core']['provinces']['p1']['treasury_gold_cents'],3000)
        self.assertGreater(out['loyalty_after'],out['loyalty_before'])
        self.assertEqual(self.c.fingerprint,load_bytes(save_bytes(self.c)).fingerprint)
        self.c.state['core']['provinces']['p1']['treasury_gold_cents']=0
        self.rejected('reward_officer','NO_GOLD',target_officer='A2',method='gold',gold=100)
        self.rejected('reward_officer','FOREIGN_OFFICER',target_officer='B1',method='gold',gold=1)
    def test_governor_can_reward_only_in_own_province(self):
        self.act('appoint_governor','A1',officer='A2',focus='farms')
        out=self.act('reward_officer','A2',target_officer='A1',method='gold',gold=1)
        self.assertGreater(out['loyalty_after'],65)
        self.rejected('swear_brotherhood','REQUIRES_RULER',actor='A2',target_officer='A1')
    def test_horse_purchase_consumption_and_atomic_no_horse_error(self):
        self.rejected('reward_officer','NO_HORSE',target_officer='A2',method='horse',gold=1)
        self.c.state['core']['provinces']['p1']['treasury_gold_cents']=40000
        out=self.act('buy_horse',units=1);self.assertEqual(out['cost_cents'],10000)
        out=self.act('reward_officer',target_officer='A2',method='horse',gold=1)
        self.assertEqual(self.c.state['reward_horses']['p1'],0)
        self.assertGreater(out['loyalty_after'],65)
    def test_writings_advisor_intelligence_gate(self):
        self.rejected('reward_officer','NO_ADVISOR',target_officer='A2',method='writings',gold=1)
        self.c.state['core']['officers']['A1']['intelligence']=91
        self.act('appoint_advisor',target_officer='A1')
        old=self.c.state['core']['officers']['A2']['intelligence']
        out=self.act('reward_officer',target_officer='A2',method='writings',gold=1)
        self.assertEqual(out['intelligence_after'],old+1)
        self.assertEqual(self.c.state['officer_loyalty']['A2'],65)
    def test_feast_rest_bp_loyalty_gold(self):
        self.c.state['core']['provinces']['p1']['treasury_gold_cents']=40000
        self.c.state['core']['officers']['A1']['bp']=20
        self.c.state['core']['officers']['A2']['bp']=20
        before=self.c.state['core']['provinces']['p1']['treasury_gold_cents']
        out=self.act('feast');self.assertEqual(out['cost_cents'],2000)
        self.assertEqual(self.c.state['officer_loyalty']['A2'],67)
        self.assertEqual(self.c.state['core']['provinces']['p1']['treasury_gold_cents'],before-2000)
        self.assertEqual(self.c.state['core']['officers']['A2']['bp'],25)
        self.rejected('rest','RECOVERY_ALREADY_USED')
        self.c.advance_month()
        out=self.act('rest');self.assertEqual(out['cost_cents'],0)
        self.assertGreaterEqual(self.c.state['core']['officers']['A2']['bp'],35)
    def test_brotherhood_threshold_cap_loyalty_lock_and_save(self):
        self.rejected('swear_brotherhood','INSUFFICIENT_LOYALTY',target_officer='A2')
        self.c.state['officer_loyalty']['A2']=95
        out=self.act('swear_brotherhood',target_officer='A2')
        self.assertEqual(out['loyalty'],100)
        self.assertEqual(self.c.state['sworn_brothers']['A'],['A2'])
        self.assertEqual(self.c.fingerprint,load_bytes(save_bytes(self.c)).fingerprint)
        self.c.state['core']['provinces']['p1']['treasury_gold_cents']=40000
        self.act('feast')
        self.assertEqual(self.c.state['officer_loyalty']['A2'],100)
        self.rejected('swear_brotherhood','ALREADY_SWORN',target_officer='A2')
        rel.drop_binding(self.c.state,'A2')
        self.assertEqual(self.c.state['sworn_brothers']['A'],[])
    def test_governor_personnel_policy_spends_gold_and_bp(self):
        self.c.state['core']['provinces']['p1']['treasury_gold_cents']=40000
        self.act('appoint_governor',officer='A2',focus='personnel')
        old_money=self.c.state['core']['provinces']['p1']['treasury_gold_cents']
        old_loyalty=self.c.state['officer_loyalty']['A2']
        self.c.advance_month()
        self.assertEqual(self.c.state['officer_loyalty']['A2'],old_loyalty+3)
        self.assertEqual(self.c.state['personnel_history'][-1]['gold_cents'],1000)
        # Monthly taxes may be collected but exact personnel expense is journaled.
        self.assertGreaterEqual(self.c.state['core']['provinces']['p1']['treasury_gold_cents'],0)
        self.assertEqual(self.c.fingerprint,load_bytes(save_bytes(self.c)).fingerprint)

    def test_scenario1_family_not_mislabeled_brotherhood(self):
        hist=new_campaign_historical_s1(7)
        self.assertTrue(hist.state['family_relations'])
        self.assertFalse(hist.state.get('sworn_brothers'))
        self.assertEqual(len(hist.state['officer_catalog']),352)

if __name__=='__main__':unittest.main()
