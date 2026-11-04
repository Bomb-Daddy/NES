exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
restore('spring1176_menu');RESULTS=[];snap('year2_spring1176_before')
for season,name in [(1,'summer1176'),(2,'fall1176'),(3,'winter1176'),(0,'spring1177')]:
 pass_start();step(3600);out=snap('year2_'+name)
 call('state.checkpoint',path=str(OUT/('year2_'+name+'.mss')));step(2)
 RESULTS.append({k:out[k] for k in ['label','year','season','rate','gold','food','troops','fur','orders']});(OUT/'year2_mongol_results.json').write_text(json.dumps(RESULTS,indent=2))
 assert out['season']==season,(name,'Not reached season',out['season'])
 if out['orders']!=3:raise RuntimeError('Original UI event pending at '+name+'; inspect saved screenshot/checkpoint and continue')
RESULTS={'seasons':4,'passed':True}
