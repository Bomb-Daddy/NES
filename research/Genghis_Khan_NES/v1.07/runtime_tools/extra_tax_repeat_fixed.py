exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
for q in [1,9]:
 restore();write(0x8a,[1]);write(0x84,(8).to_bytes(6,'big'));menu(0);press('down',5,80);press('a',5,500);number(q);press('a',5,400);press('a',5,300);before=snap(f'levy_repeat_v2_{q}_before')
 menu(0);press('down',5,80);press('a',5,500);snap(f'levy_repeat_v2_{q}_denial_message');press('a',5,300);out=snap(f'levy_repeat_v2_{q}_after')
 x={'q':q,'gold_before':before['gold'],'gold_after':out['gold'],'orders_before':before['orders'],'orders_after':out['orders'],'used':word(0x6e13),'abilities_before':before['abilities'],'abilities_after':out['abilities']};RESULTS.append(x)
 assert x['gold_before']==x['gold_after'] and x['orders_before']==x['orders_after']==2 and x['abilities_before']==x['abilities_after'] and x['used']==1
(OUT/'extra_tax_second_use_verified.json').write_text(json.dumps(RESULTS,indent=2))
