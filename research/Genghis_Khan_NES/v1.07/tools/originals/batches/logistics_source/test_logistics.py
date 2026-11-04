import unittest,hashlib
from replay_logistics import *
OWNER_ADDR=[int.from_bytes(BASELINE[(SRC+83*i+81)-0x6000:(SRC+83*i+83)-0x6000],'little')+29 for i in range(14)]
class LogisticsChecks(unittest.TestCase):
 def test_rom_and_baseline_hashes(self):
  self.assertEqual(hashlib.sha256(t.h.ROM.read_bytes()).hexdigest(),t.h.EXPECTED)
  self.assertEqual(hashlib.sha256(BASELINE).hexdigest(),'dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773')
 def test_scalar_capacity_all_boundaries(self):
  for stock in (0,1,8,255,256,998,999):
   for dst in (0,1,255,998,999):self.assertEqual(scalar_cap(stock,dst),min(stock,999-dst))
 def test_gold_food_all_commodity_conservation(self):
  for offset in (64,54,*range(34,56,2)):
   for src,dst in ((1,0),(439,66),(255,255),(999,998),(999,0)):
    cap=min(src,999-dst)
    for q in set((0,1,cap)):
     if q<=cap:
      r=scalar_commit(src,dst,q,offset)
      self.assertEqual((r['source'],r['destination'],r['amount']),(src-q,dst+q,q));self.assertEqual(r['source']+r['destination'],src+dst)
      self.assertTrue(set(r['changed_addresses']).issubset({SRC+offset,SRC+offset+1,DST+offset,DST+offset+1}))
 def test_no_scalar_random_transport_loss(self):
  for q in (1,2,7,8,17,100,499,999):
   r=scalar_commit(999,0,q);self.assertEqual((999-r['source'],r['destination']),(q,q))
 def test_return_transfer_reverses_scalar_ledger(self):
  for offset in (64,54,*range(34,54,2)):
   a=scalar_commit(439,66,17,offset);b=scalar_commit(a['destination'],a['source'],17,offset)
   self.assertEqual((b['destination'],b['source']),(439,66))
 def test_transfer_mutated_add_to_subtract_detected(self):
  good=scalar_commit(439,66,10);bad=scalar_commit(439,66,10,mutation={0x9628:0xbc})
  self.assertEqual(good['destination'],76);self.assertEqual(bad['destination'],56);self.assertNotEqual(bad['source']+bad['destination'],505)
 def test_capacity_operand_negative_control(self):
  c=prepare(0x95fc,[SRC+64,DST+64,998],words={SRC+64:100},byteset={0x95fd:0xe6});run(c,0x960a);self.assertEqual(word(c,FRAME-2),0)
 def test_cancel_does_not_write_sram(self):self.assertEqual(scalar_cancel(439,66),(0,True))
 def test_quantity_min_max_and_cancel_source_gate(self):
  for q in (0,1,8,9,65535):
   c=prepare(0xd3a1,[1,8],locals={-10:q});r=run(c,(0xd37c,0xd3b4));self.assertEqual(r['stop']==0xd3b4,q in (1,8,65535))
 def test_people_all_five_categories_conserved(self):
  for category in range(5):
   s=[21,22,23,24,25];d=[31,32,33,34,35];q=7
   r=people(s,d,[110,124,118],[150,80,100],category,q)
   es=s.copy();ed=d.copy();es[category]-=q;ed[category]+=q
   self.assertEqual(r['source']['counts'],es);self.assertEqual(r['destination']['counts'],ed)
   self.assertEqual(sum(r['source']['counts'])+sum(r['destination']['counts']),sum(s)+sum(d))
   self.assertEqual(r['source']['quality'],[110,124,118]);self.assertEqual(r['source']['eligibility'],32);self.assertEqual(r['destination']['eligibility'],27);self.assertEqual(r['source']['civilians'],34);self.assertEqual(r['destination']['civilians'],43)
   self.assertEqual(r['destination']['quality'],[(old*sum(d)+incoming*q)//(sum(d)+q) for old,incoming in zip([150,80,100],[110,124,118])])
 def test_people_destination_total_capacity_not_category(self):
  s=[100,0,0,0,0];d=[10,988,0,0,0];self.assertEqual(scalar_cap(s[0],sum(d)),1)
  r=people(s,d,[100,100,100],[100,100,100],0,1);self.assertEqual(sum(r['destination']['counts']),999)
 def test_people_empty_source_zeroes_arms_skill_before_average(self):
  r=people([8,0,0,0,0],[10,0,0,0,0],[110,124,118],[150,80,100],0,8)
  self.assertEqual(r['source']['quality'],[0,0,118]);self.assertEqual(r['destination']['quality'],[83,44,108])
 def test_people_nonempty_other_workforce_retains_source_quality(self):
  r=people([8,1,0,0,0],[10,0,0,0,0],[110,124,118],[150,80,100],0,8)
  self.assertEqual(r['source']['quality'],[110,124,118]);self.assertEqual(r['destination']['quality'],[132,99,108])
 def test_people_empty_destination(self):
  r=people([9,0,0,0,0],[0]*5,[110,124,118],[999,999,999],0,8);self.assertEqual(r['destination']['quality'],[110,124,118])
 def test_people_both_source_and_destination_empty_after_full_migration_case(self):
  r=people([8,0,0,0,0],[0]*5,[110,124,118],[999,999,999],0,8);self.assertEqual(r['destination']['quality'],[0,0,118])
 def test_people_quality_mutation_detected(self):
  good=people([9,0,0,0,0],[10,0,0,0,0],[110,124,118],[150,80,100],0,8)
  bad=people([9,0,0,0,0],[10,0,0,0,0],[110,124,118],[150,80,100],0,8,mutation={0x95bf:22})
  self.assertNotEqual(good['destination']['quality'][0],bad['destination']['quality'][0])
 def test_people_and_scalar_switch_targets(self):
  for selection,stop in ((1,0x96d4),(2,0x971b),(3,0x9725),(4,0x972f),(5,0x9739),(0xffff,0x9743)):
   c=prepare(0x96c3);put(c,8,selection);r=run(c,(0x96d4,0x971b,0x9725,0x972f,0x9739,0x9743));self.assertEqual(r['stop'],stop)
  for selection,stop in ((3,0x977d),(4,0x97b1),(5,0x97c6),(0xffff,0x97da)):
   c=prepare(0x976b,[SRC,selection]);r=run(c,(0x977d,0x97b1,0x97c6,0x97da));self.assertEqual(r['stop'],stop)
 def test_no_specialty_gate_for_generic_stock_pointer(self):
  for i in range(11):
   c=prepare(0xdf0a,[DST,i]);r=run(c,0xdf10);self.assertEqual(r['A'],DST+34+2*i)
 def test_original_ownership_country2_intervention(self):
  self.assertEqual(owned_source(1),0);self.assertEqual(owned_source(1,owner_mutations={OWNER_ADDR[1]:0}),1)
 def test_source_picker_all_friendly_not_limited_to_neighbors(self):
  patches={a:0 for a in OWNER_ADDR}
  for idx in range(14):self.assertEqual(owned_source(idx,owner_mutations=patches),1)
 def test_destination_full14country_adjacency_matrix(self):
  patches={a:0 for a in OWNER_ADDR}
  for s in range(14):
   adj=list(BASELINE[0x7693-0x6000+12*s:0x7693-0x6000+12*s+7])
   for d in range(14):self.assertEqual(destination_gate(s,d,patches),int(d in adj))
 def test_enemy_adjacent_rejected_and_friend_accepted(self):
  self.assertEqual(destination_gate(0,1),0);self.assertEqual(destination_gate(0,1,{OWNER_ADDR[1]:0}),1)
 def test_friend_nonadjacent_rejected(self):self.assertEqual(destination_gate(0,2,{OWNER_ADDR[2]:0}),0)
 def test_self_destination_rejected_by_adjacency(self):self.assertEqual(destination_gate(0,0),0)
 def test_direction_uses_source_list(self):
  patches={OWNER_ADDR[1]:0}
  self.assertEqual(destination_gate(0,1,patches,{0x7693:0xff}),0)
  self.assertEqual(destination_gate(1,0,patches,{0x7693:0xff}),1)
 def test_adjacency_early_sentinel_and_last_slot(self):
  patches={OWNER_ADDR[1]:0};empty={0x7693+i:0xff for i in range(7)}
  self.assertEqual(destination_gate(0,1,patches,empty),0)
  end={0x7693+i:13 for i in range(6)};end[0x7699]=1;self.assertEqual(destination_gate(0,1,patches,end),1)
 def test_any_friendly_neighbor_preflight(self):
  self.assertEqual(any_owned_neighbor(0),0);self.assertEqual(any_owned_neighbor(0,{OWNER_ADDR[1]:0}),1)
 def test_initial_only_base_count_and_control(self):
  self.assertEqual(owned_count(),1);self.assertEqual(owned_count({OWNER_ADDR[1]:0}),2)
 def test_main_move_requires_ruler_role100(self):
  for role in (100,101,102):
   c=prepare(0x82f7,[SRC],baseline=True,byteset={0x65dd:role});r=run(c,(0x8300,0x8301));self.assertEqual(r['stop']==0x8300,role==100)
 def test_original_ability_rows(self):
  self.assertEqual(list(BASELINE[0x7e18-0x6000+22*6:0x7e18-0x6000+23*6]),[10,0,0,10,0,0]);self.assertEqual(list(BASELINE[0x7e18-0x6000+23*6:0x7e18-0x6000+24*6]),[5,5,5,0,0,0])
 def test_success_costs_and_one_order(self):
  self.assertEqual(cost(22,[100]*6),{'abilities':[90,100,100,90,100,100],'orders':2})
  self.assertEqual(cost(23,[100]*6),{'abilities':[95,95,95,100,100,100],'orders':2})
 def test_cancel_has_zero_ability_and_order_cost(self):
  for code in (22,23):self.assertEqual(cost(code,[100]*6,success=False),{'abilities':[100]*6,'orders':3})
 def test_ability_boundaries_each_required_attribute(self):
  for code,required in ((22,[10,0,0,10,0,0]),(23,[5,5,5,0,0,0])):
   self.assertTrue(ability_gate(code,required))
   for i,n in enumerate(required):
    if n:
     lower=required.copy();lower[i]-=1;self.assertFalse(ability_gate(code,lower))
 def test_actor_cost_independent_of_transport_quantity(self):
  for q in (1,17,999):
   scalar_commit(999,0,q);self.assertEqual(cost(23,[100]*6)['abilities'],[95,95,95,100,100,100])
 def test_population_ledger_and_production_history_conservation(self):
  for category in range(5):
   r=people([21,22,23,24,25],[31,32,33,34,35],[110,124,118],[150,80,100],category,7)
   self.assertEqual(sum(r['source']['counts'])+r['source']['civilians']+sum(r['destination']['counts'])+r['destination']['civilians'],115+34+165+43)
   for country in ('source','destination'):self.assertEqual(r[country]['production_history'],[17,23])
   allowed=set()
   for p in (SRC,DST):
    for off in ((24,28,32,58,62)[category],20,22,66):allowed.update((p+off,p+off+1))
   self.assertTrue(set(r['changed_addresses']).issubset(allowed))
 def test_ability_pointer_uses_7b34_not_7b66(self):
  for bank,start,stop in ((3,0x8235,0x8241),(0,0x8b0b,0x8b17)):
   c=prepare(start,bank=bank,baseline=True,words={0x7b66:0,0x7b34:1},byteset={OWNER_ADDR[1]:0})
   r=run(c,stop);self.assertEqual(r['A'],0x65de+14)
 def test_destination_picker_restores_original_active_country(self):
  c=prepare(0x9596,words={0x7b34:1},locals={-4:0,-2:DST});r=run(c,0x959b)
  self.assertEqual(word(c,0x7b34),0);self.assertEqual(r['A'],DST)
if __name__=='__main__':unittest.main(verbosity=2)
