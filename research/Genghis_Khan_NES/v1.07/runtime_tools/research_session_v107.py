"""Drive the private original ROM; record every request and response.

Usage: GK_ROM=/private/game.nes python3 -u research_session.py OUTPUT_DIR
JSON lines on stdin: boot, press, wait, snapshot, tool, close.
"""
import os,sys,json,time,hashlib,traceback,runpy
from pathlib import Path
from PIL import Image
ROOT=Path(__file__).resolve().parents[2]
TOOL=ROOT/'recovery/mesen'
sys.path.insert(0,str(ROOT/'batches/runtime/mesen-for-ai/skills/mesen-emulator/scripts'))
os.environ.update(MESEN_BIN=str(TOOL/'mesen/Mesen'),MESEN_FOR_AI_REPO=str(ROOT/'batches/runtime/mesen-for-ai'),SDL_AUDIODRIVER='dummy')
from mesen_client import Mesen
OUT=Path(sys.argv[1]).resolve();OUT.mkdir(parents=True,exist_ok=True)
PREFIX=[('start',5,40)]+[('a',5,40)]*10+[('b',20,100),('a',20,100),('start',20,100),('b',20,100)]+[('a',20,100)]*8+[('wait',300,0),('b',20,80),('wait',300,0),('a',20,80),('wait',300,0),('start',20,80),('wait',300,0)]
log_path=OUT/os.environ.get('GK_EVENT_LOG','events.jsonl')
def emit(v):
 print(json.dumps(v),flush=True)
def record(kind,value):
 with log_path.open('a') as log:
  log.write(json.dumps({'kind':kind,'value':value})+'\n')
with Mesen() as m:
 def call(name,**kw):
  record('request',{'tool':name,**kw})
  v=m.tool(name,**kw);record('response',v);return v
 def step(n,reset=False):return call('run.step_frames',frames=n,reset=reset)
 def press(key,hold=5,pause=50):
  call('input.set',port=0,subport=0,buttons={key:True});step(hold)
  call('input.set',port=0,subport=0,buttons={});return step(pause)
 def snap(label):
  call('video.export_frame',path=str(OUT/(label+'.ppm')))
  Image.open(OUT/(label+'.ppm')).save(OUT/(label+'.png'))
  ram=bytes(call('cpu.read_memory',memoryType='nesMemory',address=0x6000,length=8192)['bytes'])
  (OUT/(label+'.sram')).write_bytes(ram)
  def b(a):return ram[a-0x6000]
  def w(a):return int.from_bytes(ram[a-0x6000:a-0x6000+2],'little')
  s={'label':label,'frame':call('run.status')['frame'],'gold':b(0x6e0d)+256*b(0x6e0e),'food':w(0x6e03),'troops':w(0x6de5),'fur':w(0x6def),'rate':w(0x7a8a),'orders':b(0x7b36),'image':str(OUT/(label+'.png')),'sram_sha256':hashlib.sha256(ram).hexdigest()}
  zero=bytes(call('cpu.read_memory',memoryType='nesMemory',address=0,length=256)['bytes']); (OUT/(label+'.zeropage')).write_bytes(zero)
  s['rng_seed_hex']=zero[0x84:0x8a].hex();s['year']=w(0x65bb);s['scenario_count']=w(0x65b9);s['abilities']=[w(w(0x6e1e)+0x0e+2*i) for i in range(6)];s['reserve']=w(0x6e16);s['eligible_percent']=b(0x6e15)
  s['season']=w(0x65bd);s['secondary']=w(0x7a88);s['country_words']={hex(a):w(a) for a in range(0x6dcd,0x6e4d,2)}
  (OUT/(label+'.json')).write_text(json.dumps(s,indent=2));return s
 loaded=m.load_rom(os.environ['GK_ROM'],timeout=14400);record('loaded',loaded);emit({'ready':loaded['romInfo']})
 for line in sys.stdin:
  try:
   a=json.loads(line);record('action',a);op=a.pop('op')
   if op=='close':break
   if op=='boot':
    step(180,True)
    for key,n,pause in PREFIX:
     step(n) if key=='wait' else press(key,n,pause)
    v=snap('baseline')
   elif op=='press':v=press(**a)
   elif op in ('checkpoint','restore'):
    call('input.set',buttons={});call('state.checkpoint' if op=='checkpoint' else 'state.restore',path=str(OUT/(a['name']+'.mss')));step(2);v=call('state.status')
   elif op=='sequence':
    for key in a['keys']:
     press(key)
    v=snap(a['label'])
   elif op=='wait':v=step(a['frames'])
   elif op=='snapshot':v=snap(a['label'])
   elif op=='tool':v=call(a.pop('name'),**a)
   elif op=='batch':
    env=runpy.run_path(a['path'],init_globals={'call':call,'press':press,'step':step,'snap':snap,'OUT':OUT,'ROOT':ROOT});v=env.get('RESULTS','Batch finished')
   else:raise ValueError(op)
   emit({'ok':True,'result':v})
  except Exception as e:
   record('error',traceback.format_exc());emit({'error':str(e)})
