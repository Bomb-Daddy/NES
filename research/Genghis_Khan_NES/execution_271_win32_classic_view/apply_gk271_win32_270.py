#!/usr/bin/env python3
"""Exact EXEC0269 -> EXEC0270 native game transformation, never a silent no-op."""
import hashlib
from pathlib import Path
import sys

BEFORE='a0edd0a5dc4974a17485320368c6613240785a2c813a941dcaef527fd632c05c'
AFTER='ee00ef0fc03676d09edfef9ca630acb9e605f128e5af5a66b1eafb2e86c2dc68'
REPLACEMENTS=(
 (b'   if(s->stock_reconcile_view==3u){',
  b'   /* EXEC0270: a read-only VIEW mode change cancels an armed Pass. An A\n'
  b'    * after switching views must arm anew, never spend the Spring budget. */\n'
  b'   s->stock_checkpoint.pending_pass=0u;\n'
  b'   if(s->stock_reconcile_view==3u){'),
 (b'  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){\n'
  b'   cp->view_index=(uint8_t)((cp->view_index+cp->world.country_count-1u)%cp->world.country_count);return 1;',
  b'  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){\n'
  b'   /* EXEC0270: browsing another verified country is not confirmation of\n'
  b'    * a pending Pass. Disarm atomically before the country changes. */\n'
  b'   cp->pending_pass=0u;\n'
  b'   cp->view_index=(uint8_t)((cp->view_index+cp->world.country_count-1u)%cp->world.country_count);return 1;'),
 (b'  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){\n'
  b'   cp->view_index=(uint8_t)((cp->view_index+1u)%cp->world.country_count);return 1;',
  b'  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){\n'
  b'   cp->pending_pass=0u;\n'
  b'   cp->view_index=(uint8_t)((cp->view_index+1u)%cp->world.country_count);return 1;')
)

def digest(raw):
 return hashlib.sha256(raw).hexdigest()

def transform(raw):
 if digest(raw)!=BEFORE: raise ValueError('preimage mismatch: no source was modified')
 for old,new in REPLACEMENTS:
  if raw.count(old)!=1: raise ValueError('ambiguous exact source anchor')
  raw=raw.replace(old,new,1)
 if digest(raw)!=AFTER: raise ValueError('unexpected postimage: no source was modified')
 return raw

def hostile(raw):
 assert digest(raw)==BEFORE and digest(transform(raw))==AFTER
 mutations=(raw+b' ',raw.replace(b'pending_pass',b'pending_fake',1),
            raw.replace(b'view_index',b'view_idxXX',1),
            raw.replace(b'if(s->stock_reconcile_view==3u)',b'if(s->stock_reconcile_view==4u)',1),
            raw+REPLACEMENTS[0][0],transform(raw))
 for i,bad in enumerate(mutations):
  try: transform(bad)
  except ValueError: continue
  raise AssertionError('hostile mutation %d accepted'%i)
 print('EXEC0271 six hostile variants and exact source SHA checks PASS')

if __name__=='__main__':
 if len(sys.argv) not in (2,3): sys.exit('usage: SCRIPT SOURCE.c [--self-test]')
 path=Path(sys.argv[1]);raw=path.read_bytes()
 if len(sys.argv)==3:
  if sys.argv[2]!='--self-test': sys.exit('unknown option')
  hostile(raw)
 else:
  updated=transform(raw)
  path.write_bytes(updated)
  if path.read_bytes()!=updated: raise RuntimeError('source readback mismatch')
  print('EXEC0271 exact linked EXEC0270 source SHA '+AFTER)
