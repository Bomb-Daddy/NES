#!/usr/bin/env python3
"""Fail-closed audit of a Mesen W/C trace. DOES NOT authenticate trace provenance.

This separately validates the original-ROM C289 arithmetic, CPU chronology,
RAM before/after for *completed* external seed writes, and stage marker sanity.
Do not distribute source gameplay logs; run privately against stock output.
"""
import argparse
import hashlib
import pathlib

ROM_SHA='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
MULT=0xe0900e0187c5
MASK=(1<<48)-1

class TraceError(ValueError): pass

def need(ok, message):
    if not ok: raise TraceError(message)

def verify(lines, *, rom_path=None):
    if rom_path:
        raw=pathlib.Path(rom_path).read_bytes()
        need(len(raw)==262160 and hashlib.sha256(raw).hexdigest()==ROM_SHA,
             'original-stock ROM hash/length mismatch')
    lines=iter(lines)
    try: header=next(lines).rstrip('\n').split(',')
    except StopIteration: raise TraceError('empty trace')
    need(header==['GK1_291_STOCK_TRACE_V1',ROM_SHA,'FRESH_RESET_WORKRAM'],
         'header, original stock identity or memory bank mismatch')
    seed=[None]*6
    last_cycle=0
    calls=writes=0
    last_exit=0
    stages=[]
    for line in lines:
        if not line.strip(): continue
        parts=line.strip().split(',')
        kind=parts[0]
        try:
            if kind=='W':
                need(len(parts)==10,'write field count')
                _,cy,pc,bank,addr,before,value,actual,confirmed,frame=parts
                cy,pc,bank,addr,before,value,actual,confirmed,frame=map(int,
                    (cy,pc,bank,addr,before,value,actual,confirmed,frame))
                need(cy>last_cycle and cy>last_exit,'nonmonotonic source write')
                need(0x8000<=pc<=0xffff and 0<=bank<=15,'unknown source mapping')
                need(0x84<=addr<=0x89,'not RNG seed address')
                need(0<=value<=255 and 0<=before<=255 and actual==value and confirmed==1,
                     'incomplete or fabricated write completion')
                index=addr-0x84
                if seed[index] is not None:
                    need(seed[index]==before,'unreported seed mutation before write')
                seed[index]=value
                writes+=1;last_cycle=cy
            elif kind=='C':
                need(len(parts)==8,'draw field count')
                _,cy,ex,bank,before,after,draw,frame=parts
                cy,ex,bank,draw,frame=map(int,(cy,ex,bank,draw,frame))
                need(cy>last_cycle and cy>last_exit and ex>=cy,'nonmonotonic draw')
                need(bank==15 and all(x is not None for x in seed),'unproven seed or bank')
                need(len(before)==len(after)==12,'invalid seed hex')
                b=bytes.fromhex(before);a=bytes.fromhex(after)
                need(b==bytes(seed),'unreported source write before C289')
                n=int.from_bytes(b,'big')
                predicted=(-((n|1)*MULT))&MASK
                need(draw==((predicted>>32)&0x7fff),'C289 return arithmetic mismatch')
                need(a==predicted.to_bytes(6,'big'),'C289 RAM seed mismatch')
                seed=list(a);last_cycle=cy;last_exit=ex;calls+=1
            elif kind=='S':
                need(len(parts)==8,'stage field count')
                _,step,frame,year,season,orders,work,count=parts
                step,frame,year,season,orders,work,count=map(int,
                    (step,frame,year,season,orders,work,count))
                need(step==len(stages)+1 and 1<=step<=14,'stage order')
                need(count==calls and work in (0,1),'stage counter or bank')
                stages.append((year,season,orders,work))
            else: raise TraceError(f'unknown record {kind}')
        except (ValueError,OverflowError) as exc:
            if isinstance(exc,TraceError):raise
            raise TraceError(f'bad {kind} record: {exc}') from exc
        need(writes<=100000 and calls<=8192,'bounded trace exceeded')
    need(calls>0 and writes>=6,'no executed C289 or six source writes')
    need(len(stages)==14,'incomplete controller-stage capture')
    # These are observational probes ONLY; an outcome cannot be inferred
    # or claimed universal across RNG/controller histories from this fixture.
    ready=(stages[-1][0]==1175 and stages[-1][2]==3 and stages[-1][3]==1)
    return {'writes':writes,'calls':calls,'stages':len(stages),
            'observed_first_spring_menu':ready,
            'source_math_parity':True,
            'mesen_origin_independently_authenticated':False,
            'stock_spring_generator_implemented':False}

def main():
    p=argparse.ArgumentParser()
    p.add_argument('private_trace',type=pathlib.Path)
    p.add_argument('--rom',type=pathlib.Path)
    args=p.parse_args()
    result=verify(args.private_trace.read_text().splitlines(),rom_path=args.rom)
    print(result)

if __name__=='__main__': main()