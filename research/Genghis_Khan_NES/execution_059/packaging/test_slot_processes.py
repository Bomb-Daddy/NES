#!/usr/bin/env python3
"""EXEC0058: black-box native-app processes, no emulator/Xbox claim."""
import os
import pathlib
import re
import subprocess
import sys
import tempfile

def run(exe, *args, expected=0, env=None):
    p = subprocess.run([exe, *map(str,args)], text=True, stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=15, env=env)
    assert p.returncode == expected, (args, p.returncode, expected,p.stdout,p.stderr)
    return p.stdout

def main():
    exe = os.path.abspath(sys.argv[1])
    with tempfile.TemporaryDirectory(prefix="gk58-slot-") as td:
        base = pathlib.Path(td)/"gk1_presetup_slot1.gkp"
        initial1=run(exe,'--slot-export',base,1,2,1,4)
        initial2=run(exe,'--slot-export',base,2,2,1,7)
        initial3=run(exe,'--slot-export',base,3,1,1,2)
        assert 'troops=111 town=9' in initial1,initial1
        assert 'troops=108 town=12' in initial2,initial2
        assert 'slot=3 scenario=1' in initial3,initial3
        paths=[base, pathlib.Path(str(base)+'.slot2'), pathlib.Path(str(base)+'.slot3')]
        assert all(x.is_file() and x.stat().st_size==2734 for x in paths)
        if os.name != 'nt':
            assert all(x.stat().st_mode & 0o777 == 0o600 for x in paths)
        bytes_before=[x.read_bytes() for x in paths]
        assert 'troops=111 town=9' in run(exe,'--slot-import',base,1,2,1)
        assert 'troops=108 town=12' in run(exe,'--slot-import',base,2,2,1)
        assert 'slot=3 scenario=1' in run(exe,'--slot-import',base,3,1,1)
        # No overwrite, even with different action/data requested.
        run(exe,'--slot-export',base,2,2,1,9,expected=6)
        assert [x.read_bytes() for x in paths]==bytes_before
        # Only slot3 intentionally damaged. Slot1/2 remain importable.
        with paths[2].open('r+b') as f:
            f.seek(247);v=f.read(1);assert v
            f.seek(247);f.write(bytes([v[0]^1]))
        run(exe,'--slot-import',base,3,1,1,expected=7)
        assert [x.read_bytes() for x in paths[:2]]==bytes_before[:2]
        assert 'troops=111 town=9' in run(exe,'--slot-import',base,1,2,1)
        assert 'troops=108 town=12' in run(exe,'--slot-import',base,2,2,1)
        # New filename collision and unsupported slot rejected before any mutation.
        for bad in [0,4,999]:
            run(exe,'--slot-export',base,bad,1,1,2,expected=4)
            run(exe,'--slot-import',base,bad,1,1,expected=4)
        assert [x.read_bytes() for x in paths[:2]]==bytes_before[:2]
        # Wrong original CSV identity blocks creation in otherwise empty directory.
        bad_env=os.environ.copy()
        bad_env['GK1_SCENARIO_CSV']=str(pathlib.Path(td)/'bad.csv')
        run(exe,'--slot-export',str(pathlib.Path(td)/'invalid'),1,2,1,4,expected=8,env=bad_env)
        assert not (pathlib.Path(td)/'invalid').exists()
        # Import of absent slot must fail, no partial save created.
        absent=pathlib.Path(td)/'absent'
        run(exe,'--slot-import',absent,1,2,1,expected=7)
        assert not absent.exists()
        print("EXEC0058 TESTED_PORT_LINUX 3 slots: independent country edits, 3 fresh-process cold-loads, 0600, exclusive-create, selective corruption rollback, wrong source/slot, backward slot1 filename PASS")
        print("EXEC0058 tested paths:",len(paths))
if __name__=='__main__': main()
