"""Validated, atomic per-user PC controller/keyboard key bindings.

The always-available physical B/Menu and keyboard Escape fallback are reserved
for cancellation/recovery, so remapping cannot strand a new player in a menu.
"""
from __future__ import annotations
import json
import os
from pathlib import Path

ACTIONS=('up','down','left','right','a','b','start','select','help')
PAD_POSSIBLE=('up','down','left','right','a','b','x','y','start','select','lb','rb')
KEY_POSSIBLE=('Up','Down','Left','Right','Return','Escape','F10','Tab','F1','space','F2','F3','F4','F5','F6','F7','F8','F9','Home','End','Prior','Next','Insert','Delete','h','j','k','l','w','s','a','d')
DEFAULT_PAD=dict(zip(ACTIONS,('up','down','left','right','a','b','start','select','y')))
DEFAULT_KEYS=dict(zip(ACTIONS,('Up','Down','Left','Right','Return','Escape','F10','Tab','F1')))
RESERVED={'pad':{'b':'b','start':'start'},'keyboard':{'b':'Escape','start':'F10'}}
class BindingError(ValueError):pass

class InputBindings:
    def __init__(self, path=None):
        if path is None:
            import sys
            home=Path.home()
            if sys.platform=='win32':
                path=Path(os.environ.get('LOCALAPPDATA') or home/'AppData'/'Local')/'KOEI_Original_Third_Branch'/'controls.json'
            else:
                path=Path(os.environ.get('XDG_CONFIG_HOME') or home/'.config')/'koei-original-third-branch'/'controls.json'
        self.path=Path(path)
        self.pad=DEFAULT_PAD.copy();self.keyboard=DEFAULT_KEYS.copy()
    def binding(self,device,action):return self._get(device)[action]
    def _get(self,device):
        if device=='pad':return self.pad
        if device=='keyboard':return self.keyboard
        raise BindingError('Unknown input device')
    def translate(self,device,physical):
        return next((logical for logical,key in self._get(device).items() if key==physical),None)
    def remap(self,device,action,physical):
        mapping=self._get(device)
        allowed=PAD_POSSIBLE if device=='pad' else KEY_POSSIBLE
        if action not in ACTIONS or physical not in allowed:raise BindingError('Unknown action or button')
        if action in RESERVED[device] and physical != RESERVED[device][action]:
            raise BindingError('Cancel/Menu controls are reserved for safe recovery')
        if physical in RESERVED[device].values() and RESERVED[device].get(action)!=physical:
            raise BindingError('Cannot take a reserved Cancel/Menu control')
        old=mapping[action]
        displaced=next((a for a,v in mapping.items() if a!=action and v==physical),None)
        if displaced and displaced in RESERVED[device]:raise BindingError('Cannot displace reserved control')
        if displaced:mapping[displaced]=old
        mapping[action]=physical
        return displaced
    def reset(self,device):
        mapping=self._get(device);mapping.clear();mapping.update(DEFAULT_PAD if device=='pad' else DEFAULT_KEYS)
    def save(self):
        self.path.parent.mkdir(parents=True,exist_ok=True)
        encoded=json.dumps({'format':'KOEI-CONTROLS-1','pad':self.pad,'keyboard':self.keyboard},sort_keys=True).encode('utf8')
        temp=self.path.with_suffix('.tmp')
        with temp.open('wb') as stream:
            stream.write(encoded);stream.flush();os.fsync(stream.fileno())
        os.replace(temp,self.path)
    @classmethod
    def load(cls,path=None):
        result=cls(path)
        try:
            doc=json.loads(result.path.read_text(encoding='utf8'))
            if doc.get('format')!='KOEI-CONTROLS-1':raise BindingError('Unknown binding version')
            for device in ('pad','keyboard'):
                mapping=doc[device]
                if set(mapping)!=set(ACTIONS) or len(set(mapping.values()))!=len(ACTIONS):raise BindingError('Bad binding map')
                for action,physical in mapping.items():
                    if physical not in (PAD_POSSIBLE if device=='pad' else KEY_POSSIBLE):raise BindingError('Unknown key')
                if any(mapping[a]!=v for a,v in RESERVED[device].items()):raise BindingError('Reserved controls missing')
                result._get(device).update(mapping)
        except FileNotFoundError:pass
        except (ValueError,KeyError,TypeError,AttributeError,OSError):
            # Corrupted settings are nonfatal; defaults always allow controller-only recovery.
            result.pad=DEFAULT_PAD.copy();result.keyboard=DEFAULT_KEYS.copy()
        return result
