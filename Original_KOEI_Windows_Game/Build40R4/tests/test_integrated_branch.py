"""Independent end-to-end tests for the ORIGINAL third branch, not the NES port."""
import copy, unittest
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate, hash_state
from third_branch.ai import take_turn,profile_campaigns
from og.core.fixed import CoreError
from pathlib import Path
from unittest.mock import patch

class IntegratedCampaign(unittest.TestCase):
    def setUp(self):self.g=new_campaign(7)
    def act(self,kind,actor,params):
        return self.g.do(kind,actor,params,f't:{self.g.state["revision"]}:{kind}')
    def assertRejectUnchanged(self,kind,actor,params,code):
        before=self.g.fingerprint
        with self.assertRaises(CoreError) as e:self.g.preview(kind,actor,params,'fail:'+code)
        self.assertEqual(e.exception.code,code)
        self.assertEqual(self.g.fingerprint,before)
    def test_combined_monthly_economy_bp_and_cold_load(self):
        p=self.g.state['core']['provinces']['p1'];p['food_units']=300
        out=self.act('allocate_workers','A1',{'province_id':'p1','source':'unassigned','target':'farmers','count':3})
        self.assertEqual(out['bp_after'],79)
        before=self.g.fingerprint
        self.g.advance_month()
        self.assertNotEqual(before,self.g.fingerprint)
        self.assertEqual(self.g.state['core']['officers']['A1']['bp'],103) # 79 + floor(80*.30)=24
        self.assertEqual(load_bytes(save_bytes(self.g)).fingerprint,self.g.fingerprint)
    def test_known_distribute_101_rejected_and_charisma_applies(self):
        self.assertRejectUnchanged('distribute_food','A1',{'province_id':'p1','units':101},'BAD_PARAMS')
        high=self.g.preview('distribute_food','A1',{'province_id':'p1','units':10},'high')['result']['support_gain_ticks']
        low=self.g.preview('distribute_food','A2',{'province_id':'p2','units':10},'low')['result']['support_gain_ticks']
        self.assertGreater(high,low)
    def test_distribution_cap_only_spends_required_stock(self):
        p=self.g.state['core']['provinces']['p1'];p['support_ticks']=4999500
        before=p['food_units'];r=self.act('distribute_food','A1',{'province_id':'p1','units':100})
        self.assertEqual(r['consumed_units'],1)
        self.assertEqual(self.g.state['core']['provinces']['p1']['food_units'],before-1)
        self.assertEqual(self.g.state['core']['provinces']['p1']['support_ticks'],5000000)
        self.assertRejectUnchanged('distribute_food','A1',{'province_id':'p1','units':1},'AT_CAP')
    def test_market_bid_ask_conserves_gold_and_goods(self):
        p=self.g.state['core']['provinces']['p1'];x=self.g.state['extras']['p1']
        money,metal=p['treasury_gold_cents'],x['metal']
        buy=self.act('market','A1',{'side':'buy','commodity':'metal','units':4})
        self.assertEqual(money-self.g.state['core']['provinces']['p1']['treasury_gold_cents'],buy['total_cents']);self.assertEqual(self.g.state['extras']['p1']['metal'],metal+4)
        sell=self.act('market','A1',{'side':'sell','commodity':'metal','units':4})
        self.assertEqual(self.g.state['extras']['p1']['metal'],metal)
        self.assertEqual(self.g.state['core']['provinces']['p1']['treasury_gold_cents'],money-buy['total_cents']+sell['total_cents'])
    def test_labor_makes_weapons_no_officer_forge_order_and_training_changes_stat(self):
        # Director decision: metalworker production is monthly workforce output;
        # NO officer 'Make Weapons' command and no officer BP cost for forging.
        before=self.g.fingerprint
        self.assertRejectUnchanged('forge','A1',{'units':10},'UNKNOWN_KIND')
        self.assertEqual(self.g.fingerprint,before)
        metal=self.g.state['extras']['p1']['metal'];arms=self.g.state['extras']['p1']['arms']
        self.g.advance_month()
        self.assertEqual(self.g.state['extras']['p1']['arms'],arms+1)
        self.assertEqual(self.g.state['extras']['p1']['metal'],metal+2-1)
        war=self.g.state['core']['officers']['A1']['war']
        self.act('train','A1',{'trait':'war','effort':3})
        self.assertEqual(self.g.state['core']['officers']['A1']['war'],war+3)
    def test_atomic_command_preview_duplicate_and_stale_revision(self):
        before=self.g.fingerprint;rid='once';p={'province_id':'p1','source':'unassigned','target':'farmers','count':1}
        quote=self.g.preview('allocate_workers','A1',p,rid)
        self.assertEqual(self.g.fingerprint,before)
        with self.assertRaises(CoreError) as e:self.g.execute('allocate_workers','A1',p,rid,'bogus')
        self.assertEqual(e.exception.code,'STALE_QUOTE');self.assertEqual(self.g.fingerprint,before)
        self.g.execute('allocate_workers','A1',p,rid,quote['quote_id'],quote['revision'])
        with self.assertRaises(CoreError):self.g.execute('allocate_workers','A1',p,rid)
        self.assertEqual(self.g.state['core']['provinces']['p1']['workers']['farmers'],151)
    def test_gift_rejection_or_acceptance_is_real_and_deterministic(self):
        before=save_bytes(self.g)
        a=load_bytes(before);b=load_bytes(before)
        r1=a.do('Give','A1',{'target':'B','gold_cents':500},'gift');r2=b.do('Give','A1',{'target':'B','gold_cents':500},'gift')
        self.assertEqual(r1,r2);self.assertEqual(a.fingerprint,b.fingerprint)
        orig=self.g.state['core']['provinces']['p1']['treasury_gold_cents']
        self.assertEqual(a.state['core']['provinces']['p1']['treasury_gold_cents'],orig-(500 if r1['accepted'] else 0))
        self.assertRejectUnchanged('Give','A1',{'target':'B','gold_cents':9999999},'NO_GOLD')
    def test_alliance_cancel_and_joint_requires_real_plan(self):
        g=self.g;g.state['alliances'].append('A|C')
        g.state['core']['officers']['A3']['bp']=200
        self.assertRejectUnchanged('Joint','A3',{'target':'C','objective':'p4','promised_troops':20},'NO_SCHEDULED_ATTACK')
        self.act('plan_attack','A3',{'target':'p4'})
        r=self.act('Joint','A3',{'target':'C','objective':'p4','promised_troops':20})
        if r['accepted']:self.assertEqual(len(g.state['promises']),1)
        self.act('Cancel','A3',{'target':'C'})
        self.assertNotIn('A|C',self.g.state['alliances']);self.assertFalse(self.g.state['promises'])
    def test_battle_winner_and_survivor_ledger_conservation(self):
        prior=sum(p['troops'] for p in self.g.state['core']['provinces'].values())
        self.act('attack','A3',{'target':'p4','troops':180})
        self.assertIsNotNone(self.g.state['battle'])
        self.act('battle_auto','A3',{})
        self.assertIsNone(self.g.state['battle'])
        self.assertEqual(len(self.g.state['history']),1)
        h=self.g.state['history'][0]
        current=sum(p['troops'] for p in self.g.state['core']['provinces'].values())+sum(p['prisoners'] for p in self.g.state['extras'].values())
        casualties=(h['start_attacker']-h['attacker_survivors']+h['start_defender']-h['defender_survivors'])
        self.assertEqual(prior-current,casualties)
        self.assertIn(h['winner'],('ATTACKER','DEFENDER'))
        self.assertEqual(self.g.state['owners']['p4'],'A' if h['winner']=='ATTACKER' else 'B')
        self.assertEqual(load_bytes(save_bytes(self.g)).fingerprint,self.g.fingerprint)
    def test_no_attack_across_nonadjacency_or_alliance(self):
        self.assertRejectUnchanged('attack','A1',{'target':'p7','troops':100},'NOT_ADJACENT')
        self.g.state['alliances'].append('A|B')
        self.assertRejectUnchanged('attack','A3',{'target':'p4','troops':100},'ATTACK_ALLY')
    def test_battle_midpoint_cold_reload_and_auto_replay(self):
        self.act('attack','A3',{'target':'p4','troops':160})
        a=load_bytes(save_bytes(self.g));b=load_bytes(save_bytes(self.g))
        for c in (a,b):c.do('battle_auto','A3',{},'resolve')
        self.assertEqual(a.fingerprint,b.fingerprint)
        self.assertEqual(a.state['history'],b.state['history'])
    def test_6_month_advisor_changes(self):
        for _ in range(7):self.g.advance_month()
        r=self.g.advice('A')
        self.assertEqual(len(r['six_month_review']),3)
        self.assertEqual(r['observed_months'],7)
    def test_ai_legal_orders_and_safe_replay(self):
        a=self.g.clone();b=self.g.clone()
        for c in (a,b):
            for m in range(3):
                for f in ('A','B','C'):take_turn(c,f,'balanced','unit',m)
                c.advance_month()
        self.assertEqual(a.fingerprint,b.fingerprint)
    def test_transport_conserves_physical_stock_and_no_enemy_shipping(self):
        src=self.g.state['core']['provinces']['p1']['food_units']
        dst=self.g.state['core']['provinces']['p3']['food_units']
        self.act('transport','A1',{'target':'p3','commodity':'food','units':25})
        self.assertEqual(self.g.state['core']['provinces']['p1']['food_units'],src-25)
        self.assertEqual(self.g.state['core']['provinces']['p3']['food_units'],dst+25)
        self.assertRejectUnchanged('transport','A3',{'target':'p4','commodity':'food','units':5},'NOT_FRIENDLY_ADJACENT')
    def test_real_governor_allocates_workers_and_spends_bp_at_month_end(self):
        before=self.g.state['core']['provinces']['p1']['workers']['farmers']
        self.act('appoint_governor','A1',{'officer':'A1','focus':'farms'})
        self.g.advance_month()
        self.assertEqual(self.g.state['core']['provinces']['p1']['workers']['farmers'],before+10)
        self.assertEqual(self.g.state['governors']['p1']['officer'],'A1')
        self.assertEqual(load_bytes(save_bytes(self.g)).state['governors'],self.g.state['governors'])
    def test_market_stock_and_spread_prevents_money_loop(self):
        start=self.g.state['core']['provinces']['p1']['treasury_gold_cents']
        stock=self.g.state['market_stocks']['p1']['food']
        buy=self.act('market','A1',{'side':'buy','commodity':'food','units':20})
        sell=self.act('market','A1',{'side':'sell','commodity':'food','units':20})
        self.assertGreaterEqual(buy['total_cents'],sell['total_cents'])
        self.assertEqual(self.g.state['market_stocks']['p1']['food'],stock)
        self.assertLessEqual(self.g.state['core']['provinces']['p1']['treasury_gold_cents'],start)
        self.assertRejectUnchanged('market','A1',{'side':'buy','commodity':'arms','units':5000},'NO_MARKET_STOCK')
    def test_marriage_is_single_use_persistent(self):
        with patch('third_branch.integrated.randrange',return_value=0):
            r=self.act('Marry','A1',{'target':'B'})
        self.assertTrue(r['accepted'])
        self.assertFalse(self.g.state['factions']['A']['daughter'])
        self.assertIn('A|B',self.g.state['marriages'])
        self.assertEqual(load_bytes(save_bytes(self.g)).state['marriages'],['A|B'])
        self.assertRejectUnchanged('Marry','A1',{'target':'B'},'COOLDOWN')
    def test_threat_surrender_transfers_provinces_and_officers(self):
        with patch('third_branch.integrated.randrange',return_value=0):
            result=self.act('Threat','A3',{'target':'B'})
        self.assertTrue(result['accepted']);self.assertFalse(self.g.state['factions']['B']['alive'])
        self.assertEqual(self.g.state['owners']['p4'],'A')
        self.assertEqual(self.g.state['officer_faction']['B1'],'A')
        validate(load_bytes(save_bytes(self.g)).state)
    def test_joint_real_war_plan_and_allied_troop_callback(self):
        self.g.state['alliances'].append('A|C')
        self.act('plan_attack','A3',{'target':'p4'})
        with patch('third_branch.integrated.randrange',return_value=0):
            joint=self.act('Joint','A3',{'target':'C','objective':'p4','promised_troops':20})
            battle=self.act('attack','A3',{'target':'p4','troops':180})
        self.assertTrue(joint['accepted']);self.assertEqual(battle['allied_support'],20)
        self.assertEqual(self.g.state['core']['provinces']['p6']['troops'],220)
        self.act('battle_auto','A3',{})
        self.assertEqual(len(self.g.state['history']),1)
        self.assertFalse(self.g.state['promises'])
        validate(load_bytes(save_bytes(self.g)).state)
    def test_save_tamper_rejected(self):
        raw=save_bytes(self.g)
        with self.assertRaises(CoreError) as e:load_bytes(raw.replace(b'"version":1',b'"version":9'))
        self.assertEqual(e.exception.code,'HASH_MISMATCH')
        self.assertRejectUnchanged('market','A1',{'side':'buy','commodity':'arms','units':300},'NO_GOLD')
    def test_battle_end_turn_no_duplicate_and_recovers(self):
        self.act('attack','A3',{'target':'p4','troops':160})
        self.act('battle_end_turn','A3',{})
        self.assertEqual(self.g.state['battle']['day'],2)
        for _ in range(30):
            if not self.g.state['battle']:break
            self.act('battle_end_turn','A3',{})
        self.assertIsNone(self.g.state['battle'])

if __name__=='__main__':unittest.main()
