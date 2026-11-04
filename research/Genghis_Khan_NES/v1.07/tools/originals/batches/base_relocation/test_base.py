import unittest,hashlib
from replay_base import *

class BaseChecks(unittest.TestCase):
 def test_private_input_hashes(self):
  self.assertEqual(hashlib.sha256(l.t.h.ROM.read_bytes()).hexdigest(),l.t.h.EXPECTED)
  self.assertEqual(hashlib.sha256(l.BASELINE).hexdigest(),'dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773')
 def test_cost_row_and_body_threshold(self):
  self.assertEqual(list(l.BASELINE[0x7e18-0x6000+6*21:0x7e18-0x6000+6*22]),[0,0,0,0,20,0])
  for body in (0,19,20,21,999):self.assertEqual(l.ability_gate(21,[0,0,0,0,body,0]),body>=20)
 def test_swap_and_body_deduction(self):
  c,r=relocate();self.assertEqual(r['after']['officers'],[0x65de,0x65bf]);self.assertEqual(r['after']['abilities'],[89,77,112,61,150,129]);self.assertEqual(r['after']['roles'],[101,100])
 def test_all_remaining_orders_consumed(self):
  for orders in (0,1,2,3):self.assertEqual(relocate(orders)[1]['after']['orders'],0)
 def test_resources_do_not_move_with_ruler(self):
  c,r=relocate();self.assertEqual(r['after']['gold'],r['before']['gold']);self.assertEqual(r['after']['jewels'],r['before']['jewels'])
  allowed={0x65d5,0x65d6,0x6e1e,0x6e1f,0x6e71,0x6e72,0x7b36,0x7b37,0x77d2,0x77d3,0x77e5,0x77e6}
  self.assertTrue({int(a,16) for a in r['changed_addresses']}<=allowed)
 def test_record_words_cleared(self):
  self.assertEqual(relocate()[1]['after']['record_words'],[0,0])
 def test_cancel_before_commit_no_persistent_write(self):
  c=prepare(0x9675,[SRC],baseline=True);put(c,8,0);before=bytes(c.m[0x6000:0x8000]);r=run(c,0x9695)
  self.assertEqual(r['A'],255);self.assertEqual(before,bytes(c.m[0x6000:0x8000]))
 def test_owner_adjacency_gate(self):
  self.assertEqual(l.destination_gate(0,1),0);self.assertEqual(l.destination_gate(0,1,{0x65fb:0}),1)
  self.assertEqual(l.destination_gate(0,0),0)
 def test_no_duplicate_general_command_cost(self):
  c,_=relocate();resume(c,0x9834,bank=3);put(c,8,21);self.assertEqual(run(c,0x983f)['A'],255)
  resume(c,0x8c95);put(c,8,255);before=state(c);run(c,0x8ca9);self.assertEqual(state(c),before)
 def test_remaining_order_clear_mutation_detected(self):
  # Change VM A=0 to A=1 immediately before the persistent order write.
  self.assertEqual(relocate(mutation={0x9686:0x41})[1]['after']['orders'],1)
 def test_body_cost_table_mutation_detected(self):
  self.assertEqual(relocate(mutation={0x7e9a:19})[1]['after']['abilities'][4],151)
 def test_original_later_country2_slot(self):
  c,r=relocate();order=[word(c,0x7b3a+2*i)+1 for i in range(14)]
  self.assertEqual(order,[5,12,13,1,3,9,10,4,6,7,2,8,11,14]);self.assertEqual(r['after']['slot'],3)
  self.assertEqual(schedule_select(c,10),(1,DST))
 def test_dispatch_tracks_swapped_ruler(self):
  c,r=relocate();self.assertFalse(dispatch(c,SRC)['player']);self.assertTrue(dispatch(c,DST)['player'])
  self.assertEqual(fresh_orders(c,DST),3)
 def test_delegated_role101_dispatch_negative_control(self):
  c=prepare(0x8735,baseline=True,byteset={0x65fb:0,0x65fc:101})
  self.assertFalse(dispatch(c,DST)['player']);c.m[0x65fc]=102;self.assertTrue(dispatch(c,DST)['player']);self.assertEqual(fresh_orders(c,DST),1)
 def test_role100_duplicate_not_automatically_repaired(self):
  c,r=relocate(role=100);self.assertEqual(r['after']['roles'],[100,100]);self.assertTrue(dispatch(c,SRC)['player'])
 def test_already_processed_destination_waits_for_next_schedule(self):
  c,_=relocate();put(c,0x7b3a+2*1,1);put(c,0x7b3a+2*10,11)
  order=[word(c,0x7b3a+2*i) for i in range(14)]
  self.assertNotIn(1,order[word(c,0x7b38)+1:]);self.assertEqual(schedule_select(c,4),(2,SRC+2*83))
 def test_new_base_sale_uses_destination_stock_and_gold(self):
  c,_=relocate();schedule_select(c,10);fresh_orders(c,DST);oldgold=word(c,SRC+64);oldstock=word(c,SRC+38)
  put(c,DST+38,word(c,DST+38)+1);g=word(c,DST+64);stock=word(c,DST+38);s=quote(c,0,2)
  self.assertEqual(sale_at(c,DST,2,1,s),{'stock':stock-1,'gold':g+s//10})
  self.assertEqual((word(c,SRC+64),word(c,SRC+38)),(oldgold,oldstock))
 def test_new_base_ability_pointer_resolves_temujin(self):
  c,_=relocate();schedule_select(c,10);resume(c,0x8235,bank=3)
  self.assertEqual(run(c,0x8241)['A'],0x65cd)
 def test_transfer_relocate_sell_full_selected_source_chain(self):
  for q in (1,3,26):
   r=transfer_relocate_sell(q)
   self.assertEqual(r['after_transfer']['jewels'],[26-q,29+q]);self.assertEqual(r['after_transfer']['orders'],2)
   self.assertEqual(r['after_move_base']['jewels'],[26-q,29+q]);self.assertEqual(r['after_move_base']['orders'],0)
   self.assertEqual(r['before_trade']['active'],1);self.assertEqual(r['before_trade']['orders'],3)
   self.assertEqual(r['after_trade']['jewels'],[26-q,29]);self.assertEqual(r['after_trade']['gold'],[439,66+(q*r['seller_quote'])//10])
   self.assertEqual(r['after_trade']['abilities'],[84,67,107,61,150,129]);self.assertEqual(r['after_trade']['orders'],2)

if __name__=='__main__':unittest.main(verbosity=2)
