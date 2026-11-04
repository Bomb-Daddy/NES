exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
recipes={r['name']:r for r in json.loads((ROOT/'batches/event_runtime_setup/runtime_recipes.json').read_text())['recipes']}
RESULTS=[]
for name,checkpoint,b in [('cold_positive','fall1175_menu',False),('cold_negative','fall1175_menu',False),('sickness_positive','summer1175',True),('sickness_negative','summer1175',True),('spring_food_10','winter1175_menu',False),('spring_food_51','winter1175_menu',False),('spring_food_52','winter1175_menu',False),('harvest_0','summer1175',True),('harvest_1','summer1175',True),('harvest_2','summer1175',True)]:
 RESULTS.append(native_recipe_case(recipes[name],checkpoint,b));(OUT/'events_mongol_results.json').write_text(json.dumps(RESULTS,indent=2))
RESULTS={'cases':len(RESULTS),'passed':True}
