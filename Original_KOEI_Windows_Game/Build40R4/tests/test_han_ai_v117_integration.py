"""Regression: v1.16 PC runtime actually uses Han v3.2 on current economic policy."""
from unittest import TestCase
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.player_runtime import stage_month_with_opponents
from third_branch.historical_ai import PROFILES
from third_branch.historical_ai_v3 import rank_choices

class HanV117Tests(TestCase):
    def test_profiles_twelve_unique_han_labels(self):
        self.assertEqual(len(PROFILES), 12)
        self.assertEqual(len({p['name'] for p in PROFILES}), 12)
        self.assertTrue(all('policy_v3' in p for p in PROFILES))
    def test_han_v3_used_by_live_pc_month_with_1000_policy(self):
        c=new_campaign_historical_s1(seed=17,balance_policy='OG_1000_DIRECTOR_V2')
        _,r=stage_month_with_opponents(c,player='R0')
        self.assertTrue(r['opponents'])
        seen=[e for e in r['opponents'] if 'ai_name' in e]
        self.assertEqual(len(seen),len(r['opponents']))
        self.assertTrue(all('profile_id' in e or e['kind']=='inactive' for e in seen))
        self.assertEqual(c.state['month'],1)  # PC action staged, original untouched
    def test_1000_cultivation_candidate_observed(self):
        from third_branch.historical_ai_v3 import _candidates, _recent, PROFILES as V3
        c=new_campaign_historical_s1(seed=17,balance_policy='OG_1000_DIRECTOR_V2')
        faction='R3'  # Historical agrarian profile, no bonus or private data.
        for pid,owner in c.state['owners'].items():
            if owner==faction:
                c.state['core']['provinces'][pid]['cultivation']=150
                w=c.state['core']['provinces'][pid]['workers']
                w['town']+=w['land']; w['land']=0  # conservation-preserving baseline for labor reallocation
        opts=_candidates(c,faction,'farmer',V3['P04'],_recent(c.state,faction))
        self.assertFalse(any(x.kind=='cultivate' for x in opts))
        self.assertTrue(any(x.kind=='allocate_workers' and x.params.get('target')=='land' for x in opts),
                        'Land improvements must come from real worker reallocation, not officer Cultivate')
