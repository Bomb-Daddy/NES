"""Live Troop Assignments UI; controller D-pad/A/B and Save/Load Start integration.

This is NOT a mock military screen. Each command passes through the exact
Campaign.preview -> execute -> atomic on-disk save path used by the strategy
view. No military reserve is represented or created here.
"""
from __future__ import annotations
from . import army_cohorts as ac

ACTIONS=('Transfer Between Generals','Disband to Population')


def province_armies(campaign,pid):
    s=campaign.state
    return tuple((army['commander'], army['stations'][pid])
                 for _,army in sorted(s['armies'].items()) if pid in army['stations'])


def build_command(s,pid,action,actor,target,number,province=None):
    if action not in ACTIONS:raise ValueError('Invalid troop order')
    if type(number) is not int or number<1:raise ValueError('Positive integer required')
    if action=='Transfer Between Generals':return 'assign_troops',{'target_officer':target,'count':number}
    if action=='Disband to Population':return 'disband_troops',{'count':number}
    raise ValueError('Invalid troop command')


class TroopAssignmentsWindow:
    def __init__(self,province_stage):
        import tkinter as tk
        from tkinter import ttk
        self.tk=tk;self.ttk=ttk
        self.stage=province_stage
        self.parent=province_stage.parent
        self.pid=f'p{self.parent.selected}'
        self.pending=None
        self.index=0
        self.count_step_index=1
        self.count_steps=(1,10,100,1000)
        self.root=tk.Toplevel(self.stage.root)
        self.root.title('Troop Assignments — Real Army Ledger')
        self.root.geometry('900x675')
        self.root.minsize(650,550)
        self.root.configure(bg='#101c30')
        self.root.protocol('WM_DELETE_WINDOW',self.close)
        self.officers=self._officers()
        self.action=tk.StringVar(value=ACTIONS[0])
        self.actor=tk.StringVar(value=self.officers[0] if self.officers else '')
        self.target=tk.StringVar(value=self.officers[-1] if self.officers else '')
        self.count=tk.StringVar(value='10')
        tk.Label(self.root,text='TROOP ASSIGNMENTS  |  PROVINCE '+self.pid[1:].zfill(2),bg='#101c30',fg='#f2d68e',font=('DejaVu Sans',18,'bold')).pack(pady=12)
        self.overview=tk.StringVar(value='')
        tk.Label(self.root,textvariable=self.overview,bg='#101c30',fg='#ebeadb',font=('DejaVu Sans',12),justify='left',anchor='nw',wraplength=780).pack(fill='x',padx=24,pady=7)
        self.fields=(('Action',self.action,lambda:ACTIONS),('General',self.actor,self._officers),
                     ('To General',self.target,self._officers),('Troop Count',self.count,None))
        self.controls=[]
        form=tk.Frame(self.root,bg='#101c30');form.pack(fill='x',padx=24,pady=10)
        for i,(name,var,choices) in enumerate(self.fields):
            tk.Label(form,text=name,bg='#101c30',fg='#e6d8b2',anchor='w',font=('DejaVu Sans',12)).grid(row=i,column=0,sticky='w',pady=7)
            if choices:
                widget=ttk.Combobox(form,textvariable=var,values=choices(),state='readonly',width=40)
                widget.bind('<<ComboboxSelected>>',lambda e:self._edited())
            else:
                widget=tk.Entry(form,textvariable=var,width=44)
                widget.bind('<KeyRelease>',lambda e:self._edited())
            widget.grid(row=i,column=1,sticky='ew',padx=20,pady=6)
            self.controls.append(widget)
        form.grid_columnconfigure(1,weight=1)
        self.info=tk.StringVar(value='NES D-PAD: choose field / value   A: preview, then commit   B: cancel   Start: Save/Load')
        tk.Label(self.root,textvariable=self.info,bg='#101c30',fg='#f1dfaa',wraplength=780,justify='left',font=('DejaVu Sans',11)).pack(fill='x',padx=24,pady=10)
        bar=tk.Frame(self.root,bg='#101c30');bar.pack(pady=12)
        self.preview_button=tk.Button(bar,text='Preview / Commit (NES A)',command=self.submit)
        self.preview_button.pack(side='left',padx=8)
        tk.Button(bar,text='Cancel / Back (NES B)',command=self.close).pack(side='left',padx=8)
        tk.Button(bar,text='Save / Load (NES Start)',command=self.parent.pause).pack(side='left',padx=8)
        self.refresh()
        self.root.focus_force()

    def active(self):return self.root.winfo_exists()
    def _officers(self):
        s=self.parent.c.state
        return tuple(o for o,data in sorted(s['core']['officers'].items())
                     if data['province_id']==self.pid and s['officer_faction'][o]==self.parent.player)
    def _destinations(self):
        s=self.parent.c.state
        return tuple(n for n in s['neighbors'][self.pid] if s['owners'][n]==self.parent.player)
    def refresh(self):
        s=self.parent.c.state;p=s['core']['provinces'][self.pid]
        armies=province_armies(self.parent.c,self.pid)
        roster='\n'.join(f'  Army {oid}  ·  {strength} assigned soldiers' for oid,strength in armies)
        if not roster:roster='  No assigned army stationed here (no troop reserve exists)'
        self.overview.set(f'Assigned soldiers: {p["troops"]}\n'
                          f'{roster}\nTransfer moves existing soldiers between local generals. Disband returns survivors to population.')
        for (name,var,choices),widget in zip(self.fields,self.controls):
            if choices and isinstance(widget,self.ttk.Combobox):widget.configure(values=choices())
        editable=s['owners'][self.pid]==self.parent.player
        self.preview_button.configure(state='normal' if editable else 'disabled')
        if not editable:self.info.set('Read-only enemy province. NES B returns to province.')

    def _edited(self):self.pending=None
    def _candidate(self):
        if self.parent.c.state['owners'][self.pid]!=self.parent.player:raise ValueError('Enemy-controlled province')
        name=self.action.get()
        raw=self.count.get().strip()
        if not raw.isascii() or not raw.isdecimal() or int(raw)<1:raise ValueError('Invalid troop count')
        number=int(raw)
        oid=self.actor.get()
        if oid not in self._officers():raise ValueError('General is not stationed in this friendly province')
        kind,params=build_command(self.parent.c.state,self.pid,name,oid,self.target.get(),number)
        return kind,oid,params,self.parent.c.fingerprint

    def submit(self):
        try:
            kind,oid,params,fingerprint=self._candidate()
            candidate=(kind,oid,str(sorted(params.items())),fingerprint)
            if self.pending!=candidate:
                quote=self.parent.c.preview(kind,oid,params,'cohort-preview:'+fingerprint[:16])
                self.pending=candidate
                self.info.set(f'PREVIEW {kind}: {quote["result"]}. Press NES A again to COMMIT; NES B cancels.')
                return
            self.pending=None
            if self.parent.issue(kind,params,actor=oid,confirm=False):
                self.info.set('Army order committed and saved to disk. No unassigned military reserve.')
                self.stage.speaker=oid
            else:self.info.set('Order rejected; state unchanged. '+self.parent.msg.get())
            self.refresh();self.stage.redraw()
        except Exception as e:
            # Domain previews raise CoreError: always leave game unchanged.
            self.pending=None
            self.info.set('Cannot commit: '+str(e))
            self.refresh()

    def on_nes_button(self,button):
        if button=='b':
            if self.pending:
                self.pending=None
                self.info.set('Preview canceled; no army or population changed')
            else:self.close()
            return
        if button=='start':return self.parent.pause()
        if button=='select':
            self.count_step_index=(self.count_step_index+1)%len(self.count_steps)
            self.info.set(f'NES Select: troop adjustment step {self.count_steps[self.count_step_index]}')
            return
        if button in ('up','down'):
            self.index=(self.index+(-1 if button=='up' else 1))%(len(self.fields)+1)
            self.info.set('Selected: '+(self.fields[self.index][0] if self.index<len(self.fields) else 'Preview / Commit'))
            return
        if button=='a':return self.submit()
        if button in ('left','right') and self.index<len(self.fields):
            name,var,choices=self.fields[self.index]
            d=-1 if button=='left' else 1
            if choices:
                values=choices()
                if values:
                    current=values.index(var.get()) if var.get() in values else 0
                    var.set(values[(current+d)%len(values)])
            else:
                n=int(var.get()) if var.get().isascii() and var.get().isdecimal() else 1
                var.set(str(max(1,n+d*self.count_steps[self.count_step_index])))
            self.pending=None
            self.info.set(f'{name}: {var.get()} (NES A preview, then commit)')

    def close(self):
        self.pending=None
        if self.active():self.root.destroy()
        if getattr(self.stage,'troop_dialog',None) is self:self.stage.troop_dialog=None
        if self.stage.active():self.stage.root.focus_force()
