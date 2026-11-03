import unittest
from third_branch.integrated import new_campaign_41
from third_branch.tactical_view import render_battle, CELL, X0, Y0

class TacticalRuntimeTests(unittest.TestCase):
    def setUp(self):
        self.c=new_campaign_41(123)
        self.c.do('attack','O14',{'target':'p15','troops':100},'attack:initial')
    def test_true_runtime_battle_and_terrain(self):
        img=render_battle(self.c)
        self.assertEqual(img.size,(1024,768))
        self.assertEqual(self.c.state['battle']['dst'],'p15')
        self.assertEqual(self.c.state['battle']['a']['troops'],100)
    def test_live_marker_moves_and_redraws_without_modifying_terrain(self):
        start=render_battle(self.c)
        terrain=list(self.c.state['battle']['terrain'])
        from third_branch.rotk2_battle_core import reachable
        b=self.c.state['battle'];u=next(u for u in b['officer_units'] if u['id']=='O14')
        old=(u['q'],u['r'])
        blocked={(v['q'],v['r']) for v in b['officer_units'] if v['id']!='O14'}
        legal=reachable(b['terrain'],old,u['mp'],blocked,b['burning'])
        target=min(legal,key=lambda p:(legal[p],p))
        self.c.do('battle_move','O14',{'q':target[0],'r':target[1]},'move:one')
        moved=render_battle(self.c)
        self.assertNotEqual(start.tobytes(),moved.tobytes())
        self.assertEqual(self.c.state['battle']['terrain'],terrain)
        self.assertEqual(moved.getpixel((X0+target[0]*CELL+10,Y0+target[1]*CELL+10)),(219,92,69))
        self.assertNotEqual(target,old)
    def test_reject_bad_move_without_mutation_and_redraw(self):
        from og.core.fixed import CoreError
        old=self.c.fingerprint
        with self.assertRaises(CoreError):self.c.do('battle_move','O14',{'q':15,'r':0},'move:invalid')
        self.assertEqual(self.c.fingerprint,old)
        self.assertEqual(render_battle(self.c).size,(1024,768))
    def test_dynamic_soldiers_uses_actual_live_data(self):
        image=render_battle(self.c)
        u=next(u for u in self.c.state['battle']['officer_units'] if u['id']=='O14')
        x,y=X0+u['q']*CELL+10,Y0+u['r']*CELL+10
        self.c.state['battle']['officer_units'][0]['troops']=0
        no_army=render_battle(self.c)
        self.assertNotEqual(image.getpixel((x,y)),no_army.getpixel((x,y)))
    def test_actual_attack_auto_settles_to_campaign(self):
        self.c.do('battle_auto','O14',{},'battle:auto')
        self.assertIsNone(self.c.state['battle'])
        with self.assertRaises(ValueError):render_battle(self.c)
        self.assertEqual(len(self.c.state['history']),1)
if __name__=='__main__':unittest.main()
