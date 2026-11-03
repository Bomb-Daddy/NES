"""Independent third-branch direct engine command probes; no NES/DOS imports."""
import copy,json,platform,sys
from persona_campaigns import build,envelope,OUT
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.world import world_hash,validate_world
from og.core import scheduler
from og.core.save import dump_save,load_save

def do(b,name,kind,p,actor='o1',expect_fail=True):
    before=world_hash(b.data);rng=b.data['rng_state'];c=envelope(b,'QA',name,0,kind,p,actor)
    try:r=b.commit(c);res={'case':name,'accepted':True,'code':'OK','result':r['result']}
    except CoreError as e:res={'case':name,'accepted':False,'code':e.code}
    res['state_changed']=before!=world_hash(b.data)
    res['rng_changed']=rng!=b.data['rng_state']
    res['expect_fail']=expect_fail
    return res
b=build(7)
probes=[]
probes.append(do(b,'wrong-province','cultivate',{'province_id':'p2'}))
probes.append(do(b,'not-enough-workers','recruit',{'province_id':'p1','count':999999}))
probes.append(do(b,'invalid-zero','recruit',{'province_id':'p1','count':0}))
probes.append(do(b,'invalid-unknown-command','not_a_command',{'province_id':'p1'}))
probes.append(do(b,'invalid-bool','recruit',{'province_id':'p1','count':True}))
probes.append(do(b,'wrong-bucket','allocate_workers',{'province_id':'p1','source':'farmers','target':'farmers','count':1}))
# Real approved design: one food distribution <=100, integer support gain based on Charisma with no diminishing-return factor.
b=build(7)
b.data['provinces']['p1']['food_units']=1000000;validate_world(b.data)
probes.append(do(b,'quantity-101-must-reject','distribute_food',{'province_id':'p1','units':101},expect_fail=True))
# Baseline expected approved policy at C80, q10 is 11,600 support ticks, not a relative-to-current-support fraction.
for actor in ('o1','o2'):
    c=build(7)
    r=do(c,'charisma-'+actor,'distribute_food',{'province_id':'p1','units':10},actor,expect_fail=False)
    r['expected_approved_food_ticks']=10*(1000+2*c.data['officers'][actor]['charisma'])
    r['matches_approved_rate']=r.get('result',{}).get('support_gain_ticks')==r['expected_approved_food_ticks']
    probes.append(r)
# Probe full rollback on stale revision, including RNG.
c=build(7)
req=envelope(c,'QA','valid',0,'cultivate',{'province_id':'p1'})
q=c.preview(req);c.commit(req,q['quote_id'])
old=envelope(c,'QA','stale',0,'cultivate',{'province_id':'p1'})
old['expected_world_revision']=0
old_before=world_hash(c.data)
try:c.commit(old);code='ACCEPTED'
except CoreError as e:code=e.code
probes.append({'case':'stale-revision','accepted':code=='ACCEPTED','code':code,'state_changed':old_before!=world_hash(c.data),'expect_fail':True})
raw=dump_save(c.data);c2=Bus(load_save(raw))
probes.append({'case':'save-load','pass':world_hash(c.data)==world_hash(c2.data)})
report={'environment':platform.platform(),'python':sys.version.split()[0],'probes':probes}
(OUT/'contract_probes.json').write_text(json.dumps(report,indent=2))
for p in probes:print(p)
