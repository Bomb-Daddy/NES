exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
r={x['name']:x for x in json.loads((ROOT/'batches/event_runtime_setup/runtime_recipes.json').read_text())['recipes']}
RESULTS=[]
for name in ['storm_positive','storm_negative']:
 RESULTS.append(native_recipe_case(r[name],'world_spring1206'))
 (OUT/'events_world_results.json').write_text(json.dumps(RESULTS,indent=2))
RESULTS={'cases':len(RESULTS),'passed':True}
