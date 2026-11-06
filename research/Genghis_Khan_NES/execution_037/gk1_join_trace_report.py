#!/usr/bin/env python3
"""EXEC0037 candidate call-order report for a future private Mesen join trace.
A passive decoder, NOT a simulator. Synthetic fixture tests never claim a
naturally reached VM source writer. No copyrighted bytes needed.
"""
import argparse
import json
from pathlib import Path

class TraceFormatError(ValueError): pass
ALLOWED={'unit_record_write','word_613e_write','scratch_write',
         'native_handler_F1C8','native_handler_F1CD'}
def validate_trace(lines):
    out=[]
    last=(-1,-1)
    for idx,line in enumerate(lines,1):
        if not line.strip():continue
        try: e=json.loads(line)
        except (TypeError,ValueError) as ex:raise TraceFormatError(f'line {idx} JSON') from ex
        required=('kind','frame','cycle','cpu_pc','vm_source_pointer',
                  'vm_source_prg_offset','unit0','unit1','scratch_word',
                  'scratch_614c','scratch_614e')
        if not isinstance(e,dict) or any(x not in e for x in required):
            raise TraceFormatError(f'line {idx} missing fields')
        if e['kind'] not in ALLOWED:raise TraceFormatError(f'line {idx} kind')
        fields={'frame':(0,2**32-1),'cycle':(0,2**64-1),
                'cpu_pc':(0,65535),'vm_source_pointer':(0,65535),
                'vm_source_prg_offset':(-1,2**32-1),'scratch_word':(0,65535),
                'scratch_614c':(0,255),'scratch_614e':(0,255)}
        for k,(lo,hi) in fields.items():
            v=e[k]
            if type(v) is not int or not lo<=v<=hi:raise TraceFormatError(f'line {idx} invalid {k}')
        pos=(e['frame'],e['cycle'])
        if pos<last:raise TraceFormatError(f'line {idx} time order')
        last=pos
        for k in ('unit0','unit1'):
            u=e[k]
            if not isinstance(u,dict) or not all(type(u.get(f)) is int and 0<=u[f]<=(65535 if f=='soldiers' else 255) for f in ('soldiers','x','y','type','mobility')):
                raise TraceFormatError(f'line {idx} unit state')
        a=e.get('addr')
        if e['kind']=='unit_record_write' and (type(a) is not int or not 0x6060<=a<=0x6075):raise TraceFormatError('unit address')
        if e['kind']=='word_613e_write' and (type(a) is not int or not 0x613e<=a<=0x613f):raise TraceFormatError('word address')
        if e['kind']=='scratch_write' and (type(a) is not int or not 0x614c<=a<=0x614e):raise TraceFormatError('scratch address')
        out.append(e)
    if not out:raise TraceFormatError('empty trace')
    def context(e):return {k:e[k] for k in ('frame','cycle','cpu_pc','vm_source_pointer','vm_source_prg_offset','scratch_word','scratch_614c','scratch_614e')}
    return {'events':len(out),'native_handler_events':[context(e) for e in out if e['kind'].startswith('native_handler')],
      'word_writer_events':[context(e) for e in out if e['kind']=='word_613e_write'],
      'unit_write_events':[dict(context(e),addr=e['addr']) for e in out if e['kind']=='unit_record_write'],
      'scratch_write_events':[dict(context(e),addr=e['addr']) for e in out if e['kind']=='scratch_write'],
      'caveat':'Candidate order only. Callback values may be pre-write, bank mapping requires verification and snapshots alone do not prove causality.'}

def main():
    p=argparse.ArgumentParser();p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path)
    a=p.parse_args();s=json.dumps(validate_trace(a.trace.read_text().splitlines()),indent=2)+'\n'
    if a.output:a.output.write_text(s)
    print(s,end='')
if __name__=='__main__':main()
