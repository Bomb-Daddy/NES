"""Validate authored sequencer against native Mesen driver and APU captures.

Only complete driver calls are compared. An unfinished final call is disclosed.
Every capture request/response log is independently retained by the runner.
"""
from __future__ import annotations
import argparse, csv, json, hashlib
from collections import Counter
from pathlib import Path
from sound_driver_model import SoundDriver

def key(r:dict, cycle_name:str='cycle')->tuple[int,int]:
    return int(r['frame']),int(r[cycle_name])

def verify(rom:bytes, run:Path)->dict:
    records=[json.loads(s) for s in (run/'driver.jsonl').read_text().splitlines()]
    with (run/'apu.csv').open(newline='') as f:
        apu=[{k:int(v) for k,v in r.items()} for r in csv.DictReader(f)]
    assert all(key(a,'cpu_cycle')<=key(b,'cpu_cycle') for a,b in zip(apu,apu[1:])), 'unordered APU log'
    pending={};compared=0;matched=0;ei=0;tokens=Counter();regs=Counter()
    external=Counter();other_deltas=[];prior=None;continuous=0;continuous_writes=0;d=None
    for r in records:
        if r['kind']=='enter':
            assert r['n'] not in pending,'duplicate entry'
            pending[r['n']]=r;continue
        assert r['kind']=='exit','unknown driver record'
        a=pending.pop(r['n'])
        baseline=SoundDriver.from_trace(rom,a)
        # Independent call parity; continuous replay carries the preceding result.
        if prior is None:
            d=SoundDriver.from_trace(rom,a)
        else:
            z=bytes.fromhex(a['zero']);lp=bytes.fromhex(a['loops'])
            delta=[(0xb2+i,old,new) for i,(old,new) in enumerate(zip(d.memory[0xb2:0x100],z)) if old!=new]
            loopchanged=d.memory[0x731:0x771]!=lp
            if not loopchanged and all(p in (0xb2,0xb3,0xb4,0xf5,0xf6) for p,_,_ in delta):
                # Sound request inputs are the *only* state supplied between calls.
                for p,old,new in delta:d.memory[p]=new;external[hex(p)]+=1
                continuous+=1
            else:
                other_deltas.append({'invocation':a['n'],'frame':a['frame'],'addresses':[hex(x[0]) for x in delta],'loops_changed':loopchanged})
                d=SoundDriver.from_trace(rom,a)
        actual=baseline.tick();carry=d.tick()
        while ei<len(apu) and key(apu[ei],'cpu_cycle')<key(a):ei+=1
        j=ei
        while j<len(apu) and key(apu[j],'cpu_cycle')<key(r):j+=1
        expected=[(e['address'],e['value']) for e in apu[ei:j]]
        assert actual==expected, f'APU mismatch call {r["n"]}, frame{r["frame"]}'
        assert baseline.zero_hex()==r['zero'],f'zero-page mismatch call{r["n"]}'
        assert baseline.loop_hex()==r['loops'],f'loop-stack mismatch call{r["n"]}'
        assert carry==actual and d.zero_hex()==baseline.zero_hex() and d.loop_hex()==baseline.loop_hex(),f'continuous mismatch call{r["n"]}'
        matched+=len(expected);tokens.update(f'{v:02X}' for _,_,v in baseline.tokens)
        regs.update(f'{a:04X}' for a,_ in expected)
        compared+=1;ei=j;prior=r
    if pending:
        assert len(pending)==1 and records[-1]['kind']=='enter','nonterminal incomplete driver call'
    return {'run':run.name,'complete_native_driver_calls':compared,'matched_register_writes':matched,
            'total_captured_register_writes':len(apu),'continuous_carry_transitions':continuous,
            'external_request_updates':dict(external),'non_request_state_reset_boundaries':other_deltas,
            'incomplete_terminal_calls':list(pending),'decoded_tokens':dict(sorted(tokens.items())),
            'matched_register_histogram':dict(sorted(regs.items())),
            'files':{p.name:{'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'bytes':p.stat().st_size} for p in [run/'driver.jsonl',run/'apu.csv']},
            'result':'PASS VERIFIED','evidence':'independent high-level sequencer vs OBSERVED_RUNTIME_MOD',
            'does_not_validate':'waveform synthesis, within-call CPU cycle generation, complete Windows program, gameplay event routing'}

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--rom',type=Path,required=True);p.add_argument('--run',type=Path,action='append',required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();result={'runs':[verify(a.rom.read_bytes(),r) for r in a.run]};a.out.write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2))
