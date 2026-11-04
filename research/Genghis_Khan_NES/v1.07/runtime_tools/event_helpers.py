def create_bp(spec):
 return call('breakpoint.create',**{k:v for k,v in spec.items() if k in ['memoryType','address','length','access','cpuType']})
def hit(handle):
 return next((b for b in call('breakpoint.list')['breakpoints'] if b['handle']==handle),{}).get('hit',False)
def delete_bp(b):call('breakpoint.delete',handle=b['handle'])
def until_hit(b,label,ack=False,loops=25):
 for i in range(loops):
  if hit(b['handle']):return
  r=step(600)
  if hit(b['handle']):return
  if ack:
   snap(label+'_wait_'+str(i));call('input.set',buttons={'a':True});step(5);call('input.set',buttons={})
 raise AssertionError('Breakpoint not reached '+label)
def pass_start():
 keys(['down']*12,50);press('a',5,180);press('a',5,50)
def native_recipe_case(recipe,checkpoint,initial_b=False):
 restore(checkpoint)
 if initial_b:press('b',5,180)
 entry=create_bp(recipe['entry_breakpoint']);pass_start();until_hit(entry,recipe['name']+'_entry',True)
 delete_bp(entry)
 for x in recipe['entry_writes']:write(x['address'],x['bytes'])
 write(0x8a,[1]) # Explicit original NMI RNG guard; native C289 draws remain active.
 snap('event_'+recipe['name']+'_before')
 eff=recipe.get('first_effect_breakpoint')
 if eff:
  e=create_bp(eff);until_hit(e,recipe['name']+'_effect',True);delete_bp(e)
  w=recipe['first_effect_seed_write'];write(w['address'],w['bytes']);write(0x8a,[1])
  snap('event_'+recipe['name']+'_effect_seed')
 finish=create_bp(recipe['capture_after_breakpoint']);until_hit(finish,recipe['name']+'_finish',True)
 out=snap('event_'+recipe['name']+'_after')
 state={'breakpoint':call('breakpoint.list'),'snapshot':out,'recipe':recipe,'target':{}}
 target=0x6e20 if recipe['name'].startswith('storm') else 0x6dcd
 for x in recipe['entry_writes']:
  if target<=x['address']<target+83:state['target'][x['field']]=int.from_bytes(memory(x['address'],len(x['bytes'])),'little')
 delete_bp(finish)
 expected=recipe.get('expected_after',recipe.get('expected_after_upkeep',{}))
 # Only named initialized fields are expected; synthetic source defaults are not assertions about untouched native fields.
 state['checks']={k:state['target'][k]==v for k,v in expected.items() if k in state['target']}
 (OUT/('event_'+recipe['name']+'_verification.json')).write_text(json.dumps(state,indent=2))
 assert all(state['checks'].values()),(recipe['name'],state['target'],expected)
 return {'case':recipe['name'],'passed':True,'fields':state['target']}
