exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
restore('year2_spring1177');snap('year2_spring1177_treaty');press('down',5,80);press('a',5,300);step(2400);out=snap('year2_spring1177_player')
assert out['year']==1177 and out['season']==0 and out['orders']==3
RESULTS={k:out[k] for k in ['year','season','rate','gold','food','troops','fur','orders']};(OUT/'year2_final_result.json').write_text(json.dumps(RESULTS,indent=2))
