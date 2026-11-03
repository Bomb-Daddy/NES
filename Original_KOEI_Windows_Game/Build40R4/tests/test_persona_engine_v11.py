"""Hostile checks for independent original-game simulated-player runner v1.1."""
import copy
import json
import tempfile
import unittest
from pathlib import Path

from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.world import world_hash
from tools.persona_engine_v11 import (CONFIG_DEFAULT,CATEGORIES,build,envelope,choose,decision_probes,
    legal_choices,load_profiles,run_campaign,run_all,visible_own_state)

class TestProfileV11(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.profiles,cls.sha=load_profiles()

    def test_strict_loader_and_all_12(self):
        self.assertEqual([x['id'] for x in self.profiles],[f'P{i:02}' for i in range(1,13)])
        self.assertEqual(len(set(x['competence']['signature_plan'] for x in self.profiles)),12)
        self.assertEqual([sum(x['priority_weights'].values()) for x in self.profiles],[100]*12)

    def test_rejects_duplicate_json_keys(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'bad.json';p.write_text('{"schemaVersion":"1.1","schemaVersion":"1.1"}')
            with self.assertRaises(ValueError):load_profiles(p)

    def test_rejects_nan_and_unknown_fields(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'bad.json';p.write_text('{"schemaVersion":NaN}')
            with self.assertRaises(ValueError):load_profiles(p)
            p.write_text('{"schemaVersion":"1.1","command":"os.system"}')
            with self.assertRaises(ValueError):load_profiles(p)

    def test_rejects_bad_competence_and_bad_weights(self):
        base=json.loads(CONFIG_DEFAULT.read_text())
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'bad.json'
            for field,value in [('candidate_limit',-1),('mechanic_knowledge_pct',101),('evaluation_noise_bps',10001)]:
                a=copy.deepcopy(base);a['profiles'][0]['competence'][field]=value
                p.write_text(json.dumps(a))
                with self.subTest(field=field),self.assertRaises(ValueError):load_profiles(p)
            a=copy.deepcopy(base);a['profiles'][0]['priority_weights']['food']=True
            p.write_text(json.dumps(a))
            with self.assertRaises(ValueError):load_profiles(p)
            a=copy.deepcopy(base);a['profiles'][0]['new_privilege']='unlimited BP'
            p.write_text(json.dumps(a))
            with self.assertRaises(ValueError):load_profiles(p)

    def test_only_own_state_observed(self):
        b=build(7);obs=visible_own_state(b)
        self.assertEqual(set(obs),{'province','officers'})
        self.assertNotIn('p2',str(obs));self.assertNotIn('rng_state',str(obs));self.assertNotIn('journal',str(obs))
        self.assertEqual(set(obs['officers']),{'o1','o2'})

    def test_other_province_cannot_change_decision(self):
        b=build(7);p=self.profiles[3]
        a,_=choose(b,p,7,100)
        b2=build(7);b2.data['provinces']['p2']['food_units']=987654
        other,_=choose(b2,p,7,100)
        self.assertEqual((a['tag'],a['params']), (other['tag'],other['params']))

    def test_preview_and_choice_do_not_mutate_or_advance_rng(self):
        b=build(7);before=world_hash(b.data);rng=b.data['rng_state']
        choice,_=choose(b,self.profiles[0],7,100)
        self.assertIsNotNone(choice)
        self.assertEqual(world_hash(b.data),before)
        self.assertEqual(b.data['rng_state'],rng)

    def test_all_available_candidate_commands_go_through_bus(self):
        b=build(7);ctx=visible_own_state(b)
        choices,_=legal_choices(b,ctx,self.profiles[9],100)
        self.assertGreater(len(choices),5)
        for c in choices:
            self.assertIn(c['kind'],b._handlers)
            self.assertIn(c['actor'],('o1','o2'))
            self.assertNotIn('p2',str(c['params']))

    def test_no_unearned_resources_or_BP(self):
        b=build(7);choice,_=choose(b,self.profiles[3],7,100)
        before=world_hash(b.data);oldbp=b.data['officers'][choice['actor']]['bp']
        cmd=envelope(b,'v11-test',1,0,choice['kind'],choice['params'],choice['actor'])
        q=b.preview(cmd);self.assertEqual(world_hash(b.data),before)
        b.commit(cmd,q['quote_id'])
        self.assertLess(b.data['officers'][choice['actor']]['bp'],oldbp)
        with self.assertRaises(CoreError):b.commit(cmd,q['quote_id'])

    def test_campaign_repeat_and_cold_load_match(self):
        p=self.profiles[3];a=run_campaign(p,7,12);b=run_campaign(p,7,12,cold_reload=True)
        self.assertEqual(a['calendar'],b['calendar'])
        self.assertEqual(a['final_hash'],b['final_hash'])
        self.assertEqual(a['decisions'],b['decisions'])

    def test_adversarial_probes_are_rejected(self):
        a=run_campaign(self.profiles[-1],7,12)
        self.assertEqual(a['probe_accepted'],0)
        self.assertGreater(a['probe_rejected'],0)

    def test_p11_attention_is_limited(self):
        a=run_campaign(self.profiles[10],7,12)
        self.assertLessEqual(a['accepted'],4)

    def test_competence_aggregate_beats_same_personality_weakened_clone(self):
        cases=decision_probes(self.profiles)
        self.assertEqual(len(cases),144)
        self.assertLess(sum(x['skilled_regret'] for x in cases),sum(x['weak_regret'] for x in cases))
        self.assertGreater(sum(x['chosen_differ'] for x in cases),30)
        # A persona has a distinct specialty and can still make a worse individual choice.

    def test_reproducible_full_matrix(self):
        a=run_all()
        self.assertEqual(a['summary']['campaign_count'],36)
        self.assertEqual(a['summary']['replayed'],12)
        self.assertEqual(a['summary']['paired_probe_count'],144)

    def test_distribution_above_100_must_reject(self):
        # Documented RED acceptance criterion: current snapshot incorrectly allows 101 units.
        b=build(7);b.data['provinces']['p1']['food_units']=1000
        c=envelope(b,'bug101',1,0,'distribute_food',{'province_id':'p1','units':101})
        h=world_hash(b.data)
        # xfail style: acceptance of 101 is observed defect; not classified as success.
        with self.assertRaises(CoreError): b.preview(c)
        self.assertEqual(world_hash(b.data),h)

    def test_distribution_food_charisma_must_change_support_effect(self):
        # Documented RED acceptance criterion. This engine still ignores Charisma.
        b_hi=build(7);b_lo=build(7)
        c_hi=envelope(b_hi,'cha_hi',1,0,'distribute_food',{'province_id':'p1','units':10},'o1')
        c_lo=envelope(b_lo,'cha_lo',1,0,'distribute_food',{'province_id':'p1','units':10},'o2')
        hi=b_hi.preview(c_hi)['result']['support_gain_ticks']
        lo=b_lo.preview(c_lo)['result']['support_gain_ticks']
        self.assertGreater(hi,lo,'higher Charisma should improve legal food distribution')

if __name__=='__main__':unittest.main()
