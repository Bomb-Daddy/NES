#!/usr/bin/env python3
import unittest,hashlib,json,os
from pathlib import Path
import replay_identity as r
ROOT=Path(__file__).resolve().parent
BASE=ROOT.parents[1]
class IdentityChecks(unittest.TestCase):
 def test_rom_identity(self):
  self.assertEqual(hashlib.sha256(r.h.ROM.read_bytes()).hexdigest(),'ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3')
 def test_manual_identity(self):
  self.assertEqual(hashlib.sha256((ROOT/'Genghis_Khan_NES_USA_manual_archive.pdf').read_bytes()).hexdigest(),'ecc2150b0ef43861b3590fa8d1671b99fe5f486894b1289836eb77265f49d0ae')
 def test_original_stock_address_bytes(self):
  self.assertEqual(r.h.FIX[0xdf05-0xc000:0xdf11-0xc000],bytes.fromhex('20 85 ef 00 00 0d d2 1c bb 8f 22 cf'))
 def test_all_eleven_stock_addresses(self):
  for i,a in enumerate([0x6def,0x6df1,0x6df3,0x6df5,0x6df7,0x6df9,0x6dfb,0x6dfd,0x6dff,0x6e01,0x6e03]):
   self.assertEqual(r.run_addr(i)['address'],hex(a))
 def test_stock_address_base_dependence(self):
  for base in (0x6100,0x6dcd,0x7000):
   for i in range(11): self.assertEqual(r.run_addr(i,base)['address'],hex(base+34+2*i))
 def test_all_seventeen_quote_coordinates(self):
  xy=[(14,y) for y in (9,11,13,15,17,19)]+[(20,y) for y in (9,11,13,15,17)]+[(26,y) for y in (9,11,13,15,17,19)]
  for i,(x,y) in enumerate(xy):
   v=r.run_layout(i);self.assertEqual((v['quote_x_tile'],v['quote_y_tile']),(x,y))
 def test_weapon_branch_separates_arms_from_stock(self):
  for i in range(17): self.assertEqual(r.run_route(i)['path'],'stock' if i<11 else 'arms')
 def test_weapon_shared_arms_address(self):
  self.assertEqual(r.run_arms_address(),0x6de1)
  self.assertEqual(r.run_arms_address(0x7000),0x7014)
 def test_stock_operand_mutation_negative(self):
  c=r.initial(0xdf0a);c.m[0x720b:0x720d]=(0x6dcd).to_bytes(2,'little');c.m[0x720d:0x720f]=(2).to_bytes(2,'little');c.m[0xdf0f]=0x24
  while not(c.pc==0xefc9 and c.stream()==0xdf10):c.step()
  self.assertEqual(c.readind(8),0x6df5);self.assertNotEqual(c.readind(8),0x6df3)
 def test_wrong_layout_column_mutation_negative(self):
  c=r.initial(0x816e);c.m[0x71fa:0x71fc]=(11).to_bytes(2,'little');c.m[0x816f]=25
  while not(c.pc==0xefc9 and c.stream()==0x8192):c.step()
  self.assertEqual(int.from_bytes(c.m[0x71f8:0x71fa],'little'),25)
  self.assertNotEqual(int.from_bytes(c.m[0x71f8:0x71fa],'little'),r.run_layout(11)['quote_x_tile'])
 def test_known_runtime_transactions_match_addresses(self):
  def word(file,address):
   b=(BASE/'evidence/roundtrip'/file).read_bytes();return int.from_bytes(b[address-0x6000:address-0x6000+2],'little')
  self.assertEqual((word('baseline.sram',0x6df3),word('buy1_result.sram',0x6df3)),(26,27))
  self.assertEqual((word('baseline.sram',0x6df9),word('food_settled.sram',0x6df9)),(50,51))
  self.assertEqual((word('baseline.sram',0x6e03),word('food_settled.sram',0x6e03)),(208,208))
 def test_seller_coordinates_match_all_eleven_commodities(self):
  for i in range(11):
   a=r.run_layout(i);b=r.run_seller_layout(i)
   self.assertEqual((a['quote_x_tile'],a['quote_y_tile']),(b['quote_x_tile'],b['quote_y_tile']))
if __name__=='__main__': unittest.main(verbosity=2)
