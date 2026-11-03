"""Classic is the authentic NES game, never the retired Enhanced prototype.

Run the user's licensed NES ROM in a supported standalone emulator. Prefer an
existing standalone faithful Classic executable when configured. Never fabricate
Classic gameplay or silently fall back to the expanded Enhanced game.
"""
from __future__ import annotations
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

class ClassicUnavailable(RuntimeError):
    pass

def config_path(platform=None, environ=None, home=None):
    platform=sys.platform if platform is None else platform
    environ=os.environ if environ is None else environ
    home=Path.home() if home is None else Path(home)
    if platform=='win32':
        base=Path(environ.get('LOCALAPPDATA') or home/'AppData'/'Local')
    else:
        base=Path(environ.get('XDG_CONFIG_HOME') or home/'.config')
    return base/'KOEI_Classic_Enhanced'/'classic.json'

def settings(path=None):
    p=Path(path or config_path())
    if not p.is_file():return {}
    try:
        obj=json.loads(p.read_text(encoding='utf-8'))
        return obj if isinstance(obj,dict) else {}
    except (OSError,ValueError):return {}

def save_settings(values,path=None):
    p=Path(path or config_path())
    p.parent.mkdir(parents=True,exist_ok=True)
    clean={k:str(values[k]) for k in ('rom','emulator','classic_executable') if values.get(k)}
    tmp=p.with_suffix('.pending')
    tmp.write_text(json.dumps(clean,indent=2),encoding='utf-8')
    tmp.replace(p)
    return p

def valid_nes_rom(path):
    try:
        p=Path(path)
        with p.open('rb') as f:header=f.read(16)
        if len(header)!=16 or header[:4]!=b'NES\x1a':return False
        size=p.stat().st_size
        # Recognize standard iNES/NES2 headers and reject obviously truncated files.
        return size>=16+16384 and size<=64*1024*1024
    except (OSError,TypeError,ValueError):return False

def executable(path):
    if not path:return None
    p=Path(path).expanduser()
    if p.is_file():return str(p.resolve())
    return shutil.which(str(path))

def resolve_classic(env=None, cfg=None):
    env=os.environ if env is None else env
    cfg=settings() if cfg is None else cfg
    # If user already owns a standalone faithful Classic game build, use it.
    native=executable(env.get('KOEI_CLASSIC_EXE') or cfg.get('classic_executable'))
    if native:return [native]
    rom=env.get('KOEI_CLASSIC_ROM') or cfg.get('rom')
    if not rom or not valid_nes_rom(rom):
        raise ClassicUnavailable('Classic requires your own valid English NES game ROM. Choose it once to configure Classic.')
    provider=env.get('KOEI_CLASSIC_EMULATOR') or cfg.get('emulator')
    emulator=executable(provider)
    if not emulator:
        # Native installers vendor an upstream GPL-3 MesenCE executable. No ROM
        # is ever included; Classic uses the user's original licensed copy.
        roots=[Path(getattr(sys,'_MEIPASS',Path(__file__).resolve().parents[1])),
               Path(sys.executable).resolve().parent,Path(__file__).resolve().parents[1]]
        family='windows' if sys.platform=='win32' else 'linux'
        options=('Mesen.exe','Mesen2.exe') if family=='windows' else ('Mesen','Mesen2','mesen')
        for base in roots:
            folder=base/'third_party'/'Mesen'/family
            if not folder.is_dir():continue
            for name in options:
                for path in folder.rglob(name):
                    if path.is_file():
                        emulator=str(path.resolve());break
                if emulator:break
            if emulator:break
    if not emulator:
        candidates=['Mesen.exe','Mesen2.exe','Mesen','Mesen2','mesen','mesen2']
        emulator=next((v for v in map(executable,candidates) if v),None)
    if not emulator:
        raise ClassicUnavailable('Classic requires Mesen/Mesen2 installed or a configured faithful Classic executable. Choose the emulator once.')
    return [emulator,str(Path(rom).expanduser().resolve())]

def launch_classic(env=None,cfg=None,popen=subprocess.Popen):
    cmd=resolve_classic(env,cfg)
    try:return popen(cmd)
    except (OSError,ValueError) as exc:raise ClassicUnavailable(f'Could not start Classic: {exc}') from exc
