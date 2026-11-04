exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
restore('world_spring1206');snap('world_year_repeat_start');RESULTS=[]
for season,name in [(1,'summer1206'),(2,'fall1206'),(3,'winter1206'),(0,'spring1207')]:
 pass_start();step(3600);out=snap('world_year_repeat_'+name)
 call('state.checkpoint',path=str(OUT/('world_year_repeat_'+name+'.mss')));step(2)
 RESULTS.append({k:out[k] for k in ['label','year','season','rate','gold','food','troops','fur','orders']});(OUT/'world_year_repeat_results.json').write_text(json.dumps(RESULTS,indent=2))
 assert out['season']==season
 if out['orders']!=3:
  if season==2:
   press('down',5,80);press('a',5,300);step(2400);out=snap('world_year_repeat_fall_player')
   assert out['orders']==3
  else:raise RuntimeError('Inspect pending original UI event '+name)
RESULTS={'seasons':4,'passed':True}
