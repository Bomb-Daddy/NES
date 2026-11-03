"""Controller-navigable, TWO-CHOICE game launcher. No third mode.

Classic launches an authentic NES emulator or explicitly configured Classic
executable; Enhanced launches the expanded 41-province game in this source tree.
"""
from __future__ import annotations
import argparse
import os
import subprocess
import sys
from pathlib import Path
from .classic import ClassicUnavailable, launch_classic, save_settings, settings, valid_nes_rom

MODES=('Classic Game','Enhanced Game')

class Launcher:
    def __init__(self,tk=None,controller=None):
        import tkinter as tkmod
        self.tk=tk or tkmod
        self.root=self.tk.Tk()
        self.root.title('KOEI — Classic / Enhanced')
        self.root.geometry('960x720')
        self.root.resizable(False,False)
        self.root.configure(bg='#17273a')
        self.index=0
        self.controller=controller
        self.picker=None
        self.status=self.tk.StringVar(value='D-pad: select    A: play    B: close    Select: configure Classic')
        self.tk.Label(self.root,text='KOEI',font=('Courier New',34,'bold'),bg='#17273a',fg='#f5d998').pack(pady=(110,18))
        self.tk.Label(self.root,text='CHOOSE YOUR GAME',font=('Courier New',15),bg='#17273a',fg='#e7e7dd').pack(pady=(0,56))
        self.buttons=[]
        for text in MODES:
            widget=self.tk.Label(self.root,text=text,font=('Courier New',23,'bold'),width=21,pady=22,
                         bg='#283a50',fg='#e9e5d6',bd=3,relief='ridge')
            widget.pack(pady=8)
            self.buttons.append(widget)
        for i,widget in enumerate(self.buttons):
            widget.bind('<Button-1>',lambda e,i=i:self.click(i))
            widget.bind('<Enter>',lambda e,i=i:self.hover(i))
        self.root.bind('<Button-1>',lambda e:self.root.focus_force(),add='+')
        self.tk.Label(self.root,textvariable=self.status,bg='#17273a',fg='#f0dd9f',wraplength=820,
                      font=('Courier New',12),justify='center').pack(pady=(40,10))
        self.root.bind_all('<KeyPress>',self.keypress)
        self.root.protocol('WM_DELETE_WINDOW',self.close)
        self.refresh()
        if self.controller is None:
            from third_branch.xbox_controller import NesInputRouter
            self.controller=NesInputRouter(self.action)
        self._after=self.root.after(80,self.poll)
        self.result=None
        self.take_focus()
    def take_focus(self):
        # A freshly started Windows program is often not the foreground window, so
        # it receives no keyboard events. Raise it briefly and take focus.
        try:
            self.root.update_idletasks();self.root.lift()
            self.root.attributes('-topmost',True)
            self.root.after(400,lambda:self._release_topmost())
        except Exception:pass
        try:self.root.focus_force()
        except Exception:pass
    def _release_topmost(self):
        try:
            if self.root.winfo_exists():self.root.attributes('-topmost',False);self.root.focus_force()
        except Exception:pass
    def hover(self,i):
        if self.picker is not None and self.picker.root.winfo_exists():return
        self.index=i;self.refresh()
    def click(self,i):
        if self.picker is not None and self.picker.root.winfo_exists():return
        self.index=i;self.refresh();self.choose()
    def poll(self):
        if not self.root.winfo_exists():return
        if self.controller is not None:self.controller.poll()
        self._after=self.root.after(80,self.poll)
    def refresh(self):
        for i,b in enumerate(self.buttons):b.configure(bg='#947334' if i==self.index else '#283a50',
                                     fg='#111e32' if i==self.index else '#e9e5d6')
    def keypress(self,event):
        key=event.keysym
        mapping={'Up':'up','Down':'down','Left':'up','Right':'down','Return':'a','space':'a',
                 'Escape':'b','BackSpace':'b','Tab':'select'}
        if key in mapping:self.action(mapping[key]);return 'break'
    def action(self,name):
        if self.picker is not None and self.picker.root.winfo_exists():
            return self.picker.action(name)
        if name in ('up','down','left','right'):
            self.index=1-self.index;self.refresh()
        elif name=='a':self.choose()
        elif name in ('b','start'):self.close()
        elif name=='select' and self.index==0:self.configure_classic()
    def configure_classic(self):
        from .picker import ControllerFileBrowser
        if self.picker is None:
            self.picker=ControllerFileBrowser(self,'rom',self._configured_rom)
    def _configured_rom(self,path):
        from .classic import resolve_classic
        if not valid_nes_rom(path):
            self.status.set('Not a valid NES ROM. Classic was not changed.');return
        data=settings();data['rom']=str(path)
        save_settings(data)
        try:resolve_classic(cfg=data)
        except ClassicUnavailable:
            from .picker import ControllerFileBrowser
            self.picker=ControllerFileBrowser(self,'emulator',self._configured_emulator)
            self.status.set('Choose your installed Mesen/Mesen2 executable with the controller.')
        else:self.status.set('Classic configured. Press A to play.')
    def _configured_emulator(self,path):
        data=settings();data['emulator']=str(path)
        save_settings(data)
        self.status.set('Classic configured. Press A to play.')
    def choose(self):
        if self.index==0:
            try:launch_classic()
            except ClassicUnavailable as exc:
                self.status.set(str(exc)+' Use View/Tab to configure.');return
            self.close()
        else:
            if sys.platform=='win32':
                # Windows: run the game in this same process (no child process to fail silently);
                # main() starts it after this window closes.
                self.result='enhanced';self.close();return
            self.close()
            launch_enhanced()
    def close(self):
        if self.picker is not None:
            try:self.picker.close()
            except Exception:pass
        try:self.root.after_cancel(self._after)
        except Exception:pass
        if self.controller is not None:self.controller.close()
        self.root.destroy()
    def run(self):self.root.mainloop()

def launch_enhanced():
    # This is the EXPANDED game built in this repository. The old Enhanced mode is gone.
    if getattr(sys,'frozen',False):cmd=[sys.executable,'--mode','enhanced']
    else:cmd=[sys.executable,'-m','third_branch.strategic_game']
    return subprocess.Popen(cmd,cwd=str(Path(__file__).resolve().parents[1]) if not getattr(sys,'frozen',False) else None)

def main(argv=None):
    p=argparse.ArgumentParser(description='KOEI two-game Classic / Enhanced launcher')
    p.add_argument('--mode',choices=('classic','enhanced'))
    p.add_argument('--verify-modes',action='store_true')
    args=p.parse_args(argv)
    if args.verify_modes:
        print('\n'.join(MODES))
        return 0
    if args.mode=='enhanced':
        from third_branch.strategic_game import main as game_main
        # The game parser must not receive the launcher's --mode flag.
        sys.argv=[sys.argv[0]]
        return game_main()
    if args.mode=='classic':
        try:launch_classic();return 0
        except ClassicUnavailable as exc:print(str(exc),file=sys.stderr);return 2
    launcher=Launcher();launcher.run()
    if launcher.result=='enhanced':
        from third_branch.strategic_game import main as game_main
        sys.argv=[sys.argv[0]]
        return game_main()
    return 0
