exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
restore();snap('tax100_correct_before');menu(0);press('a',5,500);number(100);snap('tax100_correct_prompt');press('a',5,400);press('a',5,300);out=snap('tax100_correct_result');RESULTS={'tax':word(0x6e11),'morale':word(0x6e0f),'orders':out['orders'],'abilities':out['abilities']}
(OUT/'tax100_corrected_results.json').write_text(json.dumps(RESULTS,indent=2))
assert out['orders']==2 and 41<=word(0x6e11)<=48 and word(0x6e0f)==0
