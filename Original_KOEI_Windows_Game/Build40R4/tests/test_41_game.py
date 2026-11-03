import copy, tempfile, unittest
from pathlib import Path
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_file, load_file, validate
from third_branch.graph41 import GRAPH41, validate_graph41
from third_branch.strategic_game import render, province_at_pixel, SLOTS, SEASONS, MAP_X, MAP_Y, MAP_DW, MAP_DH

class Test41Integration(unittest.TestCase):
    def setUp(self): self.c=new_campaign_41(41)
    def test_all_41_real_synchronized_records(self):
        s=self.c.state
        for field in ('core','owners','neighbors','extras','market_stocks'):
            data=s[field]['provinces'] if field=='core' else s[field]
            self.assertEqual(set(data),{f'p{i}' for i in range(1,42)})
        self.assertEqual(len(s['core']['officers']),41)
        self.assertEqual(sum(map(len,s['neighbors'].values()))//2,93)
        self.assertTrue(validate_graph41())
    def test_4_40_negative_graph_controls(self):
        x={k:tuple(v) for k,v in GRAPH41.items()};x[4]=x[40]
        with self.assertRaises(ValueError):validate_graph41(x)
        y=copy.deepcopy(GRAPH41);y[41]=tuple(n for n in y[41] if n!=40)
        with self.assertRaises(ValueError):validate_graph41(y)
    def test_all_41_art_selection_targets_and_all_seasons(self):
        for i in range(1,42):
            x,y,w,h=SLOTS[str(i)]['rect_px']
            p=province_at_pixel(MAP_X+(x+w/2)*MAP_DW/1448,MAP_Y+(y+h/2)*MAP_DH/1086)
            self.assertEqual(p,i)
        for season in SEASONS:
            img=render(self.c,41,season)
            self.assertEqual(img.size,(1024,768))
    def test_real_action_and_month_persist_after_cold_load(self):
        old=self.c.fingerprint
        before_farmers=self.c.state['core']['provinces']['p41']['workers']['farmers']
        r=self.c.do('allocate_workers','O41',{'province_id':'p41','source':'unassigned','target':'farmers','count':2},'farm41')
        self.assertEqual(r['province_id'],'p41')
        self.assertEqual(self.c.state['core']['provinces']['p41']['workers']['farmers'],before_farmers+2)
        self.c.advance_month()
        with tempfile.TemporaryDirectory() as t:
            path=Path(t)/'save.json';save_file(self.c,path);fresh=load_file(path)
        self.assertNotEqual(self.c.fingerprint,old)
        self.assertEqual(self.c.fingerprint,fresh.fingerprint)
        self.assertEqual(len(fresh.state['core']['provinces']),41)
    def test_attack_14_15_actual_ledger_and_owner_transition(self):
        prev=self.c.state['owners']['p15'];self.assertEqual(prev,'B')
        self.c.do('attack','O14',{'target':'p15','troops':190},'war-14-15')
        self.assertIsNotNone(self.c.state['battle'])
        self.c.do('battle_auto','O14',{},'resolve')
        self.assertIsNone(self.c.state['battle']);self.assertEqual(len(self.c.state['history']),1)
        validate(self.c.state)
    def test_neighbor_41_and_far_attack_reject_without_mutation(self):
        self.assertEqual(set(self.c.state['neighbors']['p41']),{'p34','p36','p39','p40'})
        b=self.c.fingerprint
        with self.assertRaises(CoreError):self.c.do('attack','O41',{'target':'p1','troops':20},'badwar')
        self.assertEqual(self.c.fingerprint,b)
    def test_all_4_season_months_consistent(self):
        from third_branch.strategic_game import SEASONS
        for n,season in enumerate(SEASONS):
            # render uses live calendar, no manual art overrides
            self.assertEqual((self.c.state['month']-1)//3,n)
            self.assertEqual(render(self.c,4).size,(1024,768))
            for _ in range(3):self.c.advance_month()
        self.assertEqual(self.c.state['month'],13)
        self.assertEqual(len(self.c.state['advisor_snapshots']),12)
        self.assertEqual(len(self.c.state['core']['provinces']),41)
if __name__=='__main__':unittest.main()
