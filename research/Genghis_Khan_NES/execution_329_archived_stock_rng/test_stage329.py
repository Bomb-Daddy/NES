#!/usr/bin/env python3
"""Independent staging refusal and rollback checks, no private inputs."""
from pathlib import Path
import shutil
import tempfile
from stage329 import stage, HERE, PUBLIC_FILES, REQUIRED, MARKER


def setup(root):
    root.mkdir()
    for name in REQUIRED:
        (root/name).write_bytes(b'add_library(gk1_app_state STATIC x.c)\n' if name=='CMakeLists.txt' else b'int dummy;\n')


def check():
    tests=0
    with tempfile.TemporaryDirectory() as d:
        base=Path(d)
        root=base/'good';setup(root)
        assert stage(root) is True; tests+=1
        assert stage(root) is False; tests+=1
        assert MARKER in (root/'CMakeLists.txt').read_text(); tests+=1
        assert all((root/n).read_bytes()==(HERE/n).read_bytes() for n in PUBLIC_FILES);tests+=1
        bad=base/'bad';setup(bad)
        (bad/'gk329_archived_seed_replay.c').write_text('tampered\n')
        original=(bad/'CMakeLists.txt').read_bytes()
        try:stage(bad);raise AssertionError('tamper not rejected')
        except ValueError: tests+=1
        assert (bad/'CMakeLists.txt').read_bytes()==original;tests+=1
        assert not (bad/'gk329_app_bridge.c').exists();tests+=1
        missing=base/'missing';setup(missing)
        (missing/'gk327_fixed6502_rng.c').unlink()
        try:stage(missing);raise AssertionError('incomplete inherited app accepted')
        except ValueError: tests+=1
        assert MARKER not in (missing/'CMakeLists.txt').read_text();tests+=1
        conflict=base/'partial';setup(conflict)
        (conflict/'CMakeLists.txt').write_text('add_library(gk1_app_state STATIC x.c)\n'+MARKER+'\n')
        try:stage(conflict);raise AssertionError('partial integration accepted')
        except ValueError: tests+=1
        assert not (conflict/'gk329_app_bridge.c').exists();tests+=1
        other=base/'notgame';setup(other)
        (other/'CMakeLists.txt').write_text('project(unrelated)\n')
        try:stage(other);raise AssertionError('non-game source accepted')
        except ValueError:tests+=1
        assert not (other/'gk329_app_bridge.c').exists();tests+=1
    print(f'GK329 staged source and hostile rollback {tests}/{tests} PASS')

if __name__=='__main__':check()
