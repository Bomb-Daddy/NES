"""Live native Tk orders, not stubs: each supported order previews and commits
through the original-game Campaign command bus and persistent state.

A player can inspect all provinces, but only issue orders from player-controlled
provinces. All order parameters are typed; invalid values fail before commit.
"""
from __future__ import annotations
from .integrated import COMMANDS, DIPLO, PRICE_CENTS

WORKERS=('farmers','town','artisans','metalworkers','resources','land','flood')
TRAITS=('politics','war','leadership','intelligence','charisma')

# name -> (engine command, ordered fields). Values for fields are UI defaults.
SPECS={
 'Allocate Workers':('allocate_workers',(('source',WORKERS,'farmers'),('target',WORKERS,'town'),('count',None,'10'))),
 'Distribute Food':('distribute_food',(('units',None,'10'),)),
 'Distribute Goods':('distribute_goods',(('units',None,'3'),)),
 'Recruit':('recruit',(('count',None,'10'),('worker_source',('auto',)+WORKERS,'auto'))),
 'Recruit Officer':('recruit_officer',(('officer_id',None,'nes:000'),)),
 'Assign Troops':('assign_troops',(('target_officer',None,'O02'),('count',None,'10'))),
 'Disband Soldiers':('disband_troops',(('count',None,'10'),)),
 'Move Army':('relocate_army',(('target',None,'p1'),)),
 'Market':('market',(('commodity',tuple(PRICE_CENTS),'food'),('side',('buy','sell'),'buy'),('units',None,'5'))),
 'Train Officer':('train',(('trait',TRAITS,'war'),('effort',None,'1'))),
 'Train Troops':('train_troops',()),
 'Equip Arms':('equip_arms',(('general',None,'O01'),('units',None,'1'))),
 'Transport':('transport',(('target',None,'p1'),('commodity',('gold','food','specialty','troops','metal','arms','furs'),'food'),('units',None,'10'))),
 'Reward Governor':('reward_governor',(('target_governor',None,'O02'),('method',('gold','horse'),'gold'),('gold',None,'20'))),
 'Reward Officer':('reward_officer',(('target_officer',None,'O02'),('method',('gold','horse','writings'),'gold'),('gold',None,'20'))),
 'Buy Horse':('buy_horse',(('units',None,'1'),)),
 'Appoint Advisor':('appoint_advisor',(('target_officer',None,'O02'),)),
 'Swear Brotherhood':('swear_brotherhood',(('target_officer',None,'O02'),)),
 'Feast':('feast',()),
 'Rest':('rest',()),
 'Appoint Governor':('appoint_governor',(('officer',None,'O01'),('focus',('farms','industry','defense','personnel'),'farms'))),
 'Plan Attack':('plan_attack',(('target',None,'p1'),)),
 'Attack':('attack',(('target',None,'p1'),('troops',None,'80'))),
 'Ally':('Ally',(('target',None,'B'),)),
 'Joint':('Joint',(('target',None,'B'),('objective',None,'p1'),('promised_troops',None,'20'))),
 'Marry':('Marry',(('target',None,'B'),)),
 'Give':('Give',(('target',None,'B'),('gold_cents',None,'500'))),
 'Cancel Alliance':('Cancel',(('target',None,'B'),)),
 'Threat':('Threat',(('target',None,'B'),)),
}
# Battle-only orders are intentionally handled by TacticalWindow instead of the province OrdersWindow.
BATTLE_ONLY = frozenset({'battle_move','battle_wait','battle_move_enemy','battle_attack',
                         'battle_bribe','battle_duel','battle_duel_response',
                         'battle_reinforce','battle_withdraw','battle_prisoner',
                         'battle_end_turn','battle_auto'})
assert {c for c,_ in SPECS.values()} == COMMANDS - BATTLE_ONLY, 'Unexposed main-game command'
assert BATTLE_ONLY <= COMMANDS, 'Battle command category changed: reconcile UI before release'

NUMERIC_FIELDS={'count','units','effort','troops','promised_troops','gold_cents','gold'}
PROVINCE_FIELD_CMDS={'allocate_workers','distribute_food','distribute_goods','recruit'}


def params_for_order(name, field_values, current_province):
    """Convert form values into the actual command's validated parameter shape."""
    if name not in SPECS:
        raise ValueError('Unknown order')
    cmd,fields=SPECS[name]
    required={key for key,_,_ in fields}
    optional={'worker_source'} if cmd=='recruit' else set()
    if not required-optional <= set(field_values) <= required:
        raise ValueError('Missing or unknown order parameter')
    params={}
    for key,choices,_ in fields:
        if key in optional and key not in field_values:continue
        raw=str(field_values[key]).strip()
        if key=='worker_source' and raw=='auto':continue
        if key in NUMERIC_FIELDS:
            if not raw or not raw.isascii() or not raw.isdecimal() or int(raw)<1:
                raise ValueError(f'{key} must be a positive integer')
            params[key]=int(raw)
        else:
            if not raw:raise ValueError(f'{key} is required')
            if choices and raw not in choices:raise ValueError(f'{key} is not in the allowed list')
            params[key]=raw
    if cmd in PROVINCE_FIELD_CMDS:
        params['province_id']=current_province
    return cmd,params


def eligible_governors(s,faction):
    """Real, appointed governor roster for the selected ruler; stable order, no UI dependency."""
    rows=[]
    from .officer_relations import ruler
    ruler_id=ruler(s,faction)
    for pid,posting in sorted(s.get('governors',{}).items(),key=lambda pair: (0,int(pair[0][1:])) if pair[0].startswith('p') and pair[0][1:].isdigit() else (1,pair[0])):
        oid=posting.get('officer')
        if s['owners'].get(pid)!=faction or oid not in s['core']['officers']:
            continue
        if oid==ruler_id or s['officer_faction'].get(oid)!=faction or s['core']['officers'][oid]['province_id']!=pid:
            continue
        rows.append((oid,pid,s['officer_loyalty'][oid]))
    return rows


class OrdersWindow:
    def __init__(self, parent, allowed_orders=None, initial_order=None):
        import tkinter as tk
        from tkinter import ttk
        self.tk,self.ttk=tk,ttk
        self.allowed_orders=tuple(allowed_orders) if allowed_orders is not None else tuple(SPECS)
        if not self.allowed_orders or any(x not in SPECS for x in self.allowed_orders):
            raise ValueError('Invalid approved command category')
        if initial_order is not None and initial_order not in self.allowed_orders:
            raise ValueError('Command is outside selected category')
        self.parent=parent
        # Director ruling 2026-11-02: there is no "inspect only" Orders window. Orders exist only for a
        # player-controlled province that has one of the player's officers; other provinces are viewed
        # through the province screen, whose only command is Exit.
        _pid=f'p{parent.selected}'
        if (parent.c.state['owners'][_pid]!=parent.player or not any(
                o['province_id']==_pid and parent.c.state['officer_faction'][oid]==parent.player
                for oid,o in parent.c.state['core']['officers'].items())):
            raise ValueError('Orders need a player-controlled province with one of your officers')
        self.root=tk.Toplevel(parent.root)
        self.root.title('KOEI Enhanced — Orders')
        self.root.geometry('700x525')
        self.root.resizable(False,False)
        self.pid=f'p{parent.selected}'
        self.fields={};self.controller_index=1;self.pending=None;self.governor_tree=None
        owner=parent.c.state['owners'][self.pid]
        tk.Label(self.root,text=f'Province {parent.selected:02d} | Faction {owner or "Vacant"}',font=('Arial',14,'bold')).pack(pady=5)
        officers=[oid for oid,o in sorted(parent.c.state['core']['officers'].items())
                  if o['province_id']==self.pid and parent.c.state['officer_faction'][oid]==parent.player]
        self.actor=tk.StringVar(value=officers[0]);self.order=tk.StringVar(value=initial_order or self.allowed_orders[0])
        top=tk.Frame(self.root);top.pack(pady=8)
        tk.Label(top,text='Officer').grid(row=0,column=0)
        self.actor_widget=ttk.Combobox(top,textvariable=self.actor,values=officers,state='readonly',width=10)
        self.actor_widget.grid(row=0,column=1,padx=8)
        tk.Label(top,text='Order').grid(row=0,column=2)
        selector=ttk.Combobox(top,textvariable=self.order,values=self.allowed_orders,state='readonly',width=20)
        selector.grid(row=0,column=3,padx=8)
        selector.bind('<<ComboboxSelected>>',lambda e:self._fields())
        self.fieldframe=tk.Frame(self.root);self.fieldframe.pack(fill='both',padx=20,pady=10)
        self.fields={}
        self.info=tk.StringVar(value='Preview uses authoritative game rules. Confirm before commit.')
        tk.Label(self.root,textvariable=self.info,wraplength=490,justify='left',fg='#23364e').pack(pady=12)
        self.map_target_button=tk.Button(self.root,text='Choose Target Province on Map',command=self.select_target_on_map)
        # Shown only when the selected order has a real province-target field;
        # irrelevant controls never clutter the player UI.

        tk.Button(self.root,text='Preview and confirm order',command=self.submit).pack(pady=8)
        tk.Button(self.root,text='Close',command=self.root.destroy).pack()
        self._fields()
        self.root.protocol('WM_DELETE_WINDOW',self.close)
    def close(self):
        self.pending=None
        self.root.destroy()
    def _controller_items(self):
        if not hasattr(self,'actor'):return [('close',None)]
        return [('actor',self.actor),('order',self.order)]+[(name,var) for name,var in self.fields.items()]+[('confirm',None),('close',None)]
    def on_nes_button(self,button):
        """NES D-pad moves form focus/changes value, A selects/confirms, B cancels."""
        if button=='b':
            if self.pending is not None:
                self.pending=None;self.info.set('Order preview cancelled; no state changed')
            else:self.close()
            return
        if button=='start':return self.parent.pause()  # NES Start opens the canonical Save/Load menu; A confirms an order
        if button=='select':
            return self.select_target_on_map()
        choices=self._controller_items()
        self.controller_index=min(self.controller_index,len(choices)-1)
        if button in ('up','down'):
            self.controller_index=(self.controller_index + (-1 if button=='up' else 1))%len(choices)
            if hasattr(self,'info'):self.info.set('Controller field: '+choices[self.controller_index][0])
            return
        name,var=choices[self.controller_index]
        if button=='a':
            if name=='confirm':return self.controller_submit()
            if name=='close':return self.close()
        if name in ('confirm','close'):return
        if button not in ('a','left','right'):return
        values=()
        if name=='actor' and self.order.get()=='Reward Governor':
            from .officer_relations import ruler
            values=(ruler(self.parent.c.state,self.parent.player),)
        elif name=='actor':
            values=tuple(oid for oid,o in sorted(self.parent.c.state['core']['officers'].items())
                  if o['province_id']==self.pid and self.parent.c.state['officer_faction'][oid]==self.parent.player)
        elif name=='order':values=self.allowed_orders
        else:
            specs={key:(c,d) for key,c,d in SPECS[self.order.get()][1]}
            if name in specs:values=tuple(specs[name][0] or ())
        if not values and name in ('target','objective') and var.get().startswith('p'):
            values=tuple('p'+str(n) for n in range(1,42))
        if not values and name=='target' and SPECS[self.order.get()][0] in DIPLO:
            values=tuple(f for f in sorted(self.parent.c.state['factions'])
                         if f!=self.parent.player and self.parent.c.state['factions'][f]['alive'])
        if not values and name=='officer_id':
            from .historical_s1 import list_recruitable
            values=tuple(list_recruitable(self.parent.c.state,self.pid))
        if not values and name=='target_governor':
            values=tuple(oid for oid,_,_ in eligible_governors(self.parent.c.state,self.parent.player))
        if not values and name=='general':
            values=tuple(oid for oid,off in sorted(self.parent.c.state['core']['officers'].items())
                         if off['province_id']==self.pid and self.parent.c.state['officer_faction'][oid]==self.parent.player)
        if not values and name=='target_officer':
            values=tuple(oid for oid,off in sorted(self.parent.c.state['core']['officers'].items())
                         if off['province_id']==self.pid and self.parent.c.state['officer_faction'][oid]==self.parent.player)
        if not values and name=='officer':
            values=tuple(oid for oid in sorted(self.parent.c.state['core']['officers'])
                  if self.parent.c.state['officer_faction'][oid]==self.parent.player)
        if values:
            index=values.index(var.get()) if var.get() in values else 0
            var.set(values[(index+(-1 if button=='left' else 1))%len(values)])
            if name=='order':
                self._fields()
                self.controller_index=1
            elif name=='target_governor' and self.governor_tree is not None:
                self.governor_tree.selection_set(var.get())
                self.governor_tree.see(var.get())
        else:
            raw=var.get()
            if raw.isascii() and raw.isdecimal():var.set(str(max(1,int(raw)+(-1 if button=='left' else 1))))
            elif name in ('target','objective') and raw.startswith('p') and raw[1:].isdecimal():
                var.set('p'+str(1+(int(raw[1:])-1+(-1 if button=='left' else 1))%41))
        self.pending=None
    def select_target_on_map(self):
        candidate='target' if 'target' in self.fields else ('objective' if 'objective' in self.fields else None)
        if candidate is None:
            self.info.set('This order does not select a target province on the map.')
            return
        self.parent.show_china_map(mode='target',target_dialog=self,target_field=candidate)

    def controller_submit(self):
        """Two-step PREVIEW then COMMIT with no keyboard/mouse/modal dialog required."""
        if not hasattr(self,'actor'):return
        try:
            kind,params=params_for_order(self.order.get(),{k:v.get() for k,v in self.fields.items()},self.pid)
            actor=self.actor.get();fingerprint=self.parent.c.fingerprint
            candidate=(kind,params,actor,fingerprint)
            if self.pending!=candidate:
                self.parent.c.preview(kind,actor,params,'gamepad-preview:'+fingerprint[:12])
                self.pending=candidate
                self.info.set('PREVIEW '+kind+' '+str(params)+' — press A again to COMMIT; B cancels; Menu pauses.')
                return
            self.pending=None
            if self.parent.issue(kind,params,actor=actor,confirm=False):
                self.info.set('Committed '+kind+' and saved.')
            else:self.info.set('Order rejected without committing. Inspect main status.')
        except Exception as exc:  # rejects CoreError preview without changing the authoritative campaign
            self.pending=None
            self.info.set('Order invalid: '+str(exc))
    def _fields(self):
        for widget in self.fieldframe.winfo_children():widget.destroy()
        self.fields={}
        self.governor_tree=None
        if self.order.get()=='Reward Governor':
            from .officer_relations import ruler
            actor_id=ruler(self.parent.c.state,self.parent.player)
            self.actor.set(actor_id)
            self.actor_widget.configure(values=(actor_id,))
        else:
            local=[oid for oid,o in sorted(self.parent.c.state['core']['officers'].items())
                   if o['province_id']==self.pid and self.parent.c.state['officer_faction'][oid]==self.parent.player]
            self.actor_widget.configure(values=local)
            if self.actor.get() not in local and local:self.actor.set(local[0])
        _,spec=SPECS[self.order.get()]
        if hasattr(self,'map_target_button'):
            self.map_target_button.pack_forget()
            if any(name in ('target','objective') for name,_,_ in spec):
                self.map_target_button.pack(pady=4)
        pid=self.pid
        for idx,(name,choices,default) in enumerate(spec):
            value=default
            if name in ('officer','general'):value=self.actor.get()
            elif name=='target' and not choices:  # a target WITH a fixed choice list (Allocate Workers job) keeps its own default
                command=SPECS[self.order.get()][0]
                if command in DIPLO:
                    owner=self.parent.player
                    value=next((f for f in sorted(self.parent.c.state['factions']) if f!=owner and self.parent.c.state['factions'][f]['alive']),'B')
                else:
                    ns=self.parent.c.state['neighbors'][pid]
                    friends=(n for n in ns if self.parent.c.state['owners'][n]==self.parent.player)
                    enemies=(n for n in ns if self.parent.c.state['owners'][n] not in (self.parent.player,None))
                    value=next(friends if command in ('transport','relocate_army') else enemies, ns[0] if ns else 'p1')
            if name=='target_governor':
                roster=eligible_governors(self.parent.c.state,self.parent.player)
                value=roster[0][0] if roster else ''
            if name=='target_officer':
                available=[oid for oid,off in sorted(self.parent.c.state['core']['officers'].items())
                           if off['province_id']==pid and self.parent.c.state['officer_faction'][oid]==self.parent.player and oid!=self.actor.get()]
                value=available[0] if available else self.actor.get()
            if name=='objective':value=next((p for p in self.parent.c.state['owners'] if self.parent.c.state['owners'][p]!=self.parent.player),'p1')
            var=self.tk.StringVar(value=value)
            self.fields[name]=var
            self.tk.Label(self.fieldframe,text=name.replace('_',' ').title(),width=18,anchor='e').grid(row=idx,column=0,pady=5)
            if name=='target_governor':
                # A visible three-column roster makes appointments, provinces and
                # loyalty readable without typing officer IDs. D-pad cycles the
                # same options; A previews and then commits.
                roster=eligible_governors(self.parent.c.state,self.parent.player)
                holder=self.ttk.Frame(self.fieldframe)
                holder.grid(row=idx,column=1,sticky='w',pady=5)
                tree=self.ttk.Treeview(holder,columns=('governor','province','loyalty'),show='headings',height=min(8,max(2,len(roster))))
                for col,width in (('governor',145),('province',90),('loyalty',85)):
                    tree.heading(col,text=col.title());tree.column(col,width=width,stretch=False,anchor='center')
                catalog=self.parent.c.state.get('officer_catalog',{})
                for oid,gpid,loyalty in roster:
                    name=catalog.get(oid,{}).get('name',oid)
                    display=f'{name} ({oid})' if name!=oid else oid
                    tree.insert('', 'end', iid=oid,values=(display,gpid,str(loyalty)+'/100'))
                tree.pack(side='left')
                scrollbar=self.ttk.Scrollbar(holder,orient='vertical',command=tree.yview)
                scrollbar.pack(side='right',fill='y')
                tree.configure(yscrollcommand=scrollbar.set)
                def on_select(_event, t=tree, target=var):
                    selection=t.selection()
                    if selection:target.set(selection[0]);self.pending=None
                tree.bind('<<TreeviewSelect>>',on_select)
                if roster:tree.selection_set(roster[0][0])
                self.governor_tree=tree
            elif choices:self.ttk.Combobox(self.fieldframe,textvariable=var,values=choices,state='readonly',width=24).grid(row=idx,column=1,pady=5)
            else:self.tk.Entry(self.fieldframe,textvariable=var,width=28).grid(row=idx,column=1,pady=5)
    def submit(self):
        try:
            kind,params=params_for_order(self.order.get(),{k:v.get() for k,v in self.fields.items()},self.pid)
            self.parent.issue(kind,params,actor=self.actor.get(),confirm=True)
            self.info.set('Submitted; inspect the main game status and province changes.')
        except (ValueError,KeyError) as exc:
            self.info.set('Invalid input: '+str(exc))
