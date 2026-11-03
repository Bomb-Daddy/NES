"""Direct executable tests for player order UI/AI integration; no mock result credits."""
import unittest, tempfile
from pathlib import Path
from unittest.mock import patch
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes, COMMANDS
from third_branch.orders_ui import SPECS, params_for_order, BATTLE_ONLY
from third_branch.player_runtime import stage_month_with_opponents

class OrdersRealSource(unittest.TestCase):
    def setUp(self):self.c=new_campaign_41(7)
    def test_full_existing_command_dispatcher_has_player_gui_forms(self):
        self.assertEqual({kind for kind,fields in SPECS.values()}, COMMANDS-BATTLE_ONLY)
        for name,(kind,fields) in SPECS.items():
            values={field:default for field,_,default in fields}
            got,params=params_for_order(name,values,'p14')
            self.assertEqual(got,kind)
            if kind in ('cultivate','allocate_workers','distribute_food','distribute_goods','recruit'):
                self.assertEqual(params['province_id'],'p14')
            for field,_,_ in fields:
                if field in ('count','units','effort','troops','promised_troops','gold_cents'):
                    self.assertIs(type(params[field]),int)
    def test_rejects_malformed_before_game_bus(self):
        for bad in ('0','-1','nan','1.0','','+3','٣','true'):
            with self.assertRaises(ValueError):params_for_order('Market',{'commodity':'metal','side':'buy','units':bad},'p14')
        with self.assertRaises(ValueError):params_for_order('Market',{'commodity':'bad','side':'buy','units':'1'},'p14')
        with self.assertRaises(ValueError):params_for_order('Market',{'commodity':'metal','side':'buy','units':'1','fake':'x'},'p14')
    def test_real_market_train_governor_and_diplomacy_are_stateful(self):
        old=self.c.fingerprint
        for idx,(name,actor,fields) in enumerate([
            ('Market','O14',{'commodity':'metal','side':'buy','units':'2'}),
            ('Train Officer','O14',{'trait':'war','effort':'2'}),
            ('Appoint Governor','O01',{'officer':'O14','focus':'farms'}),
            ('Give','O14',{'target':'C','gold_cents':'100'}),
        ]):
            kind,p=params_for_order(name,fields,'p14')
            self.c.do(kind,actor,p,f'ui-test-{idx}')
        self.assertNotEqual(old,self.c.fingerprint)
        self.assertEqual(self.c.state['governors']['p14']['officer'],'O14')
        self.assertEqual(self.c.state['core']['officers']['O14']['war'],55+14%9+2)
        self.assertEqual(load_bytes(save_bytes(self.c)).fingerprint,self.c.fingerprint)
    def test_month_runs_real_opponent_ai_and_deterministic_replay(self):
        base=self.c.fingerprint
        a,ra=stage_month_with_opponents(self.c)
        b,rb=stage_month_with_opponents(self.c)
        self.assertEqual(self.c.fingerprint,base)
        self.assertEqual(a.fingerprint,b.fingerprint)
        self.assertEqual(ra,rb)
        self.assertEqual(a.state['month'],2)
        opponents=[x for x in a.state['log'] if x['actor'] and str(x['actor']).startswith('O') and int(x['actor'][1:])>=15]
        self.assertTrue(opponents,'Actual AI commits must be logged')
        self.assertEqual(load_bytes(save_bytes(a)).fingerprint,a.fingerprint)
    def test_cannot_skip_active_battle(self):
        self.c.do('attack','O14',{'target':'p15','troops':80},'war')
        before=self.c.fingerprint
        with self.assertRaises(CoreError) as err:stage_month_with_opponents(self.c)
        self.assertEqual(err.exception.code,'BATTLE_ACTIVE')
        self.assertEqual(self.c.fingerprint,before)

if __name__=='__main__':unittest.main()
