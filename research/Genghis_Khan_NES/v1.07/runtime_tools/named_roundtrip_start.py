exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
restore();snap('named_later_buy_before');shop(0,1);snap('named_later_natural_buy_quotes');item(2);number(1);snap('named_later_buy_prompt');press('a',5,180);press('a',5,180);out=snap('named_later_buy_result');assert out['gold']==432 and word(0x6df3)==27 and out['orders']==2
press('b',5,300);snap('named_later_root');keys(['down']*12,50);snap('named_later_pass_selection');press('a',5,180);press('a',5,600);step(1800);out=snap('named_later_summer');call('state.checkpoint',path=str(OUT/'named_later_summer.mss'));step(2)
RESULTS={k:out[k] for k in ['year','season','rate','gold','orders']}
RESULTS['merchants']=[word(0x6e18+2*m) for m in range(3)];RESULTS['jewels_sell_masks']=[word(0x7a8c+22*m+4) for m in range(3)]
(OUT/'named_roundtrip_progress.json').write_text(json.dumps(RESULTS,indent=2))
