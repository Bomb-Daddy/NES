"""Targeted original-ROM domestic acceptance cases; ROOT executes, author does not.

Native entry: research_session_v107.py batch action, this file as path.
Restores the exact archived Spring1175 state before every independent case.
All RAM interventions are logged here and through the session's call() logger.
Outputs have a fresh run prefix, preserving earlier runs and failures.

Optional OUT/domestic_completion_selection.json:
 {"groups":["labor","specialty","draft","gift_bounds","morale_carryover"],
  "cases":[], "stop_on_failure":false}
Omit selection to run all48 cases. Run core groups first if preferred.
Offline: python domestic_completion.py --list | --self-test (no emulator access).
No new native results are claimed merely by authoring or offline-checking this file.
"""
import json,hashlib,copy,datetime,traceback,sys
from pathlib import Path
ROOT=globals().get('ROOT',Path(__file__).resolve().parents[2])
BASELINE=ROOT/'batches/runtime/live/spring1175.mss'
BASELINE_SHA='a3838705c58010a11cac37ae2d8f446cccf29aa7b904f41b199a604a98245cce'
B=0x6dcd
F={'arms':0x14,'skill':0x16,'troops':0x18,'economy':0x1a,'town':0x1c,'defense':0x1e,'masons':0x20,'food':0x36,'commodity_history':0x38,'artisans':0x3a,'food_history':0x3c,'foodworkers':0x3e,'gold':0x40,'morale':0x42,'tax':0x44,'extra_used':0x46,'reserve':0x49}
POOL=['troops','town','masons','artisans','foodworkers']
COSTS={2:[5,0,5,0,0,0],4:[0,0,5,0,0,0],8:[0,0,0,5,0,0],15:[20,0,0,10,0,10]}

def parsed(ram):
 w=lambda a:int.from_bytes(ram[a-0x6000:a-0x6000+2],'little')
 actor=w(B+0x51)
 return {**{k:w(B+o) for k,o in F.items()},'eligible':ram[B+0x48-0x6000],
         'country_hex':ram[B-0x6000:B-0x6000+83].hex(),'abilities':[w(actor+14+2*i) for i in range(6)],
         'orders':w(0x7b36),'rate':w(0x7a8a),'season':w(0x65bd),'year':w(0x65bb),
         'specialty':ram[0x7692-0x6000],'stock':[w(B+0x22+2*i) for i in range(11)],
         'prices':list(ram[0x7de5-0x6000:0x7de5-0x6000+51])}
def total(s):return sum(s[f] for f in POOL)
def equal(actual,expected,what):
 if actual!=expected:raise AssertionError(f'{what}: actual={actual!r}, expected={expected!r}')
def expected_country(before,words=None,bytes_=None):
 raw=bytearray.fromhex(before['country_hex'])
 for offset,v in (words or {}).items():raw[offset:offset+2]=int(v).to_bytes(2,'little')
 for offset,v in (bytes_ or {}).items():raw[offset]=v
 return raw.hex()
def no_change(before,after):
 equal(after['country_hex'],before['country_hex'],'entire83-byte country record')
 equal(after['abilities'],before['abilities'],'all abilities');equal(after['orders'],before['orders'],'orders')
def charged(before,after,code):
 equal(after['orders'],before['orders']-1,'one completed order')
 equal(after['abilities'],[a-b for a,b in zip(before['abilities'],COSTS[code])],f'command{code} ability debit')
 equal((after['season'],after['year']),(before['season'],before['year']),'same season')
def gift_prediction(before,mode,q):
 index=255 if mode==0 else 10 if mode==1 else before['specialty']
 offset=0x40 if index==255 else 0x22+2*index
 stock=before['gold'] if index==255 else before['stock'][index]
 assert 1<=q<=stock and total(before)>0
 if index==255:v=q
 else:
  sell=[max(1,(before['prices'][m*17+index]*(before['rate']+100)//100)*90//100) for m in range(3)]
  avg=sum(sell)//3;v=min(999,q*avg//10)+1
 gain=30*v//total(before)+1
 return {offset:stock-q,0x42:min(999,before['morale']+gain)}
def check_gift(before,after,mode,q):
 equal(after['country_hex'],expected_country(before,gift_prediction(before,mode,q)),'gift resource+morale, all other country fields')
 charged(before,after,4)
def check_draft(before,after,q):
 n=total(before);available=before['reserve']*before['eligible']//100
 assert 1<=q<=min(available,999-n)
 expected={0x18:before['troops']+q,0x49:before['reserve']-q}
 for name in ['arms','skill','morale']:expected[F[name]]=(before[name]*n+q)//(n+q)
 raw_percent=100*(available-q)//(before['reserve']-q)
 allowed={max(0,raw_percent-r) for r in range(3)}
 assert after['eligible'] in allowed,('Draft random decrement0..2',after['eligible'],sorted(allowed))
 equal(after['country_hex'],expected_country(before,expected,{0x48:after['eligible']}),'draft complete country record')
 equal(total(after)+after['reserve'],n+before['reserve'],'resident conservation');charged(before,after,15)

def cases():
 out=[]
 def add(group,name,**kw):out.append({'group':group,'name':name,**kw})
 # Existing Town0/66/67 and valid-edit cancellation/noop are preserved in assign_probe_results.
 for occupation,field,cap in [(1,'masons',65),(3,'artisans',67),(2,'foodworkers',88)]:
  for kind,q in [('zero',0),('full',cap),('over',cap+1)]:add('labor',f'labor_{field}_{kind}',occupation=occupation,field=field,q=q,kind=kind)
 for i in range(10):add('specialty',f'item{i}_q8_r101',mode=2,q=8,kind='positive',patches=[(0x7692,1,i,'country1 native-specialty index')])
 add('specialty','jewels_q1_r101',mode=2,q=1,kind='positive',patches=[(0x7692,1,2,'country1 native specialty=Jewels')])
 add('specialty','jewels_q8_r50',mode=2,q=8,kind='positive',patches=[(0x7692,1,2,'country1 native specialty=Jewels'),(0x7a8a,2,50,'controlled market Rate')])
 for n in [50,200]:add('specialty',f'jewels_q8_population{n}',mode=2,q=8,kind='positive',patches=[(0x7692,1,2,'country1 native specialty=Jewels'),(B+0x18,2,n-42,'controlled drafted population; other four groups remain42')])
 for name,q,kind,patches in [('zero',0,'invalid',[]),('max10',10,'positive',[]),('over11',11,'invalid',[]),('eligible0',0,'refusal',[(B+0x48,1,0,'zero trained eligibility')]),('population998_max1',1,'positive',[(B+0x18,2,956,'drafted population998'),(B+0x48,1,50,'trained eligibility50%')]),('population999_refusal',0,'refusal',[(B+0x18,2,957,'drafted population999'),(B+0x48,1,50,'trained eligibility50%')]),('train49_then_draft17',17,'chain',[(B+0x48,1,49,'Train People cap then Draft maximum')])]:
  add('draft','draft_'+name,q=q,kind=kind,patches=patches)
 # Give0 already observed for all three modes. These add stock, cancellation and saturation boundaries.
 for mode,label,addr in [(0,'gold',B+0x40),(1,'food',B+0x36),(2,'native_item',B+0x22)]:
  for kind,q,value in [('empty',0,0),('over',9,8),('cancel',8,8),('exact',8,8),('cap',8,None)]:
   patches=[(addr,2,value,'controlled available gift stock')] if value is not None else [(B+0x42,2,998,'high Morale cap')]
   add('gift_bounds',f'gift_{label}_{kind}',mode=mode,q=q,kind=kind,patches=patches)
 add('gift_bounds','gift_planning4_refusal',mode=0,q=0,kind='ability',patches=[(0x65d1,2,4,'insufficient Planning; shared Give guard requires5')])
 add('morale_carryover','morale998_gold8_no_gift',gifts=0)
 add('morale_carryover','morale998_gold8_two_gift1',gifts=2)
 return out

# The following functions require native-session globals and are never called by offline modes.
def capture(phase):
 label=RUN_PREFIX+'_'+CURRENT['case']['name']+'_'+phase
 raw=snap(label);s=parsed((OUT/(label+'.sram')).read_bytes())
 CURRENT['captures'].append({'phase':phase,'snapshot':raw,'ledger':s});return s

def intervention(address,width,value,reason):
 before=memory(address,width);data=int(value).to_bytes(width,'little')
 item={'address':hex(address),'width':width,'before':before.hex(),'after':data.hex(),'reason':reason}
 CURRENT['interventions'].append(item);persist()
 write(address,data);equal(memory(address,width),data,'RAM intervention readback')

def persist():
 REPORT['cases']=RESULTS;REPORT['current_case']=CURRENT
 (OUT/(RUN_PREFIX+'_results.json')).write_text(json.dumps(REPORT,indent=2)+'\n')

def fresh(case):
 global CURRENT
 CURRENT={'case':case,'status':'IN_PROGRESS','interventions':[],'captures':[]}
 persist()
 # Absolute restore name works through runtime_helpers' OUT/(name+'.mss').
 restore(str(BASELINE.with_suffix('')))
 baseline=capture('restored')
 for k in ['country_hex','abilities','orders','rate','season','year','specialty','prices']:equal(baseline[k],REFERENCE[k],'exact saved baseline '+k)
 for address,width,value,reason in case.get('patches',[]):intervention(address,width,value,reason)
 return capture('before')

def settle_positive(before,phase='after'):
 # Avoid an unnecessary extra A after the order is charged: it reopens Tax.
 out=capture(phase+'_first')
 for i in range(2):
  if out['orders']==before['orders']-1:return out
  if out['orders']!=before['orders']:break
  press('a',5,180);out=capture(phase+'_ack'+str(i+1))
 return out

def run_labor(case,before):
 menu(1);press('a',5,600);capture('edit_menu');keys(['down']*case['occupation']);press('a',5,500)
 capture('prompt_zero');number(case['q']);capture('prompt_entered');press('a',5,400);staged=capture('staged_or_rejected')
 # Original wrapper operates on a temporary83-byte record until confirmation.
 no_change(before,staged)
 if case['kind']=='over':CURRENT['expected']='over-cap numeric input rejected; left at prompt, then next case restores';return
 press('b',5,300);capture('confirmation');press('a',5,600);after=settle_positive(before)
 pair=before['troops']+before[case['field']];new_troops=pair-case['q'];pct=100*new_troops//total(before)
 morale=before['morale']*(100-2*(pct-65))//100 if pct>65 and new_troops>before['troops'] else before['morale']
 expected={F[case['field']]:case['q'],F['troops']:new_troops,F['morale']:morale}
 equal(after['country_hex'],expected_country(before,expected),'labor committed83-byte record')
 equal(total(after),total(before),'drafted pool conservation');charged(before,after,2)
 CURRENT['expected']={'pool':total(before),'troops':new_troops,case['field']:case['q'],'morale':morale}

def give_once(before,mode,q,tag='give'):
 menu(2);keys(['down']*mode);press('a',5,500);capture(tag+'_prompt_zero');number(q);capture(tag+'_prompt_entered')
 press('a',5,400);after=settle_positive(before,tag+'_after');check_gift(before,after,mode,q);return after

def run_gift(case,before):
 kind=case['kind'];mode=case['mode'];q=case['q']
 if kind=='positive' or kind in ('exact','cap'):
  after=give_once(before,mode,q);CURRENT['expected']={'word_changes':{hex(k):v for k,v in gift_prediction(before,mode,q).items()},'orders':before['orders']-1};return
 menu(2)
 if kind=='ability':after=capture('ability_refusal');no_change(before,after);return
 keys(['down']*mode);press('a',5,500);capture('prompt_or_empty_refusal')
 if kind=='empty':after=capture('after_empty');no_change(before,after);return
 number(q);capture('prompt_entered')
 press('b' if kind=='cancel' else 'a',5,400);after=capture('after_cancel_or_rejection');no_change(before,after)


def run_draft(case,before):
 if case['kind']=='chain':
  menu(3);keys(['down']*3);capture('train_people_selected');press('a',5,500);trained=settle_positive(before,'trained')
  equal(trained['country_hex'],expected_country(before,bytes_={0x48:50}),'Train49→50 only eligibility');charged(before,trained,8)
  before=trained
  # Verified train*_commit captures show main menu with Tax selected after completed order.
 menu(5);keys(['down']*5);press('a',5,500);prompt=capture('draft_prompt_or_refusal')
 if case['kind']=='refusal':no_change(before,prompt);return
 number(case['q']);capture('draft_prompt_entered');press('a',5,400)
 if case['kind']=='invalid':after=capture('draft_rejected');no_change(before,after);return
 after=settle_positive(before,'draft_after');check_draft(before,after,case['q'])
 CURRENT['expected']={'max_before':min(before['reserve']*before['eligible']//100,999-total(before)),'quantity':case['q'],'total_residents':total(before)+before['reserve'],'eligibility_check':'original rounded percentage minus native random0..2, floored0'}

def run_morale(case,before):
 intervention(B+0x42,2,998,'matched high Morale998');intervention(B+0x40,2,8,'matched Gold8')
 start=capture('matched_before');now=start
 for i in range(case['gifts']):
  now=give_once(now,0,1,'gift'+str(i+1))
  press('b',5,300);root=capture('gift'+str(i+1)+'_root');no_change(now,root);now=root
 equal(now['morale'],999 if case['gifts'] else 998,'pre-season saturation')
 equal(now['gold'],8-case['gifts'],'gift debit count');equal(now['orders'],3-case['gifts'],'pre-pass orders')
 capture('before_summer_pass');keys(['down']*12,50);press('a',5,180);capture('pass_prompt');press('a',5,50);step(3600)
 summer=capture('summer_result_or_pending_event')
 if summer['season']!=1 or summer['year']!=1175 or summer['orders']!=3:
  raise RuntimeError('Summer player turn not reached; inspect pending-event capture. No blind dialog acknowledgment performed.')
 equal(summer['gold'],8-case['gifts'],'summer gift debit persists');equal(summer['morale'],999 if case['gifts'] else 998,'summer high-Morale persistence')
 equal([summer[f] for f in POOL],[start[f] for f in POOL],'no workforce loss in matched Summer transition')
 CURRENT['expected']={'summer_gold':8-case['gifts'],'summer_morale':999 if case['gifts'] else 998,'interpretation':'matched stock/order/saturation carryover, not a proof of general unrest prevention'}

def offline_test():
 # Validate independent assertions against already observed successful native captures.
 p=ROOT/'batches/runtime/live';n=0
 for mode,label in [(0,'gold'),(1,'food'),(2,'native_item')]:
  for q in [1,8]:
   a=parsed((p/f'give_{label}_{q}_before.sram').read_bytes());b=parsed((p/f'give_{label}_{q}_result.sram').read_bytes());check_gift(a,b,mode,q);n+=1
 a=parsed((p/'draft8_before.sram').read_bytes());b=parsed((p/'draft8_result.sram').read_bytes());check_draft(a,b,8);n+=1
 equal(len(cases()),48,'case manifest count')
 assert all(x['q']<=999 for x in cases() if 'q' in x)
 print(json.dumps({'offline_existing_capture_checks':n,'new_native_cases_executed':0,'planned_cases':len(cases()),'status':'driver assertions checked, native execution NOT RUN'}))

if 'call' not in globals():
 if '--list' in sys.argv:
  print(json.dumps(cases(),indent=2));raise SystemExit(0)
 if '--self-test' in sys.argv:
  offline_test();raise SystemExit(0)
 raise SystemExit('Native batch only: execute through research_session_v107.py. Use --list or --self-test offline.')

exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
equal(hashlib.sha256(BASELINE.read_bytes()).hexdigest(),BASELINE_SHA,'saved baseline file SHA256')
REFERENCE=parsed((ROOT/'batches/runtime/live/baseline.sram').read_bytes())
RUN_PREFIX='domestic_completion_'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S%f')
selection_path=OUT/'domestic_completion_selection.json'
selection=json.loads(selection_path.read_text()) if selection_path.exists() else {}
selected=[c for c in cases() if (not selection.get('groups') or c['group'] in selection['groups']) and (not selection.get('cases') or c['name'] in selection['cases'])]
assert selected,'No selected cases'
RESULTS=[];CURRENT=None
REPORT={'status':'IN_PROGRESS','baseline':str(BASELINE),'baseline_sha256':BASELINE_SHA,'selection':selection,'planned_cases':len(selected),'prior_reused_evidence':['assign_probe_results: Town0/66/67, Artisans16 cancel, Town5 no-op','give_train_results: Gold/Food/nativeItem0,1,8 and Train49/50/zero-reserve'],'case_manifest':selected}
persist()
for case in selected:
 try:
  before=fresh(case)
  if case['group']=='labor':run_labor(case,before)
  elif case['group']=='draft':run_draft(case,before)
  elif case['group']=='morale_carryover':run_morale(case,before)
  else:run_gift(case,before)
  CURRENT['status']='PASS_VERIFIED_NATIVE'
 except Exception as exc:
  CURRENT['status']='FAIL_OR_BLOCKED';CURRENT['error']=repr(exc);CURRENT['traceback']=traceback.format_exc()
  try:capture('failure')
  except Exception as capture_error:CURRENT['failure_capture_error']=repr(capture_error)
 RESULTS.append(copy.deepcopy(CURRENT));CURRENT=None;persist()
 if RESULTS[-1]['status']!='PASS_VERIFIED_NATIVE' and selection.get('stop_on_failure'):break
failed=[x['case']['name'] for x in RESULTS if x['status']!='PASS_VERIFIED_NATIVE']
REPORT['status']='PASS_VERIFIED_NATIVE' if not failed and len(RESULTS)==len(selected) else 'FAIL_OR_BLOCKED'
REPORT['passed']=len(RESULTS)-len(failed);REPORT['failed']=failed;persist()
if failed:raise AssertionError(f'Domestic completion failures/blocks {failed}; inspect {OUT/(RUN_PREFIX+"_results.json")}')
RESULTS={'run_prefix':RUN_PREFIX,'cases':len(RESULTS),'passed':True,'report':str(OUT/(RUN_PREFIX+'_results.json'))}
