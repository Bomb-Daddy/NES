"""Private arranged soundtrack + ORIGINAL small PCM sound effects for the third branch.

The 1989 recordings belong to their respective rightsholders. User-provided
assets are for private testing only; this software does not license resale or
public distribution. These recordings and original SFX are NOT NES-audio parity.

Audio does not influence the saved deterministic campaign or action validity.
Pygame-ce is the preferred cross-platform backend. FFplay is a Linux fallback;
Windows winsound supports short PCM effects but not MP3 music. Missing backend
must be reported rather than falsely claiming successful playback.
"""
from __future__ import annotations
import os, shutil, subprocess, json, hashlib, time
from pathlib import Path

BASE=Path(__file__).resolve().parent/'assets'/'audio'
ALBUM=BASE/'soundware_1989'
EFFECTS=BASE/'sfx_original'
TRACKS={int(p.name[:2]):p for p in ALBUM.glob('??. *.mp3')}
SFX={p.stem:p for p in EFFECTS.glob('*.wav')}
REGION_TRACK={'north':7,'central':13,'south':9,'west':3,'frontier':4}
EVENT_TRACK={'opening':1,'prologue':2,'diplomacy':6,'marriage':10,'battle':14,
             'defeat':5,'death':8,'demise':12,'unification':15,'diaochan':11}


def region_of_province(n):
    """Optional arranged mapping for the original branch, not an NES original rule."""
    n=int(n)
    if n in (14,15) or 37<=n<=41:return 'frontier'
    if 1<=n<=7:return 'north'
    if 8<=n<=13 or 16<=n<=18:return 'central'
    if 19<=n<=28:return 'south'
    if 29<=n<=36:return 'west'
    raise ValueError('Province must be 1..41')


def track_for(event, province=None):
    if event=='map':return REGION_TRACK[region_of_province(province)]
    if event.startswith('track:'):
        try:n=int(event.split(':',1)[1]);return n if 1<=n<=15 else None
        except ValueError:return None
    return EVENT_TRACK.get(event)


def verify_private_assets():
    """Check supplied original MP3 bytes and generated SFX. No silent substitutions."""
    album=json.loads((BASE/'soundware_manifest.json').read_text())
    effects=json.loads((BASE/'sfx_manifest.json').read_text())
    errors=[]
    for item in album['tracks']:
        path=ALBUM/item['filename']
        if not path.is_file():errors.append('missing MP3: '+item['filename']);continue
        if path.stat().st_size!=item['bytes'] or hashlib.sha256(path.read_bytes()).hexdigest()!=item['sha256']:
            errors.append('MP3 checksum mismatch: '+item['filename'])
    for item in effects['assets']:
        path=EFFECTS/item['name']
        if not path.is_file():errors.append('missing SFX: '+item['name']);continue
        if path.stat().st_size!=item['bytes'] or hashlib.sha256(path.read_bytes()).hexdigest()!=item['sha256']:
            errors.append('SFX checksum mismatch: '+item['name'])
    return {'mp3_count':len(album['tracks']),'sfx_count':len(effects['assets']),
            'errors':errors,'ok':not errors}


class GameAudio:
    """Best-effort nonblocking music and sound effects; fails visibly, never blocks game.

    An actual backend selection is independent of the existence of asset files.
    'mock' backend is test-only and MUST NOT be called audible production.
    """
    def __init__(self,music_mode='arranged',sfx_enabled=True,backend=None):
        self.mode=music_mode if music_mode in ('arranged','off') else 'off'
        self.sfx_enabled=bool(sfx_enabled)
        self.backend=backend
        self.pygame=None
        self.winsound=None
        self.winmm=None
        self.process=None
        self.effect_processes=[]
        self.current=None
        self.last_event=None
        self.log=[]
        self.error=''
        self.audio_log_path=None
        self.last_audio_failure=0.0
        if backend is None:
            if os.environ.get('KOEI_DISABLE_AUDIO')=='1':
                self.backend='disabled-by-test'
            else:
                try:
                    import pygame
                    pygame.mixer.init(frequency=22050,buffer=1024)
                    self.pygame=pygame
                    self.backend='pygame'
                except Exception:
                    if os.name=='nt':
                        try:
                            import winsound
                            self.winsound=winsound
                            try:
                                import ctypes
                                self.winmm=ctypes.windll.winmm
                                self.backend='winmm-mci'
                            except Exception:
                                self.backend='winsound-sfx-only'
                        except ImportError:pass
                    if self.backend is None and shutil.which('ffplay'):
                        self.backend='ffplay'
        if self.backend is None:self.error='No sound backend. Install pygame-ce for Windows music and effects.'
        if self.backend=='winsound-sfx-only':self.error='Windows PCM effects work; MP3 music requires pygame-ce or Windows MCI.'

    @property
    def ready(self):return len(TRACKS)==15 and all(p.is_file() for p in TRACKS.values())

    def _poll_audio(self):
        """Observe FFplay errors rather than trusting successful Popen()."""
        if self.backend!='ffplay' or self.process is None:
            return
        rc=self.process.poll()
        if rc is None:return
        detail=''
        if self.audio_log_path is not None:
            try:detail=' '.join(self.audio_log_path.read_text(errors='replace')[-320:].split())
            except OSError:pass
        self.process=None
        self.current=None
        if rc:
            self.error=f'FFplay exited {rc}'+(': '+detail if detail else '')
            self.last_audio_failure=time.monotonic()
            self.log.append({'type':'audio_failure','error':self.error})

    def status(self):
        if self.mode=='off' and not self.sfx_enabled:return 'Audio muted'
        if not self.ready:return 'Private soundtrack files missing; music unavailable'+(' | '+self.error if self.error else '')
        if self.backend in (None,'winsound-sfx-only'):
            return 'Music unavailable: install pygame-ce (SFX '+('enabled' if self.backend else 'unavailable')+')'
        if self.backend=='disabled-by-test':return 'Audio disabled by KOEI_DISABLE_AUDIO for testing'
        if self.backend=='mock':return 'Mock backend — NOT audible'
        self._poll_audio()
        if self.error:return 'Music unavailable: '+self.error[:125]
        if self.mode=='off':return 'Music disabled | Effects '+('On' if self.sfx_enabled else 'Off')
        if self.backend=='ffplay' and self.process is not None:
            return 'Music playback requested (ffplay); speakers not verified'
        if self.backend=='pygame':
            if self.pygame is not None and self.pygame.mixer.music.get_busy():
                return 'Music playback requested (pygame); speakers not verified'
            return 'Music not playing (pygame)'
        return 'Music not playing'

    def stop_music(self):
        if self.backend=='winmm-mci' and self.winmm is not None:
            self._mci('stop koei_bgm');self._mci('close koei_bgm')
        if self.pygame:
            try:self.pygame.mixer.music.stop()
            except Exception:pass
        if self.process is not None:
            try:
                self.process.terminate();self.process.wait(timeout=.7)
            except Exception:
                try:self.process.kill();self.process.wait(timeout=.7)
                except Exception:pass
            self.process=None
        self.current=None

    def _mci(self,command):
        """WinMM MCI for native Windows MP3 playback, no bundled codec required."""
        try:
            import ctypes
            buf=ctypes.create_unicode_buffer(512)
            code=self.winmm.mciSendStringW(command,buf,512,0)
            if code:
                error=ctypes.create_unicode_buffer(256)
                self.winmm.mciGetErrorStringW(code,error,256)
                self.error='Windows MCI: '+(error.value or str(code))
                return False
            return True
        except Exception as exc:
            self.error='Windows MCI unavailable: '+str(exc)
            return False

    def play(self,event,province=None,loop=None):
        n=track_for(event,province)
        self.last_event=(event,province)
        self._poll_audio()
        if self.error and self.last_audio_failure and time.monotonic()-self.last_audio_failure<30:
            return False  # Do not spawn-fail loop on every redraw after device failure.
        if self.mode=='off':self.stop_music();return False
        if n is None:return False
        path=TRACKS.get(n)
        if path is None or not path.is_file():
            self.error=f'Missing private soundtrack track {n:02d}';self.log.append({'event':event,'error':self.error});return False
        looping=(event in ('map','battle','opening','prologue') or event.startswith('track:')) if loop is None else bool(loop)
        if self.current==(n,looping):
            if self.pygame and self.pygame.mixer.music.get_busy():return True
            if self.process is not None and self.process.poll() is None:return True
            if self.backend=='mock':return True
        self.stop_music()
        if self.backend=='pygame':
            try:
                self.pygame.mixer.music.load(str(path))
                self.pygame.mixer.music.set_volume(.48)
                self.pygame.mixer.music.play(-1 if looping else 0,fade_ms=250)
            except Exception as exc:self.error=f'Pygame MP3 failed: {exc}';return False
        elif self.backend=='ffplay':
            cmd=['ffplay','-nostats','-loglevel','error','-nodisp','-autoexit','-volume','48']
            if looping:cmd+=['-loop','0']
            cmd+=['-i',str(path)]
            try:
                cache=Path(os.environ.get('XDG_CACHE_HOME',str(Path.home()/'.cache')))/'koei-three-games'
                cache.mkdir(parents=True,exist_ok=True)
                self.audio_log_path=cache/'original-game-audio.log'
                with self.audio_log_path.open('w',encoding='utf-8') as logstream:
                    self.process=subprocess.Popen(cmd,stdin=subprocess.DEVNULL,
                               stdout=subprocess.DEVNULL,stderr=logstream)
            except OSError as exc:
                self.error=f'FFplay failed: {exc}'
                self.last_audio_failure=time.monotonic()
                return False
        elif self.backend=='winmm-mci':
            if not self._mci('open '+chr(34)+str(path)+chr(34)+' type mpegvideo alias koei_bgm'):
                return False
            self._mci('setaudio koei_bgm volume to 480')
            if not self._mci('play koei_bgm'+(' repeat' if looping else '')):
                self._mci('close koei_bgm');return False
        elif self.backend=='mock':pass
        else:
            self.error='MP3 music unavailable: install pygame-ce';return False
        self.current=(n,looping)
        self.error=''
        self.log.append({'type':'music','event':event,'province':province,'track':n,'loop':looping})
        return True

    def effect(self,name):
        if not self.sfx_enabled:return False
        path=SFX.get(name)
        if path is None or not path.is_file():
            self.error='Missing SFX: '+name;return False
        if self.backend=='pygame':
            try:self.pygame.mixer.Sound(str(path)).play()
            except Exception as exc:self.error=f'Pygame SFX failed: {exc}';return False
        elif self.backend=='ffplay':
            # Bound concurrent child-process count; never block the GUI to play SFX.
            self.effect_processes=[p for p in self.effect_processes if p.poll() is None]
            if len(self.effect_processes)>2:return False
            try:
                child=subprocess.Popen(['ffplay','-nostats','-loglevel','error','-nodisp','-autoexit','-volume','48','-i',str(path)],
                                       stdin=subprocess.DEVNULL,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
                self.effect_processes.append(child)
            except OSError as exc:self.error=f'FFplay SFX failed: {exc}';return False
        elif self.backend in ('winmm-mci','winsound-sfx-only'):
            try:self.winsound.PlaySound(str(path),self.winsound.SND_FILENAME|self.winsound.SND_ASYNC|self.winsound.SND_NODEFAULT)
            except RuntimeError as exc:self.error=f'Winsound SFX failed: {exc}';return False
        elif self.backend=='mock':pass
        else:self.error='No SFX backend: install pygame-ce';return False
        self.log.append({'type':'sfx','name':name})
        return True

    def mode_set(self,mode,province=None):
        if mode not in ('arranged','off'):raise ValueError(mode)
        self.mode=mode;self.stop_music()
        if mode=='arranged' and province is not None:return self.play('map',province)
        return mode=='off'

    def effects_set(self,enabled):
        self.sfx_enabled=bool(enabled)
        return self.sfx_enabled

    def close(self):
        self.stop_music()
        for p in self.effect_processes:
            if p.poll() is None:
                try:p.terminate();p.wait(timeout=.5)
                except Exception:
                    try:p.kill();p.wait(timeout=.5)
                    except Exception:pass
        self.effect_processes.clear()
        if self.pygame:
            try:self.pygame.mixer.quit()
            except Exception:pass

# Maintain old reference module interface for extensions while adding effects.
Soundtrack=GameAudio
