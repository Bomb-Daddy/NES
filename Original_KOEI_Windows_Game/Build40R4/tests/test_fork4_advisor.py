"""Fork Four regression: advisor reasoning, human zero weight, full saves."""
import copy, tempfile, unittest
from pathlib import Path
from third_branch.fork4_advisor import select_advisor, observe_faction, evaluate_advisor
from third_branch.historical_ai_v3 import Choice, rank_choices
from third_branch.historical_s1 import new_campaign_historical_s1
from fork4.batch_archive import CampaignArchive


class Fork4AdvisoryQA(unittest.TestCase):
    def setUp(self):
        self.c=new_campaign_historical_s1(17,balance_policy='OG_1000_DIRECTOR_V2')
        self.c.state['fork4_advisor_config']={'enabled':True,'weight_bps':2000,'seed':17}

    def test_highest_eligible_and_tie_is_stable(self):
        f='R0';a=select_advisor(self.c.state,f)
        roster=[(oid,o['intelligence']) for oid,o in self.c.state['core']['officers'].items()
                if self.c.state['officer_faction'][oid]==f and oid!=self.c.state['ruler_slots'][f]]
        best=max(score for oid,score in roster)
        self.assertEqual(a['intelligence'],best)
        self.assertEqual(a['officer_id'],min(oid for oid,score in roster if score==best))

    def test_true_data_identical_at_low_and_high_int(self):
        s=copy.deepcopy(self.c.state);f='R0';a=select_advisor(s,f)
        original=observe_faction(s,f)
        s['core']['officers'][a['officer_id']]['intelligence']=20
        self.assertEqual(original,observe_faction(s,f))
        s['core']['officers'][a['officer_id']]['intelligence']=95
        self.assertEqual(original,observe_faction(s,f))
        self.assertEqual(len(original['provinces']),sum(owner==f for owner in s['owners'].values()))
        self.assertNotIn('p2',original['provinces'])

    def test_bad_advisor_overvalues_recruiting_in_a_shortage(self):
        s=copy.deepcopy(self.c.state);f='R0';a=select_advisor(s,f)
        oid=a['officer_id'];home=s['core']['officers'][oid]['province_id']
        s['core']['provinces'][home]['food_units']=1
        choice=Choice(oid,'recruit',{'province_id':home,'count':10},'military',53,'new recruits')
        s['core']['officers'][oid]['intelligence']=20
        _,bad=evaluate_advisor(s,f,[(53,choice)])
        s['core']['officers'][oid]['intelligence']=95
        _,good=evaluate_advisor(s,f,[(53,choice)])
        self.assertGreater(bad['assessments'][0]['advisor_score'],good['assessments'][0]['advisor_score'])
        self.assertEqual(s['core']['provinces'][home]['food_units'],1)

    def test_ai_20_percent_human_zero_percent(self):
        f='R0';style='curious';rank,audit=rank_choices(self.c,f,style,with_audit=True)
        self.assertEqual(audit['weight_bps'],2000)
        self.c.state['human_faction']=f
        human,ha=rank_choices(self.c,f,style,with_audit=True)
        self.assertEqual(ha['weight_bps'],0)
        self.assertEqual(human,rank_choices(self.c,f,style))
        self.assertTrue(rank)

    def test_deterministic_same_input_and_no_source_mutation(self):
        before=copy.deepcopy(self.c.state)
        x=rank_choices(self.c,'R0','curious',with_audit=True)
        y=rank_choices(self.c,'R0','curious',with_audit=True)
        self.assertEqual(x,y)
        self.assertEqual(before,self.c.state)

    def test_fork4_save_all_provinces_all_generals_turn_zero_and_one(self):
        with tempfile.TemporaryDirectory() as d:
            path=Path(d)/'campaign.f4save'
            with CampaignArchive(path) as a:
                self.assertTrue(a.record(self.c,0))
                self.assertFalse(a.record(self.c,0))
                self.assertEqual(a.verify_counts(0),{'province_history':41,'officer_history':352,'faction_history':15})
                c=a.cold_load();c.advance_month()
                self.assertTrue(a.record(c,1))
                self.assertEqual(a.verify_counts(1),{'province_history':41,'officer_history':352,'faction_history':15})
                self.assertEqual(a.cold_load().fingerprint,c.fingerprint)
                self.assertEqual(a.db.execute('SELECT COUNT(*) FROM officer_history').fetchone()[0],704)

if __name__=='__main__':unittest.main()
