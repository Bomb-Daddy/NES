#!/usr/bin/env python3
"""EXEC0293: fail-closed V3 stock trace audit for 6502 setup RNG chronology.

Only verifies *supplied* transcript consistency. Does not prove that Mesen ran,
that controller buttons were accepted, or that the port creates Spring. Logs
contain original-ROM gameplay state and must remain in PRIVATE research Drive.
"""
from __future__ import annotations
import argparse
import hashlib
import pathlib
import re

ROM_SHA256 = 'ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
ROM_SHA1 = '98b39b610933d0d82a6ccbcb962a28ba7dc71a85'
MASK = (1 << 48) - 1
MULT = 0xe0900e0187c5
ACTIONS = ('start','a','a','a','a','b','a','a','a','wait','wait','wait','a','wait')
STAGE_FRAMES = []
frame_limit = 240
for action in ACTIONS:
    frame_limit += 300 if action == 'wait' else 125
    STAGE_FRAMES.append(frame_limit)

class TraceError(ValueError):
    pass

def require(condition, message):
    if not condition:
        raise TraceError(message)

def number(text, label, minimum=0, maximum=2**63 - 1):
    require(re.fullmatch(r'(0|[1-9][0-9]*)', text) is not None, f'{label} not a canonical unsigned decimal')
    n = int(text)
    require(minimum <= n <= maximum, f'{label} out of range')
    return n

def six(hex_string):
    require(re.fullmatch('[0-9a-f]{12}', hex_string) is not None, 'six-byte RNG hex invalid')
    return bytes.fromhex(hex_string)

def verify(lines, *, rom_path=None):
    if rom_path is not None:
        data = pathlib.Path(rom_path).read_bytes()
        require(len(data) == 262160 and hashlib.sha256(data).hexdigest() == ROM_SHA256
                and hashlib.sha1(data).hexdigest() == ROM_SHA1, 'wrong original-stock ROM bytes')
    it = iter(lines)
    try:
        header = next(it).strip()
    except StopIteration:
        raise TraceError('empty trace')
    require(header == f'GK1_293_STOCK_TRACE_V3,{ROM_SHA256},{ROM_SHA1},FRESH_RESET_WORKRAM',
            'trace header/source identity mismatch')
    seed = [None]*6
    previous_cycle = -1
    previous_exit = -1
    previous_frame = -1
    writes = calls = 0
    stages = []
    step_end = 0
    for index, row in enumerate(it, 2):
        require(index <= 110000, 'trace capacity exceeded')
        parts = row.strip().split(',')
        kind = parts[0]
        require(kind in ('W','C','S'), f'line {index}: unknown record')
        if kind == 'W':
            require(len(parts) == 10, 'invalid W field count')
            _, cycle, pc, bank, addr, before, value, actual, confirmed, frame = parts
            cy = number(cycle, 'write cycle', 1)
            pc = number(pc, 'write pc', 0x8000, 0xffff)
            bank = number(bank, 'mapped bank', 0, 15)
            addr = number(addr, 'write addr', 0x84, 0x89)
            before = number(before, 'write pre-value', 0, 255)
            value = number(value, 'write value', 0, 255)
            actual = number(actual, 'write actual', 0, 255)
            confirmed = number(confirmed, 'write commit', 1, 1)
            frame = number(frame, 'write frame', 0, frame_limit)
            require(cy > previous_cycle and cy > previous_exit, 'nonmonotonic W CPU cycle')
            require(frame >= previous_frame, 'nonmonotonic W frame')
            require(actual == value, 'write did not commit')
            idx = addr - 0x84
            if seed[idx] is not None:
                require(before == seed[idx], 'unreported seed mutation before W')
            seed[idx] = value
            writes += 1
            previous_cycle = cy
            previous_frame = frame
        elif kind == 'C':
            require(len(parts) == 8, 'invalid C field count')
            _, cycle, exit_cycle, bank, before, after, draw, frame = parts
            cy = number(cycle, 'C289 entry', 1)
            ex = number(exit_cycle, 'C289 return', cy)
            bank = number(bank, 'C289 mapped bank', 15, 15)
            d = number(draw, 'C289 return value', 0, 32767)
            fr = number(frame, 'draw frame', 0, frame_limit)
            require(cy > previous_cycle and cy > previous_exit, 'overlapping/nonmonotonic C289 call')
            require(fr >= previous_frame, 'nonmonotonic draw frame')
            require(all(v is not None for v in seed), 'six source seed writes not established')
            b = six(before)
            a = six(after)
            require(b == bytes(seed), 'unreported seed change before C289')
            n = int.from_bytes(b, 'big')
            transformed = (-((n | 1) * MULT)) & MASK
            require(a == transformed.to_bytes(6, 'big'), 'C289 seed arithmetic mismatch')
            require(d == (transformed >> 32) & 0x7fff, 'C289 result mismatch')
            seed = list(a)
            calls += 1
            previous_cycle = cy
            previous_exit = ex
            previous_frame = fr
        else:
            require(len(parts) == 10, 'invalid S field count')
            _, step, frame, year, season, orders, work, count, cpu_addr, physical_addr = parts
            stage = number(step, 'stage ID', 1, 14)
            fr = number(frame, 'stage frame', 0, frame_limit)
            year = number(year, 'year', 0, 65535)
            season = number(season, 'season raw', 0, 65535)
            orders = number(orders, 'orders raw', 0, 65535)
            work = number(work, 'WorkRAM bank', 0, 1)
            count = number(count, 'call counter', 0, 8192)
            cpu_addr = number(cpu_addr, 'CPU country address', 0x6dcd, 0x6dcd)
            physical_addr = number(physical_addr, 'mapped physical address', 0, 0x0dcd)
            require((work == 1 and physical_addr == 0x0dcd) or
                    (work == 0 and physical_addr == 0),
                    'PRG RAM bank/physical address mismatch')
            require(stage == len(stages)+1, 'stage missing, duplicate or reordered')
            require(fr == STAGE_FRAMES[stage-1], 'wrong controller stage frame')
            require(fr >= previous_frame, 'stage frame precedes source events')
            require(count == calls, 'stage call counter mismatch')
            stages.append({'step':stage, 'frame':fr, 'year':year,
                           'season_raw':season, 'orders_raw':orders, 'work':work,
                           'cpu_addr':cpu_addr, 'physical_addr':physical_addr})
            previous_frame = fr
            step_end = stage
        require(writes <= 100000 and calls <= 8192, 'exceeded source capture limits')
    require(writes >= 6 and calls > 0, 'no witnessed six source writes plus C289')
    require(step_end == 14, 'missing controller stage records')
    last = stages[-1]
    candidate = (last['year'] == 1175 and last['orders_raw'] == 3 and last['work'] == 1)
    return {'writes':writes, 'calls':calls, 'stages':step_end,
            'final_stage':last, 'stock_first_spring_state_candidate':candidate,
            'source_math_parity':True,
            'prg_ram_cpu_physical_mapping_claimed_by_recorder':True,
            'loaded_rom_identity_claimed_by_recorder':True,
            'mesen_execution_independently_proven':False,
            'controller_command_menu_visual_confirmed':False,
            'native_stock_spring_implemented':False}

def main():
    p = argparse.ArgumentParser()
    p.add_argument('private_trace', type=pathlib.Path)
    p.add_argument('--rom', type=pathlib.Path)
    args = p.parse_args()
    print(verify(args.private_trace.read_text().splitlines(),rom_path=args.rom))

if __name__ == '__main__': main()
