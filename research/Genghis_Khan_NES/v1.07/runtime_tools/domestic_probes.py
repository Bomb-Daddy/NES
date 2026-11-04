exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
# Tax lower bound and actual positive reduction.
for q in [0,1,99,100,101]:
 restore();snap(f'tax_{q}_before');menu(0);press('a',5,500);number(q);snap(f'tax_{q}_prompt');press('a',5,400);snap(f'tax_{q}_commit');press('a',5,300);out=snap(f'tax_{q}_result')
 RESULTS.append(dict(case=f'tax{q}',tax=word(0x6e11),morale=word(0x6e0f),orders=out['orders'],abilities=out['abilities']))
 (OUT/'domestic_probe_results.json').write_text(json.dumps(RESULTS,indent=2))
# First positive draft8, with all fields and prompt captured.
restore();snap('draft8_before');menu(5);keys(['down']*5);press('a',5,500);number(8);snap('draft8_prompt');press('a',5,400);snap('draft8_commit');press('a',5,300);out=snap('draft8_result')
RESULTS.append(dict(case='draft8',troops=out['troops'],reserve=out['reserve'],eligible=out['eligible_percent'],arms=word(0x6de1),skill=word(0x6de3),morale=word(0x6e0f),orders=out['orders'],abilities=out['abilities']))
(OUT/'domestic_probe_results.json').write_text(json.dumps(RESULTS,indent=2))
