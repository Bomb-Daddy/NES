#!/usr/bin/env python3
"""Hostile source staging, runtime source wiring, rollback, and refusal tests."""
import os,sys,tempfile,pathlib,shutil,hashlib
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
import stage307 as s
count=0
def verify(p):
 global count
 assert p, 'hostile QA failed'
 count+=1

def fixture(src,dir):
 dir.mkdir();
 for n in ('gk1_native_shell.c','CMakeLists.txt'):
  shutil.copyfile(src/n,dir/n)

if __name__=='__main__':
 if len(sys.argv)!=2:raise SystemExit('Usage test_stage307.py INHERITED_PRESTAGE_APP')
 src=pathlib.Path(sys.argv[1]);verify(s.digest(s.normalized(src/'gk1_native_shell.c'))==s.SHELL_SHA)
 with tempfile.TemporaryDirectory() as scratch:
  base=pathlib.Path(scratch)
  app=base/'ok';fixture(src,app)
  changes=s.stage(app);verify(len(changes)==5)
  verify(b'--physical-xbox-qa' in (app/'gk1_native_shell.c').read_bytes())
  verify('GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim' in (app/'CMakeLists.txt').read_text())
  verify(all(s.digest((app/n).read_bytes())==h for n,h in changes.items()))
  try:s.stage(app);verify(False)
  except ValueError:verify(True)
  for badname,badfile in [('shell', 'gk1_native_shell.c'),('cmake','CMakeLists.txt')]:
   d=base/badname;fixture(src,d)
   p=d/badfile
   if badname=='cmake':p.write_bytes(p.read_bytes().replace(b'target_link_libraries(gk1_native_shell PRIVATE gk99_xinput_bridge)',b'# MALICIOUS REMOVAL'))
   else:p.write_bytes(p.read_bytes()+b'\n/*HOSTILE*/')
   original={n:(d/n).read_bytes() for n in ('gk1_native_shell.c','CMakeLists.txt')}
   try:s.stage(d);verify(False)
   except ValueError:verify(True)
   verify(all((d/n).read_bytes()==b for n,b in original.items()))
   verify(not (d/'gk307_physical_pad_qa.c').exists())
  d=base/'malformed';fixture(src,d)
  p=d/'gk1_native_shell.c';p.write_bytes(p.read_bytes().replace(b'\n',b'\r\r\n',1))
  try:s.stage(d);verify(False)
  except ValueError:verify(True)
  verify(not (d/'gk307_physical_pad_qa.c').exists())
  d=base/'crlf';fixture(src,d)
  for n in ('gk1_native_shell.c','CMakeLists.txt'):
   p=d/n;p.write_bytes(p.read_bytes().replace(b'\n',b'\r\n'))
  verify(len(s.stage(d))==5)
  d=base/'rollback';fixture(src,d)
  original={n:(d/n).read_bytes() for n in ('gk1_native_shell.c','CMakeLists.txt')}
  try:s.stage(d,inject_failure_after=2);verify(False)
  except OSError:verify(True)
  verify(all((d/n).read_bytes()==b for n,b in original.items()))
  verify(not (d/'gk307_physical_pad_qa.c').exists())
 print('EXEC0307 hostile source staging PASS',count,'assertions; physical Xbox NOT RUN')
