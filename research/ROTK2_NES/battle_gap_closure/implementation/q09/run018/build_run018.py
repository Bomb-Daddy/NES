#!/usr/bin/env python3
"""Rebuild Classic Run018 from the immutable exact Run017 full source archive.

Usage: python build_run018.py --parent RUN017.zip --engine engine.py
    --test test_run018_q09_army_immediate_source_insertion.py --output RUN018.zip
"""
import argparse,hashlib,pathlib,zipfile,sys
PARENT_SHA='30c5dc01da6fe2a5f827d023fb926712d743dc5d1bc1d8ce826f632b484d1f8e'
ENGINE='src/rotk2/battle/engine.py'
TEST='src/tests/test_run018_q09_army_immediate_source_insertion.py'
def sha(p):return hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest()
def main():
 parser=argparse.ArgumentParser()
 for x in ('parent','engine','test','output'):parser.add_argument('--'+x,required=True)
 a=parser.parse_args()
 if sha(a.parent)!=PARENT_SHA:sys.exit('BLOCKED_PARENT_HASH_NOT_RUN017')
 with zipfile.ZipFile(a.parent) as src:
  if src.testzip() is not None:sys.exit('BLOCKED_PARENT_CRC')
  names=src.namelist()
  if names.count(ENGINE)!=1 or TEST in names:sys.exit('BLOCKED_PARENT_MEMBERS')
  with zipfile.ZipFile(a.output,'w') as dst:
   for info in src.infolist():
    content=pathlib.Path(a.engine).read_bytes() if info.filename==ENGINE else src.read(info.filename)
    dst.writestr(info,content)
   item=zipfile.ZipInfo(TEST,date_time=(2026,11,6,14,41,0))
   item.compress_type=zipfile.ZIP_DEFLATED
   item.external_attr=0o100644<<16
   dst.writestr(item,pathlib.Path(a.test).read_bytes())
 with zipfile.ZipFile(a.output) as z:
  assert z.testzip() is None and len(z.infolist())==len(names)+1
 print(sha(a.output))
if __name__=='__main__':main()
