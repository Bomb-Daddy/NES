exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
restore('named_later_summer');snap('named_later_summer_resumed');pass_start();step(3600);s=snap('named_later_fall_before_treaty')
assert s['season']==2 and s['gold']==432
press('down',5,80);press('a',5,300);step(2400);s=snap('named_later_fall_player')
assert s['gold']==442 and s['orders']==3
call('state.checkpoint',path=str(OUT/'named_later_fall_player.mss'));step(2)
RESULTS={'season':s['season'],'rate':s['rate'],'other_income':10,'orders':s['orders']}
