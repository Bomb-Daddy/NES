exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
restore();write(0x8a,[1]);write(0x84,(8).to_bytes(6,'big'));menu(0);press('down',5,80);press('a',5,500);number(1);press('a',5,400);press('a',5,300);before=snap('levy_deny_bp_before');menu(0);press('down',5,80)
bp=create_bp({'memoryType':'nesPrgRom','address':0x3eb5d,'length':1,'access':'read','cpuType':'nes'});call('input.set',buttons={'a':True});until_hit(bp,'levy_denial_call',False);call('input.set',buttons={});frame=word(4);entry={'frame':frame,'vm_pc':word(6),'message_ptr':word(frame+11),'used':word(0x6e13),'breakpoint':call('breakpoint.list')};delete_bp(bp)
for n in [1,2,4,8,16,32,64,128]:
 step(n);snap('levy_denial_message_frame'+str(n))
snap('levy_denial_actual_message');press('a',5,400);after=snap('levy_deny_bp_after')
RESULTS={'entry':entry,'gold_before':before['gold'],'gold_after':after['gold'],'orders_before':before['orders'],'orders_after':after['orders'],'abilities_before':before['abilities'],'abilities_after':after['abilities']};(OUT/'levy_denial_breakpoint_results.json').write_text(json.dumps(RESULTS,indent=2));assert entry['message_ptr']==0x9dc0 and before['gold']==after['gold'] and before['orders']==after['orders']==2 and before['abilities']==after['abilities']
