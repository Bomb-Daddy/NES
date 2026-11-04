import unittest,json,hashlib
from roi_model import *

class RoiTests(unittest.TestCase):
 def test_01_runtime_capture_chain_reconciled(self):
  predicted=simulate(PLANS[0])['seasons']
  for i,n in enumerate(NAMES):
   d=(Path(os.environ['GK_ROI_EVIDENCE'])/f'{n}.sram').read_bytes();expected=state(d)
   self.assertEqual(predicted[i],expected,n);self.assertEqual(word(d,0x7a8a),RATES[i])
 def test_02_every_policy_and_season_bound_to_original_cpu(self):
  records=[]
  for p in PLANS:
   actual,steps=original(p);expected=simulate(p)['seasons'];self.assertEqual(actual,expected,p[0]);records.append({'policy':p[0],'original_cpu_steps':steps,'seasons':actual})
  for p in (PLANS[1],PLANS[4]):
   actual,steps=original(p,liquidate=True);expected=simulate(p,liquidate=True)['seasons'];self.assertEqual(actual,expected,p[0]+'_sell_extra');records.append({'policy':p[0]+'_sell_extra','original_cpu_steps':steps,'seasons':actual})
  (ROOT/'source_policy_replays.json').write_text(json.dumps(records,indent=2)+'\n')
 def test_03_history_timing_and_troop_conservation(self):
  base=simulate(PLANS[0]);a=simulate(PLANS[1]);f=simulate(PLANS[2]);t=simulate(PLANS[3])
  self.assertEqual(a['seasons'][-1]['fur']-base['seasons'][-1]['fur'],20)
  self.assertEqual(a['seasons'][-1]['commodity_history']-base['seasons'][-1]['commodity_history'],20)
  self.assertEqual(f['seasons'][-1]['food']-base['seasons'][-1]['food'],20)
  self.assertEqual(f['seasons'][-1]['food_history']-base['seasons'][-1]['food_history'],20)
  for r in (a,f,t):
   self.assertEqual(population(r['seasons'][0]),103);self.assertEqual(r['seasons'][-1]['troops'],52)
   self.assertEqual(r['spring']['troop_payroll'],51)
 def test_04_money_is_not_imputed_stock_value(self):
  stored=simulate(PLANS[1]);sold=simulate(PLANS[1],liquidate=True)
  self.assertEqual(stored['seasons'][-1]['gold'],459);self.assertEqual(sold['seasons'][-1]['gold'],493)
  self.assertEqual(sold['transactions'][-1]['gold_credit'],34);self.assertEqual(sold['orders_total'],stored['orders_total']+1)
  self.assertEqual(sold['ability_cost_total'][1],stored['ability_cost_total'][1]+5)
 def test_05_observed_offer_and_quote_feasibility(self):
  summer=(Path(os.environ['GK_ROI_EVIDENCE'])/'summer_step3.sram').read_bytes();fall=(Path(os.environ['GK_ROI_EVIDENCE'])/'fall1175_ready.sram').read_bytes()
  for d,i,buy in ((summer,10,True),(fall,0,False)):
   self.assertNotEqual(word(d,B+0x4b),0);self.assertEqual(word(d,(0x7ace if buy else 0x7a8c)+2*i),1)
  self.assertEqual(quote(99,0,10),19);self.assertEqual(quote(91,0,0,True),17)
  # Baseline China Food offer is absent, so pretending to buy there at Spring price is invalid.
  self.assertEqual(word(RAW,0x7ace+20),0)
 def test_06_hire_is_costed_and_morale_changes_tax(self):
  r=simulate(PLANS[4]);s=r['seasons'][-1]
  self.assertEqual(r['transactions'][0]['gold_cost'],120);self.assertIn(r['seasons'][0]['morale'],(111,112))
  self.assertEqual(r['spring']['gross_tax'],58);self.assertEqual(r['spring']['food_used'],56)
  self.assertEqual((s['gold'],s['troops'],population(s)),(326,62,116))
  self.assertEqual(r['ability_cost_total'],[5,5,5,0,0,0])
 def test_07_caps_and_sales_limit_negative_controls(self):
  base=simulate(PLANS[0],{'food':980});f=simulate(PLANS[2],{'food':980})
  self.assertEqual(base['seasons'][-1]['food'],f['seasons'][-1]['food'])
  base=simulate(PLANS[0],{'fur':980});a=simulate(PLANS[1],{'fur':980});sold=simulate(PLANS[1],{'fur':980},liquidate=True)
  self.assertEqual(a['seasons'][-1]['fur']-base['seasons'][-1]['fur'],1)
  self.assertEqual(sold['transactions'][-1]['gold_credit'],1)
  capped=simulate(PLANS[1],{'gold':990},common_gold10=False,liquidate=True)
  self.assertEqual(capped['transactions'][-1]['quantity'],5);self.assertEqual(capped['seasons'][-1]['gold'],999)
 def test_08_mysterious_gold10_is_explicit_exogenous_not_production(self):
  for p in PLANS:
   a=simulate(p,common_gold10=True);b=simulate(p,common_gold10=False)
   self.assertEqual(a['seasons'][-1]['gold']-b['seasons'][-1]['gold'],10)
   # This isolated difference cancels in policy comparisons at uncapped baseline values.
 def test_09_order_budget_and_costs(self):
  for p in PLANS:
   for liquidation in (False,True):
    r=simulate(p,liquidate=liquidation);seasons={}
    for cmd in r['commands']:seasons[cmd['season']]=seasons.get(cmd['season'],0)+cmd['orders']
    self.assertTrue(all(v<=3 for v in seasons.values()));self.assertTrue(all(v>=0 for v in r['seasons'][-1]['stats']))
 def test_10_food_shortage_changes_payroll_dependency(self):
  # Source-bound independent leaf checks show why low-food cases cannot use unchanged workers/payroll.
  s=state();s['food']=0;s['food_history']=0;s['foodworkers']=0
  c=Cpu(1);c.m[0x6000:0x8000]=RAW
  for k,o in FIELDS.items():c.setword(B+o,s[k])
  c.setup(0x809e,[B]);c.run(0x8105,trace=False)
  model=copy.deepcopy(s)
  spring(model)
  self.assertEqual(population({k:c.word(B+o) for k,o in FIELDS.items()}),0)
  self.assertEqual(model['troops'],1) # later annual under20 growth; no paid troops
 def test_11_source_mutation_breaks_modeled_town_projection(self):
  c=Cpu(1);c.m[0x6000:0x8000]=RAW;c.setword(B+0x1c,15);c.setup(0x9034);c.setword(0x71f4,B);c.m[0x904e]=0x65;c.run(0x907a,trace=False)
  s=state();s['town']=15;common(s)
  self.assertNotEqual(c.word(B+0x1a),s['economy']);self.assertEqual((c.word(B+0x1a),s['economy']),(191,193))

 def test_12_price_quotes_executed_at_observed_rates(self):
  for rate in RATES:
   for item in (0,10):
    for selling in (False,True):
     c=Cpu(3);c.m[0x6000:0x8000]=RAW;c.setword(0x7a8a,rate);c.setup(0xdd06 if selling else 0xdcea,[0,item]);out=c.run(0xdd19 if selling else 0xdd00,trace=False)
     self.assertEqual(out,quote(rate,0,item,selling))
 def test_13_economy_cap_can_remove_marginal_town_yield(self):
  for town in (40,50):
   c=Cpu(1);c.m[0x6000:0x8000]=RAW;c.setword(B+0x1a,999);c.setword(B+0x1c,town);c.setup(0x9034);c.setword(0x71f4,B);c.run(0x907a,trace=False)
   s=state();s.update(economy=999,town=town);common(s)
   self.assertEqual(c.word(B+0x1a),s['economy']);self.assertEqual(s['economy'],999)

if __name__=='__main__':
 r=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(RoiTests));raise SystemExit(not r.wasSuccessful())
