#!/usr/bin/env python3
"""Bounded original-ROM CPU replay for item index/layout/address identity. No Mesen or console emulation."""
import os,importlib.util,json,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parent
BASE=ROOT.parents[1]
assert os.environ.get('GK_ROM'), 'Set GK_ROM explicitly'
p=BASE/'output/research/Genghis_Khan_NES/v1.06/tools/replay_b7_arithmetic.py'
spec=importlib.util.spec_from_file_location('helpers',p);h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h)

def initial(start):
 c=h.HelperCpu();c.m[0x8000:0xc000]=h.PRG[3*0x4000:4*0x4000]
 c.m[6:8]=start.to_bytes(2,'little');c.m[2:4]=(0x7800).to_bytes(2,'little');c.m[4:6]=(0x7200).to_bytes(2,'little')
 return c

def run_addr(index,base=0x6dcd,trace=False):
 c=initial(0xdf0a);c.m[0x720b:0x720d]=base.to_bytes(2,'little');c.m[0x720d:0x720f]=index.to_bytes(2,'little')
 out=[]
 while c.count<10000:
  if c.pc==0xefc9:
   if trace:out.append({'vm':hex(c.stream()),'A':c.readind(8),'B':c.readind(12)})
   if c.stream()==0xdf10:return {'index':index,'country_base':hex(base),'address':hex(c.readind(8)),'trace':out,'steps':c.count}
  c.step()
 raise RuntimeError('budget')

def run_layout(index):
 c=initial(0x816e);c.m[0x71fa:0x71fc]=index.to_bytes(2,'little')
 out=[]
 while c.count<10000:
  if c.pc==0xefc9:
   out.append({'vm':hex(c.stream()),'A':c.readind(8),'B':c.readind(12)})
   if c.stream()==0x8192:
    x=int.from_bytes(c.m[0x71f8:0x71fa],'little');y=int.from_bytes(c.m[0x71f6:0x71f8],'little')
    return {'index':index,'quote_x_tile':x,'quote_y_tile':y,'quote_x_pixel':x*8,'quote_y_pixel':y*8,'trace':out,'steps':c.count}
  c.step()
 raise RuntimeError('budget')


def run_route(index):
 c=initial(0x8cfb);c.m[0x71fe:0x7200]=index.to_bytes(2,'little')
 while c.count<10000:
  if c.pc==0xefc9 and c.stream() in (0x8d01,0x8d06):
   return {'index':index,'path':'stock' if c.stream()==0x8d01 else 'arms','boundary':hex(c.stream()),'steps':c.count}
  c.step()
 raise RuntimeError('budget')

def run_arms_address(base=0x6dcd):
 c=initial(0x8d1d);c.m[0x720b:0x720d]=base.to_bytes(2,'little')
 while c.count<10000:
  if c.pc==0xefc9 and c.stream()==0x8d20:return c.readind(8)
  c.step()
 raise RuntimeError('budget')

def run_seller_layout(index):
 if not 0<=index<=10: raise ValueError('Seller menu has eleven stocked commodities')
 c=initial(0x80fb);c.m[0x71fc:0x71fe]=index.to_bytes(2,'little');c.m[0x71fa:0x71fc]=(14).to_bytes(2,'little')
 while c.count<10000:
  if c.pc==0xefc9 and c.stream()==0x8111:
   return {'index':index,'quote_x_tile':int.from_bytes(c.m[0x71fa:0x71fc],'little'),'quote_y_tile':int.from_bytes(c.m[0x71f8:0x71fa],'little'),'steps':c.count}
  c.step()
 raise RuntimeError('budget')

if __name__=='__main__':
 outputs={
  'stock_address_replay.json':[run_addr(i,trace=True) for i in range(11)],
  'menu_layout_replay.json':[run_layout(i) for i in range(17)],
  'seller_layout_replay.json':[run_seller_layout(i) for i in range(11)],
  'buyer_routing_replay.json':[run_route(i) for i in range(17)]}
 for name,data in outputs.items():
  (ROOT/name).write_text(json.dumps(data,indent=2)+'\n')
  print(name,len(data),'cases')
 print('Arms destination',hex(run_arms_address()))
