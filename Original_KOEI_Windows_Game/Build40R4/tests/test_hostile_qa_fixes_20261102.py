"""Regression tests for defects found in the 2026-11-02 hostile QA pass."""
import copy, unittest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes, validate
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.orders_ui import SPECS, params_for_order

BAD_VALUES=[[1],{'a':1},{},[],[[]],(1,),{'x':[1]}]
KINDS=sorted({k for k,_ in SPECS.values()})

class UnhashableParams(unittest.TestCase):
    def _each(self,make):
        for base in (make(7),):
            s=base.state
            for kind in KINDS:
                fields=next(f for k,f in SPECS.values() if k==kind)
                names=[n for n,_,_ in fields]+['target','officer','target_officer','general','objective']
                for name in dict.fromkeys(names):
                    for bad in BAD_VALUES:
                        c=copy.deepcopy(base);fp=c.fingerprint
                        with self.assertRaises(CoreError,msg=(kind,name,bad)) as cm:
                            c.do(kind,next(iter(s['core']['officers'])),{name:bad},'u1')
                        self.assertEqual(c.fingerprint,fp,(kind,name,bad))
    def test_default_scenario_rejects_container_params_cleanly(self):self._each(new_campaign_41)
    def test_historical_scenario_rejects_container_params_cleanly(self):self._each(lambda seed:new_campaign_historical_s1())
    def test_unhashable_actor_rejected_cleanly(self):
        c=new_campaign_41(7)
        for bad in ([1],{'a':1},1,None,b'O01'):
            with self.assertRaises(CoreError):c.do('rest',bad,{},'a1')
    def test_legitimate_list_params_still_work(self):
        c=new_campaign_historical_s1()
        c.preview('train_troops','nes:000',{'trainers':['nes:000']},'t1')

class CorruptSaves(unittest.TestCase):
    def test_non_object_documents_are_clean_errors(self):
        for raw in (b'null',b'[]',b'1',b'"x"',b'true',b'{"format":"OG-INTEGRATED-TEST-1","sha256":"x","state":null}'):
            with self.assertRaises(CoreError,msg=raw):load_bytes(raw)

class ShippedOrderPathGuards(unittest.TestCase):
    """The two documented RED core-engine xfails are guarded on the real Campaign order path."""
    def test_distribute_food_over_100_units_rejected_no_mutation(self):
        c=new_campaign_41(7);c.state['core']['provinces']['p1']['food_units']=1000
        k,p=params_for_order('Distribute Food',{'units':'101'},'p1');fp=c.fingerprint
        with self.assertRaises(CoreError):c.do(k,'O01',p,'d')
        self.assertEqual(c.fingerprint,fp)
    def test_charisma_changes_food_distribution_support_gain(self):
        gains=[]
        for cha in (20,90):
            c=new_campaign_41(7);c.state['core']['officers']['O01']['charisma']=cha
            k,p=params_for_order('Distribute Food',{'units':'10'},'p1')
            gains.append(c.do(k,'O01',p,'d')['support_gain_ticks'])
        self.assertGreater(gains[1],gains[0])

class DuplicateRequestIdIsCheckedAtTheDoor(unittest.TestCase):
    """The request-id check in Campaign._envelope must work on its own; validate() is only the backstop."""
    def test_duplicate_id_rejected_before_any_handler_runs(self):
        for make in (lambda:new_campaign_41(7),new_campaign_historical_s1):
            c=make();actor=next(iter(c.state['core']['officers']))
            c.do('rest',actor,{},'same-id');fp=c.fingerprint
            def boom(*a,**k):raise AssertionError('handler ran for a duplicate request id')
            c._step=boom
            for call in (lambda:c.preview('rest',actor,{},'same-id'),lambda:c.execute('rest',actor,{},'same-id'),
                         lambda:c.do('rest',actor,{},'same-id')):
                with self.assertRaises(CoreError) as cm:call()
                self.assertEqual(cm.exception.code,'DUPLICATE_ID')
            self.assertEqual(c.fingerprint,fp)
    def test_validate_backstop_rejects_duplicate_journal_ids(self):
        c=new_campaign_41(7);actor=next(iter(c.state['core']['officers']))
        c.do('rest',actor,{},'id-1')
        c.do('feast',actor,{},'id-2') if False else None
        s=copy.deepcopy(c.state);s['processed'].append(s['processed'][0]);s['log'].append(copy.deepcopy(s['log'][0]));s['revision']+=1
        with self.assertRaises(CoreError) as cm:validate(s)
        self.assertEqual(cm.exception.code,'DUPLICATE_ID')

class EventRecordsAreTheirOwnJournalEntries(unittest.TestCase):
    """Design contract (historical scenario): a courier event is a separate committed journal entry,
    so Ally/Joint outcomes advance the revision by 2 while every other command advances it by 1."""
    def _ally_outcomes(self):
        """One Ally attempt per scenario seed (the acceptance roll comes from the seeded core RNG)."""
        for seed in range(1,80):
            c=new_campaign_historical_s1(seed);s=c.state;f=s['officer_faction']
            a=next(iter(f));t=next(x for x in sorted(s['factions']) if x!=f[a])
            try:c.do('Ally',a,{'target':t},'ally-1')
            except CoreError:continue
            yield c
    def test_ally_event_is_a_distinct_committed_entry_and_journal_stays_consistent(self):
        seen=set()
        for c in self._ally_outcomes():
            s=c.state
            self.assertEqual(s['revision'],len(s['log']),'revision must equal journal length')
            self.assertEqual(len(s['processed']),len(s['log']))
            self.assertEqual(len(set(s['processed'])),len(s['processed']))
            kinds=[e['kind'] for e in s['log']]
            self.assertEqual(kinds[0],'Ally')
            if len(kinds)==2:
                self.assertIn(kinds[1],('messenger_caught_secret_letter','secret_letter_delivered'))
                self.assertTrue(s['processed'][1].startswith('event:'))
                self.assertEqual(s['revision'],2)
                seen.add(kinds[1])
            self.assertEqual(load_bytes(save_bytes(c)).fingerprint,c.fingerprint)
            if len(seen)==2:break
        self.assertEqual(seen,{'messenger_caught_secret_letter','secret_letter_delivered'})
    def test_non_event_commands_and_non_historical_scenarios_advance_by_exactly_one(self):
        c=new_campaign_41(7);actor=next(iter(c.state['core']['officers']))
        c.do('rest',actor,{},'r1');self.assertEqual(c.state['revision'],1)
        c2=new_campaign_41(7);s=c2.state;f=s['officer_faction']
        a=next(iter(f));t=next(x for x in s['factions'] if x!=f[a]);c2.do('Ally',a,{'target':t},'a1')
        self.assertEqual(c2.state['revision'],1)
    def test_preview_quote_tracks_revision_across_a_two_entry_command(self):
        for c in self._ally_outcomes():
            if c.state['revision']==2:
                q=c.preview('rest',next(iter(c.state['core']['officers'])),{},'next')
                self.assertEqual(q['revision'],2);return
        self.fail('no two-entry Ally outcome found')

class SurvivingMutantKillers(unittest.TestCase):
    """Each check below was deleted in a mutation run and the old suite still passed."""
    def test_stale_expected_revision_alone_is_rejected_without_mutation(self):
        c=new_campaign_41(7);fp=c.fingerprint
        with self.assertRaises(CoreError) as cm:c.execute('rest','O01',{},'rev1',None,c.state['revision']+3)
        self.assertEqual(cm.exception.code,'STALE_REVISION');self.assertEqual(c.fingerprint,fp)
        c.do('rest','O01',{},'rev2')
        fp=c.fingerprint
        with self.assertRaises(CoreError) as cm:c.execute('feast','O01',{},'rev3',None,0)
        self.assertEqual(cm.exception.code,'STALE_REVISION');self.assertEqual(c.fingerprint,fp)
    def test_actor_in_a_province_its_faction_does_not_own_is_refused_without_mutation(self):
        for kind,params in (('market',{'commodity':'food','side':'buy','units':5}),('disband_troops',{'count':5}),
                            ('train',{'trait':'war','effort':1}),('allocate_workers',{'province_id':'p1','source':'unassigned','target':'farmers','count':2})):
            c=new_campaign_41(7);pid=c.state['core']['officers']['O01']['province_id']
            c.state['owners'][pid]='B'   # province taken over; the officer's faction no longer owns it
            fp=c.fingerprint
            with self.assertRaises(CoreError,msg=kind) as cm:c.do(kind,'O01',params,'wo1')
            self.assertEqual(cm.exception.code,'WRONG_OWNER',kind);self.assertEqual(c.fingerprint,fp,kind)
    def test_validate_rejects_a_revision_that_does_not_match_the_journal(self):
        for make in (lambda:new_campaign_41(7),new_campaign_historical_s1):
            c=make();actor=next(iter(c.state['core']['officers']));c.do('rest',actor,{},'j1')
            for field,delta in (('revision',1),('revision',-1)):
                s=copy.deepcopy(c.state);s[field]+=delta
                with self.assertRaises(CoreError) as cm:validate(s)
                self.assertEqual(cm.exception.code,'JOURNAL_MISMATCH')
            s=copy.deepcopy(c.state);s['processed'].pop()
            with self.assertRaises(CoreError) as cm:validate(s)
            self.assertEqual(cm.exception.code,'JOURNAL_MISMATCH')

if __name__=='__main__':unittest.main()

class CommitContractIsExplicit(unittest.TestCase):
    """Clients must not assume +1 per command: Campaign.last_commit states exactly what the commit did."""
    def test_last_commit_reports_revision_span_and_event_records(self):
        spans=set()
        for seed in range(1,80):
            c=new_campaign_historical_s1(seed);s=c.state;f=s['officer_faction']
            a=next(iter(f));t=next(x for x in sorted(s['factions']) if x!=f[a])
            before=s['revision']
            try:c.do('Ally',a,{'target':t},'ally-1')
            except CoreError:continue
            lc=c.last_commit
            self.assertEqual(lc['revision_before'],before)
            self.assertEqual(lc['revision_after'],c.state['revision'])
            self.assertEqual(lc['revision_after']-lc['revision_before'],1+lc['event_records'])
            self.assertEqual((lc['kind'],lc['request_id']),('Ally','ally-1'))
            spans.add(lc['event_records'])
        self.assertEqual(spans,{0,1},'both a plain Ally and an Ally with a courier event must be observed')
    def test_ordinary_command_is_exactly_one_revision(self):
        c=new_campaign_41(7);s=c.state;a=next(iter(s['officer_faction']))
        pid=s['core']['officers'][a]['province_id']
        r=s['revision']
        c.do('rest',a,{},'rest-1')
        self.assertEqual(c.last_commit['revision_after']-r,1);self.assertEqual(c.last_commit['event_records'],0)
