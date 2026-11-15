#!/usr/bin/env python3
"""EXEC0296: authenticate a *private* Mesen V3 trace against original NES ROM.

The callback reports a CPU PC AFTER an absolute STA ($84..$87), therefore
PC-3 is attested as the real opcode address. The first C289 call has four
witnessed seed-byte stores; $88/$89 have UNKNOWN origin, not six writes.
No ROM, gameplay trace, screenshots or retained user state are published.
"""
import argparse
import hashlib
import json
from pathlib import Path

SHA256='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
SHA1='98b39b610933d0d82a6ccbcb962a28ba7dc71a85'
HEADER_V3=f'GK1_293_STOCK_TRACE_V3,{SHA256},{SHA1},FRESH_RESET_WORKRAM'
HEADER_V4=f'GK1_296_STOCK_TRACE_V4,{SHA256},{SHA1},FRESH_RESET_WORKRAM'
MASK48=(1<<48)-1
MULT=0xe0900e0187c5
class TraceError(ValueError): pass

def require(ok,msg):
    if not ok: raise TraceError(msg)

def integer(raw, label, minimum=0,maximum=(1<<63)-1):
    require(raw.isdecimal() and str(int(raw))==raw,f'{label}: not canonical integer')
    result=int(raw)
    require(minimum<=result<=maximum,f'{label}: out of range')
    return result

def seed_bytes(s):
    require(len(s)==12 and all(c in '0123456789abcdef' for c in s),'invalid six-byte seed')
    return bytes.fromhex(s)

def audit(rom,lines):
    rom=Path(rom).read_bytes() if not isinstance(rom,bytes) else rom
    require(len(rom)==262160 and hashlib.sha256(rom).hexdigest()==SHA256 and
      hashlib.sha1(rom).hexdigest()==SHA1,'original stock ROM identity mismatch')
    require(rom[:4]==b'NES\x1a','not an iNES ROM')
    lines=list(lines)
    require(0<len(lines)<=110001,'unbounded/empty trace')
    require(lines[0].strip() in (HEADER_V3,HEADER_V4),'wrong trace format or claimed stock identity')
    version=4 if lines[0].strip()==HEADER_V4 else 3
    initial_script_seed=None
    if version==4:
        require(len(lines)>2 and lines[1].startswith('I,0,'),'V4 missing script-start RAM observation')
        i=lines[1].strip().split(',')
        require(len(i)==4 and i[3]=='SCRIPT_START_RAM_OBSERVATION_NOT_POWERON_PROOF',
                'invalid initial RAM-observation label')
        initial_script_seed=seed_bytes(i[2])
        lines=lines[:1]+lines[2:]
    observed={};writer_mask=0;writes=0;calls=0;stages=0
    last_cycle=0;last_exit=0;last_frame=0
    first_source_mask=None;unproven_tail=None
    prev_c289=None
    for index,line in enumerate(lines[1:],2):
        a=line.strip().split(',')
        require(a[0] in ('W','C','S'),f'line {index} unknown record')
        if a[0]=='W':
            require(len(a)==10,f'line {index} malformed W')
            _,cy,pc,bank,addr,before,value,actual,committed,frame=a
            cy=integer(cy,'W cycle',1);pc=integer(pc,'post-instruction PC',0x8003,0xffff)
            bank=integer(bank,'PRG bank',0,15);addr=integer(addr,'seed destination',0x84,0x89)
            before=integer(before,'prior value',0,255);value=integer(value,'written value',0,255)
            actual=integer(actual,'readback',0,255);committed=integer(committed,'committed',1,1)
            frame=integer(frame,'write frame',0,100000)
            require(cy>last_cycle and cy>last_exit,'W cycles overlap or reversed')
            require(frame>=last_frame,'W frame reversed')
            # Mesen callback sees PC of following instruction: all witnessed
            # original stores are absolute, three bytes, at (reported PC - 3).
            opcode_pc=pc-(3 if version==3 else 0)
            require((opcode_pc>=0xc000 and bank==15) or
                    (0x8000<=opcode_pc<0xc000 and bank<15),'mapped PRG bank mismatch')
            offset=16+bank*0x4000+(opcode_pc-(0xc000 if opcode_pc>=0xc000 else 0x8000))
            require(offset+2<len(rom),'instruction outside original PRG')
            require(rom[offset] in (0x8c,0x8d,0x8e) and
                    rom[offset+1]==addr and rom[offset+2]==0,
                    'callback-PC/6502 source store mismatch')
            if addr in observed: require(observed[addr]==before,'unreported seed overwrite')
            elif initial_script_seed is not None:
                require(initial_script_seed[addr-0x84]==before,
                        'source store prior byte disagrees with script-start RAM')
            # This records W.before for an unseen initial source, not proof
            # of how the first power-on value was initialized.
            observed[addr]=value;writer_mask|=1<<(addr-0x84)
            writes+=1;last_cycle=cy;last_frame=frame
        elif a[0]=='C':
            require(len(a)==8,f'line {index} malformed C')
            _,cy,ex,bank,before,after,draw,frame=a
            cy=integer(cy,'C289 entry',1);ex=integer(ex,'C289 exit',cy)
            bank=integer(bank,'fixed bank',15,15)
            draw=integer(draw,'draw15',0,32767);frame=integer(frame,'call frame',0,100000)
            require(cy>last_cycle and cy>last_exit,'C289 cycles overlap/reverse')
            require(frame>=last_frame,'C289 frame reversed')
            b=seed_bytes(before);x=seed_bytes(after)
            if prev_c289 is None:
                first_source_mask=writer_mask
                unproven_tail=0x3f & ~writer_mask
                if initial_script_seed is not None:
                    require(b[4:]==initial_script_seed[4:],
                            'unwritten tail changed after script-start observation')
                require(first_source_mask==0x0f,'real fresh-run four-writer mask not established')
            else:
                require(b==prev_c289,'unreported seed change between draws')
            for addr,val in observed.items():
                require(b[addr-0x84]==val,'source write value does not match C289 entry')
            n=int.from_bytes(b,'big')
            result=(-( (n|1)*MULT))&MASK48
            require(x==result.to_bytes(6,'big') and draw==((result>>32)&0x7fff),
                    'C289 source-math parity failure')
            prev_c289=x;observed={0x84+i:x[i] for i in range(6)}
            calls+=1;last_cycle=cy;last_exit=ex;last_frame=frame
        else:
            require(len(a)==10,f'line {index} malformed S')
            _,step,frame,year,season,orders,work,count,cpu,physical=a
            step=integer(step,'stage',1,14);frame=integer(frame,'stage frame',0,100000)
            year=integer(year,'year',0,65535);season=integer(season,'season',0,65535)
            orders=integer(orders,'orders',0,65535);work=integer(work,'bank observation',0,1)
            count=integer(count,'call count',0,8192)
            cpu=integer(cpu,'country CPU address',0x6dcd,0x6dcd)
            physical=integer(physical,'physical bank addr',0,0x0dcd)
            require(step==stages+1 and count==calls,'stage/order/call counter mismatch')
            require(frame>=last_frame,'stage frame reversed')
            # PREVIOUS recorder returns work=0/physical=0 under this Mesen API:
            # preserve as INCONCLUSIVE, never establish no country data.
            require((work==0 and physical==0) or (work==1 and physical==0x0dcd),
                    'inconsistent PRG-RAM mapping claim')
            stages=step;last_frame=frame
        require(calls<=8192 and writes<=100000,'capture limit exceeded')
    require(calls>0 and writes>=4 and stages==14,'incomplete original capture')
    return {
        'rom_sha256_authenticated':True,
        'witnessed_source_writes':writes,
        'witnessed_source_target_mask_at_first_draw':hex(first_source_mask),
        'not_source_written_before_first_draw_mask':hex(unproven_tail),
        'c289_source_arithmetic_calls_verified':calls,
        'original_controller_stage_markers':stages,
        'mesen_callback_pc_bias_bytes':3 if version==3 else 0,
        'script_start_RAM_84_89_observed':bool(version==4),
        'script_start_RAM_is_hardware_poweron_proof':False,
        'recorder_workram_bank_proof':'INCONCLUSIVE',
        'poweron_ram_origin_88_89':'UNKNOWN',
        'native_playable_spring_implemented':False,
    }

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('private_original_rom',type=Path)
    ap.add_argument('private_live_trace',type=Path)
    ap.add_argument('--receipt-output',type=Path)
    args=ap.parse_args()
    receipt=audit(args.private_original_rom,args.private_live_trace.read_text().splitlines())
    result=json.dumps(receipt,indent=2,sort_keys=True)
    if args.receipt_output: args.receipt_output.write_text(result+'\n')
    print(result)
if __name__=='__main__':main()
