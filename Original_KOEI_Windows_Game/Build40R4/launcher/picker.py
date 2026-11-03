"""Xbox D-pad navigable one-time file configuration for lawful Classic assets.

Unlike an OS file dialog, this browser has a full no-mouse route: D-pad to
choose, A to open/confirm, B to return, View to cancel. It never launches a ROM.
"""
from __future__ import annotations
from pathlib import Path

class ControllerFileBrowser:
    def __init__(self,parent,mode,on_select):
        import tkinter as tk
        if mode not in ('rom','emulator'):raise ValueError('Invalid file browser mode')
        self.parent=parent;self.tk=tk;self.mode=mode;self.on_select=on_select
        self.folder=Path.home();self.entries=[]
        self.root=tk.Toplevel(parent.root)
        self.root.title('Choose '+('your NES ROM' if mode=='rom' else 'Mesen emulator'))
        self.root.geometry('820x615');self.root.configure(bg='#142337')
        self.root.protocol('WM_DELETE_WINDOW',self.close)
        self.label=tk.StringVar(value='')
        tk.Label(self.root,textvariable=self.label,bg='#142337',fg='#eddda8',font=('Arial',11),wraplength=780).pack(pady=12)
        self.options=tk.Listbox(self.root,font=('Courier New',14),bg='#24364d',fg='white',
                                selectbackground='#bb9547',selectforeground='#101d2c',height=19)
        self.options.pack(fill='both',expand=True,padx=22,pady=8)
        self.options.bind('<Double-Button-1>',lambda _:self.action('a'))
        tk.Label(self.root,text='D-pad: move    A: open/select    B: parent/back    View: cancel',
                 bg='#142337',fg='#eddda8',font=('Arial',11)).pack(pady=12)
        self.refresh();self.root.focus_force()
    def refresh(self):
        self.entries=[('parent',self.folder.parent)]
        if self.folder!=Path.home():self.entries.insert(0,('home',Path.home()))
        if self.folder!=Path('/') and Path('/') not in [v for _,v in self.entries]:
            self.entries.insert(0,('root',Path('/')))
        try:
            children=sorted(self.folder.iterdir(),key=lambda p:(not p.is_dir(),p.name.casefold()))
        except (PermissionError,OSError):children=[]
        for path in children:
            if path.is_dir():self.entries.append(('folder',path))
            elif self.mode=='rom' and path.suffix.lower()=='.nes':self.entries.append(('file',path))
            elif self.mode=='emulator' and (path.suffix.lower()=='.exe' or path.name.startswith(('Mesen','mesen'))):
                self.entries.append(('file',path))
        self.options.delete(0,'end')
        for role,path in self.entries:
            title={'parent':'[..] Parent folder','home':'[~] Home','root':'[/] Root','folder':'[DIR] '+path.name,
                   'file':'      '+path.name}[role]
            self.options.insert('end',title)
        self.options.selection_set(0);self.options.activate(0)
        self.label.set('Classic '+self.mode.upper()+' | '+str(self.folder))
    def action(self,name):
        if name in ('up','down','left','right'):
            n=len(self.entries)
            if n:
                current=self.options.curselection()
                i=current[0] if current else 0
                i=(i+(-1 if name in ('up','left') else 1))%n
                self.options.selection_clear(0,'end');self.options.selection_set(i)
                self.options.activate(i);self.options.see(i)
        elif name=='a':
            cur=self.options.curselection()
            if not cur:return
            role,path=self.entries[cur[0]]
            if role=='file':
                self.close();self.on_select(path)
            elif path.is_dir():
                self.folder=path;self.refresh()
        elif name=='b':
            if self.folder==Path.home():self.close()
            else:self.folder=self.folder.parent;self.refresh()
        elif name in ('select','start'):self.close()
    def close(self):
        if self.parent.picker is self:self.parent.picker=None
        self.root.destroy()
