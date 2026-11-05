"""Validate EXEC0018 snapshot bytes against their complete native read responses.

Usage: python verify_capture_bundle.py /private/unpacked/evidence
This checks saved evidence only; it does not rerun Mesen or certify Windows.
"""
from __future__ import annotations
import hashlib,json,sys
from pathlib import Path


def verify_session(folder: Path) -> dict:
    pending = None
    label = None
    bindings: dict[str, dict[int, bytes]] = {}
    pairs = 0
    interventions = []
    for ordinal, line in enumerate((folder / 'events.jsonl').read_text().splitlines(), 1):
        r = json.loads(line)
        if r['kind'] == 'request':
            if pending is not None:
                raise ValueError(f'{folder.name}:{ordinal}: request without preceding response')
            pending = r['value']
            if pending['tool'] == 'video.export_frame':
                label = Path(pending['path']).stem
                bindings[label] = {}
        elif r['kind'] == 'response':
            if pending is None:
                raise ValueError(f'{folder.name}:{ordinal}: response without request')
            value = r['value']
            if pending['tool'] == 'cpu.read_memory' and label is not None:
                a, n = pending['address'], pending['length']
                raw = bytes(value['bytes'])
                if len(raw) != n or value['address'] != a:
                    raise ValueError('native memory response address/size mismatch')
                if (a,n) in ((0x6000,8192),(0,256)):
                    bindings[label][a] = raw
            elif pending['tool'] == 'cpu.write_memory':
                interventions.append({'address':pending['address'],'bytes':pending['bytes']})
            pairs += 1
            pending = None
    if pending is not None:
        raise ValueError('terminal request has no response')
    snapshots = 0
    for meta in folder.glob('*.json'):
        obj = json.loads(meta.read_text())
        if not isinstance(obj,dict) or 'sram_sha256' not in obj or 'label' not in obj:
            continue
        name = obj['label']
        if name not in bindings or set(bindings[name]) != {0,0x6000}:
            raise ValueError(f'{name}: missing native memory bindings')
        ram = meta.with_suffix('.sram').read_bytes()
        zp = meta.with_suffix('.zeropage').read_bytes()
        if ram != bindings[name][0x6000] or zp != bindings[name][0]:
            raise ValueError(f'{name}: capture bytes differ from raw native responses')
        if hashlib.sha256(ram).hexdigest() != obj['sram_sha256']:
            raise ValueError(f'{name}: metadata hash mismatch')
        if zp[0x84:0x8a].hex() != obj['rng_seed_hex']:
            raise ValueError(f'{name}: RNG metadata mismatch')
        snapshots += 1
    if not snapshots:
        raise ValueError('no bound snapshots; empty evidence cannot pass')
    return {'session':folder.name,'request_response_pairs':pairs,
            'snapshots':snapshots,'interventions':interventions}


def main() -> None:
    root = Path(sys.argv[1])
    print(json.dumps({'sessions':[verify_session(root/x) for x in ('run','combat')]},indent=2))

if __name__ == '__main__':
    main()
