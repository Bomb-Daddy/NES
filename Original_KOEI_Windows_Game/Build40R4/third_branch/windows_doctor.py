"""Cross-platform runtime preflight, including Windows packaged source installs."""
from __future__ import annotations
import argparse
import importlib
import json
import os
import sys
from pathlib import Path


def inspect_runtime(strict=False):
    result={'platform':sys.platform,'python':sys.version.split()[0], 'checks':{},'errors':[],'warnings':[]}
    def check(name,fn,critical=True):
        try:
            val=fn()
            result['checks'][name]=val
        except Exception as exc:
            result['checks'][name]=None
            (result['errors'] if critical else result['warnings']).append(f'{name}: {type(exc).__name__}: {exc}')
    check('python_supported',lambda: True if sys.version_info >= (3,11) else (_ for _ in ()).throw(RuntimeError('Python >=3.11 required')))
    check('tkinter',lambda:bool(importlib.import_module('tkinter').Tk))
    check('pillow',lambda:bool(importlib.import_module('PIL.Image').open))
    from .audio_system import verify_private_assets
    def audio_assets():
        report=verify_private_assets()
        sfx_errors=[e for e in report['errors'] if not e.startswith('missing MP3:')]
        if sfx_errors:raise RuntimeError('; '.join(sfx_errors))
        missing_music=[e for e in report['errors'] if e.startswith('missing MP3:')]
        if missing_music:
            result['warnings'].append('Optional private music pack not installed: '+str(len(missing_music))+' tracks absent')
        return {'mp3_available':report['mp3_count']-len(missing_music),'mp3_expected':report['mp3_count'],'sfx':report['sfx_count']}
    check('audio_assets',audio_assets)
    from .platform_paths import default_save_path,prepare_save_path
    def writable_save():
        path=prepare_save_path(default_save_path())
        # Check directory only; never overwrite or change an existing save.
        import tempfile
        with tempfile.TemporaryFile(dir=path.parent):pass
        return str(path)
    check('writable_saves',writable_save)
    def audio_backend():
        from .audio_system import GameAudio
        sound=GameAudio(music_mode='off')
        report={'backend':sound.backend,'status':sound.status()}
        sound.close()
        if sound.backend in (None,'winsound-sfx-only','disabled-by-test'):
            raise RuntimeError('No complete music/SFX playback backend: '+str(report))
        return report
    check('audio_backend',audio_backend,critical=False)
    if sys.platform=='win32':
        from .xbox_controller import open_native_controller
        def controller():
            provider=open_native_controller()
            if provider is None:raise RuntimeError('XInput could not initialize')
            provider.close()
            return 'XInput interface available (physical pad still unverified)'
        check('xinput_interface',controller,critical=False)
    result['ok']=not result['errors'] and (not strict or not result['warnings'])
    return result


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--strict',action='store_true')
    args=ap.parse_args()
    report=inspect_runtime(strict=args.strict)
    print(json.dumps(report,sort_keys=True,indent=2))
    return 0 if report['ok'] else 1

if __name__=='__main__':raise SystemExit(main())
