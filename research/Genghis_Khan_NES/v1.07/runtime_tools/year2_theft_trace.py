exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
restore('year2_summer1176');snap('year2_natural_theft_start');entry=create_bp({'memoryType':'nesPrgRom','address':0x295,'length':1,'access':'exec','cpuType':'nes'});pass_start();until_hit(entry,'year2_natural_theft_first',True)
visited=[]
for _ in range(60):
 active=word(0x7b34);visited.append([active,word(0x65bd)])
 if active==0 and word(0x65bd)==2:break
 delete_bp(entry);step(1);entry=create_bp({'memoryType':'nesPrgRom','address':0x295,'length':1,'access':'exec','cpuType':'nes'});until_hit(entry,'year2_natural_theft_search',True)
else:raise AssertionError('No original country1 Autumn theft check')
delete_bp(entry);before=snap('year2_natural_theft_before');positive=create_bp({'memoryType':'nesPrgRom','address':253452,'length':1,'access':'exec','cpuType':'nes'});negative=create_bp({'memoryType':'nesPrgRom','address':0x346,'length':1,'access':'exec','cpuType':'nes'})
for _ in range(30):
 if hit(positive['handle']) or hit(negative['handle']):break
 step(60)
else:raise AssertionError('Theft check did not return')
after=snap('year2_natural_theft_after');bplist=call('breakpoint.list');is_positive=hit(positive['handle']);delete_bp(positive);delete_bp(negative)
RESULTS={'visited':visited,'before_food':before['food'],'after_food':after['food'],'gold_before':before['gold'],'gold_after':after['gold'],'positive':is_positive,'no_ram_interventions':True,'before_seed':before['rng_seed_hex'],'after_seed':after['rng_seed_hex'],'breakpoints':bplist}
(OUT/'year2_natural_theft_trace.json').write_text(json.dumps(RESULTS,indent=2))
assert before['food']==346 and after['food']==214 and is_positive and before['gold']==after['gold'],RESULTS
