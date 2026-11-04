exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
recipes={r['name']:r for r in json.loads((ROOT/'batches/event_runtime_setup/runtime_recipes.json').read_text())['recipes']}
RESULTS=[]
for name in ['theft_positive','theft_negative']:
 r=recipes[name];restore('winter1175_menu');entry=create_bp(r['entry_breakpoint']);pass_start();until_hit(entry,name+'_first',True)
 visited=[]
 for _ in range(60):
  active=word(0x7b34);visited.append(active)
  if active==0:break
  delete_bp(entry);step(1);entry=create_bp(r['entry_breakpoint']);until_hit(entry,name+'_search',True)
 else:raise AssertionError('No country1 theft entry')
 delete_bp(entry)
 for x in r['entry_writes']:write(x['address'],x['bytes'])
 write(0x8a,[1]);before=snap('event_'+name+'_before');finish=create_bp(r['capture_after_breakpoint']);until_hit(finish,name+'_finish',False);after=snap('event_'+name+'_after');bp=call('breakpoint.list');delete_bp(finish)
 x={'case':name,'visited_country_indices':visited,'active':active,'food_before':before['food'],'food_after':after['food'],'gold_before':before['gold'],'gold_after':after['gold'],'orders':after['orders'],'breakpoint':bp}
 RESULTS.append(x);(OUT/'theft_runtime_results.json').write_text(json.dumps(RESULTS,indent=2));assert after['food']==r['expected_food'] and after['gold']==before['gold'],x
RESULTS={'cases':len(RESULTS),'passed':True}
