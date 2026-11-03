"""State-driven tactical battle presentation for the original-game campaign.

Reads a real Campaign.battle (terrain, position, troops, day, owner) and renders
separate terrain and living-army layers. In 41-province scenarios the actual
seasonal 624x576 per-province PNG is rendered under dynamic live army overlays.
All 41 logical tactical grids now match hash-verified English NES ROM bytes.
Legacy seven-province saves retain the synthetic fallback for compatibility.
"""
from __future__ import annotations
from PIL import Image, ImageDraw, ImageFont

WIDTH, HEIGHT = 1024, 768  # 4:3 (CRT), Director ruling 2026-11-02
GRID_W, GRID_H = 13, 12
CELL = 44
X0, Y0 = 20, 70
TX = X0 + GRID_W*CELL + 20  # right-hand text column
TERRAIN = {
    '.': (77, 120, 65), 'F': (39, 90, 51), 'H': (133, 115, 80),
    'W': (53, 108, 151), 'C': (170, 145, 112), 'M': (118, 112, 113)
}
SIDES = {'a': (219, 92, 69), 'd': (100, 155, 227)}
UI_FONT=ImageFont.load_default(size=17)
HEADING_FONT=ImageFont.load_default(size=22)
COUNT_FONT=ImageFont.load_default(size=13)

def _army(draw, key, unit, commander=False, burning=False):
    """Draw an active army using actual troop count; no markers for zero troops."""
    count = unit['troops']
    if count <= 0:
        return
    q, r = unit['q'], unit['r']
    x, y = X0 + q*CELL, Y0 + r*CELL
    color = SIDES[key]
    draw.rectangle((x+3,y+3,x+CELL-5,y+CELL-5), fill=color, outline=(10,18,24), width=2)
    draw.rectangle((x+6,y+25,x+CELL-8,y+CELL-8), fill=(14,25,39))
    draw.text((x+8,y+26), str(count), font=COUNT_FONT, fill=(250,249,229))
    if commander:
        draw.line((x+11,y+7,x+11,y+21), fill=(250,243,191), width=2)
        draw.line((x+6,y+14,x+16,y+14), fill=(250,243,191), width=2)
    if burning:
        draw.polygon(((x+27,y+21),(x+23,y+14),(x+29,y+6),(x+36,y+15),
                      (x+33,y+21)), fill=(248,176,58))

def render_battle(campaign, selected_unit='a'):
    """Returns a full 1024x768 (4:3) rendered battle from LIVE serialized state."""
    battle = campaign.state.get('battle')
    if not battle:
        raise ValueError('No active battle')
    grid = battle['terrain']
    if len(grid) != GRID_H or any(len(line) != GRID_W for line in grid):
        raise ValueError('Invalid tactical grid size')
    picture = Image.new('RGB', (WIDTH, HEIGHT), (18,28,40))
    real_asset=battle.get('map_source')=='battle-asset-v1.4-shifted-records'
    if real_asset:
        from .battle_assets import terrain_matches, background, season_for_month
        province=int(battle['dst'][1:]);season=season_for_month(campaign.state['month'])
        terrain_matches(province,grid)  # fail closed on mismatched saved terrain
        background_map=background(province,season).resize((GRID_W*CELL,GRID_H*CELL),Image.Resampling.NEAREST)
        picture.paste(background_map,(X0,Y0))
    d = ImageDraw.Draw(picture)
    if not real_asset:
        for r, line in enumerate(grid):
            for q, t in enumerate(line):
                x,y=X0+q*CELL,Y0+r*CELL
                c=TERRAIN.get(t)
                if c is None:
                    raise ValueError('Unknown terrain type '+str(t))
                d.rectangle((x,y,x+CELL-2,y+CELL-2), fill=c, outline=(22,35,32))
                if t=='W':d.line((x+8,y+12,x+33,y+12),fill=(155,193,211),width=2)
                elif t=='H':d.polygon(((x+7,y+32),(x+20,y+8),(x+35,y+32)),fill=(180,151,101))
                elif t=='F':
                    for dx in (8,25):d.polygon(((x+dx,y+4),(x+dx-5,y+28),(x+dx+6,y+28)),fill=(19,60,40))
                elif t=='C':
                    d.rectangle((x+8,y+12,x+33,y+31),fill=(184,170,142),outline=(62,52,46),width=2)
                    d.polygon(((x+6,y+14),(x+20,y+4),(x+34,y+14)),fill=(89,66,54))
    # Units are overlaid after terrain, not baked into maps.
    # Q01–Q05: display each real officer-commanded army on its own hex.
    if battle.get('officer_units'):
        for unit in sorted(battle['officer_units'],key=lambda x:(x['side'],x['id'])):
            _army(d,'a' if unit['side']=='A' else 'd',unit,
                  commander=unit['commander'],
                  burning=([unit['q'],unit['r']] in battle.get('burning',[])))
    else:
        _army(d,'a',battle['a'],commander=True, burning=([battle['a']['q'],battle['a']['r']] in battle.get('burning',[])))
        _army(d,'d',battle['d'],commander=True, burning=([battle['d']['q'],battle['d']['r']] in battle.get('burning',[])))
    # Actual campaign/turn/unit data drives all labels.
    d.text((X0,26),f'BATTLE  {battle["src"].upper()} > {battle["dst"].upper()}  DAY {battle["day"]}',font=HEADING_FONT, fill=(243,220,142))
    d.text((TX,95),f'ATTACKER {battle["attacker"]}',font=UI_FONT, fill=(246,179,144))
    d.text((TX,125),f'Troops {battle["a"]["troops"]}  Morale {battle["a"]["morale"]}',font=UI_FONT, fill=(237,238,228))
    d.text((TX,175),f'DEFENDER {battle["defender"]}',font=UI_FONT, fill=(161,205,255))
    d.text((TX,205),f'Troops {battle["d"]["troops"]}  Morale {battle["d"]["morale"]}',font=UI_FONT, fill=(237,238,228))
    d.text((TX,263),f'Province {battle["dst"][1:]}  Season {((campaign.state["month"]-1)//3)%4+1}/4  Weather {battle.get("weather","?")}',font=UI_FONT, fill=(235,215,166))
    d.text((TX,297),'Move / Normal / Charge / Simult. / Fire',font=UI_FONT, fill=(218,224,226))
    if battle.get('officer_units'):
        selected=selected_unit if selected_unit not in ('a','d') else battle.get('selected_officer')
        u=next((u for u in battle['officer_units'] if u['id']==selected),None)
        if u:
            d.text((TX,400),f'Officer: {selected}   Troops {u["troops"]}   Mobility {u["mp"]}',font=UI_FONT,fill=(247,234,173))
    if real_asset:
        d.text((TX,325),'41 x 4 SEASONAL MAP: '+season.upper(),font=UI_FONT, fill=(225,207,142))
        d.text((TX,348),'41 original ROM terrain grids verified;',font=UI_FONT, fill=(225,182,132))
        d.text((TX,370),'placement AI pending',font=UI_FONT, fill=(225,182,132))
    else:
        d.text((TX,325),'Legacy 7-province provisional grid',font=UI_FONT, fill=(225,182,132))
    d.text((X0,Y0+GRID_H*CELL+16),'LIVE ARMY MARKERS: troop count / commander;',font=UI_FONT, fill=(236,235,210))
    d.text((X0,Y0+GRID_H*CELL+40),'zero-strength armies vanish.',font=UI_FONT, fill=(236,235,210))
    return picture

class TacticalWindow:
    """Actual graphical player controls: every button dispatches a real battle command."""
    def __init__(self, parent):
        import tkinter as tk
        from PIL import ImageTk
        self.parent=parent
        self.tk=tk
        self.ImageTk=ImageTk
        self.root=tk.Toplevel(parent.root)
        self.root.title('KOEI Enhanced — Battle')
        self.root.geometry(f'{WIDTH}x{HEIGHT}')
        self.root.resizable(False,False)
        self.canvas=tk.Canvas(self.root,width=WIDTH,height=HEIGHT,highlightthickness=0)
        self.canvas.pack()
        self.message=tk.StringVar(value='Battle commands change the real campaign ledger.')
        tk.Label(self.root,textvariable=self.message,bg='#12202c',fg='#fce4b9',wraplength=380,justify='left').place(x=TX,y=424,width=392,height=65)
        commands=[('Move',self.move),('Normal',self.attack),('Charge',self.charge),('Simult.',self.simult),('Fire',self.fire),('End Turn',self.end_turn),('Auto Resolve',self.auto),('Close',self.close)]
        commands.insert(1,('Next Officer',self.next_officer))
        commands.insert(2,('Next Target',self.next_target))
        commands.insert(3,('Rest',self.wait))
        commands.insert(4,('Move Enemy',self.move_enemy))
        commands.insert(5,('Bribe',self.bribe))
        commands.insert(6,('Duel',self.duel))
        commands.insert(7,('Reinforce',self.reinforce))
        commands.insert(8,('Withdraw',self.withdraw))
        commands.insert(9,('Prisoner Fate',self.prisoner))
        self.bribe_gold=1
        self.duel_accept=True
        self.reinforce_index=0;self.reinforce_rice=0;self.reinforce_gold=0
        self.withdraw_index=0;self.withdraw_whole=False
        self.prisoner_index=0;self.prisoner_decision_index=0
        self.controller_actions=commands;self.controller_index=0;self.targeting=False;self.target_mode='battle_move'
        battle=parent.c.state.get('battle',{})
        human_side=('D' if parent.c.state.get('human_faction')==battle.get('defender') else 'A')
        own=[u['id'] for u in battle.get('officer_units',()) if u['side']==human_side
             and u['troops100']>=100 and not u.get('reserve') and not u.get('fled')]
        default=battle.get('selected_officer')
        self.selected_officer=default if default in own else (own[0] if own else None)
        self.selected_target=None
        self.target_q=0;self.target_r=0
        bar=tk.Frame(self.root)
        bar.place(x=TX,y=498)
        self.action_buttons=[]
        for n,(name,action) in enumerate(commands):
            btn=tk.Button(bar,text=name,command=action);btn.grid(row=n//3,column=n%3)
            self.action_buttons.append(btn)
        self._cursor()
        self.redraw()
        if parent.c.state.get('battle',{}).get('duel_pending'):
            self.message.set('CPU CHALLENGE: Next Officer chooses your duelist; Duel answers. SELECT toggles Accept/Refuse.')
    def _cursor(self):
        for idx,button in enumerate(self.action_buttons):
            button.config(relief='sunken' if idx==self.controller_index else 'raised')
    def on_nes_button(self,button):
        if button=='start':return self.parent.pause()
        if button=='select':
            if (not self.targeting and self.controller_actions[self.controller_index][0]=='Duel'
                    and self.parent.c.state.get('battle',{}).get('duel_pending')):
                self.duel_accept=not self.duel_accept
                self.message.set('Opening challenge: '+('ACCEPT' if self.duel_accept else 'REFUSE')+
                                 '; NES A confirms')
                return
            if not self.targeting and self.controller_actions[self.controller_index][0]=='Prisoner Fate':
                self.prisoner_index+=1
                self.message.set('Prisoner: '+str(self._prisoner_choice()))
                return
            if not self.targeting and self.controller_actions[self.controller_index][0]=='Reinforce':
                self.reinforce_index+=1
                self.message.set('Reinforcement officer: '+str(self._reinforcement_choice()))
                return
            if not self.targeting and self.controller_actions[self.controller_index][0]=='Withdraw':
                self.withdraw_index+=1
                self.message.set('Withdrawal destination: '+str(self._withdraw_destination()))
                return
            if not self.targeting and self.controller_actions[self.controller_index][0]=='Bribe':
                self.bribe_gold=1+(self.bribe_gold%99)
                self.message.set(f'Bribe offer: {self.bribe_gold} gold. NES A to confirm.')
                return
            if not self.targeting:return self.start_target()
            self.targeting=False;self.redraw();return
        if button=='b':
            if self.targeting:
                self.targeting=False;self.message.set('Move cancelled');self.redraw()
            else:self.close()
            return
        if self.targeting:
            if button in ('up','down','left','right'):
                self.target_q=max(0,min(GRID_W-1,self.target_q+({'left':-1,'right':1}.get(button,0))))
                self.target_r=max(0,min(GRID_H-1,self.target_r+({'up':-1,'down':1}.get(button,0))))
                self.message.set(f'NES A to move to ({self.target_q},{self.target_r}), B to cancel')
                self.redraw()
            elif button=='a':
                q,r=self.target_q,self.target_r;self.targeting=False
                self._issue(self.target_mode,{'q':q,'r':r})
            return
        if button in ('left','right') and self.controller_actions[self.controller_index][0]=='Prisoner Fate':
            options=self._prisoner_options()
            if not options:
                self.message.set('No prisoner awaiting disposition');return
            self.prisoner_decision_index=(self.prisoner_decision_index+(1 if button=='right' else -1))%len(options)
            self.message.set('Fate: '+options[self.prisoner_decision_index].title())
            return
        if button in ('left','right') and self.controller_actions[self.controller_index][0]=='Reinforce':
            self.reinforce_rice=max(0,self.reinforce_rice+(1 if button=='right' else -1))
            self.message.set(f'Reinforcement rice: {self.reinforce_rice}; NES A to confirm')
            return
        if button in ('left','right') and self.controller_actions[self.controller_index][0]=='Withdraw':
            self.withdraw_whole=not self.withdraw_whole
            self.message.set('Withdraw '+('whole army' if self.withdraw_whole else 'selected officer'))
            return
        if button in ('left','right') and self.controller_actions[self.controller_index][0]=='Bribe':
            self.bribe_gold=max(1,min(99,self.bribe_gold+(1 if button=='right' else -1)))
            self.message.set(f'Bribe offer: {self.bribe_gold} gold. NES A to confirm.')
            return
        if button in ('up','down','left','right'):
            self.controller_index=(self.controller_index+{'left':-1,'right':1,'up':-3,'down':3}[button])%len(self.controller_actions)
            self._cursor();self.message.set('NES A: '+self.controller_actions[self.controller_index][0])
        elif button=='a':
            if self.controller_actions[self.controller_index][0]=='Move':return self.start_target()
            if self.controller_actions[self.controller_index][0]=='Move Enemy':
                if not self.selected_target:
                    self.message.set('Select enemy with Next Target first');return
                return self.start_target('battle_move_enemy')
            self.controller_actions[self.controller_index][1]()
    def start_target(self,mode='battle_move'):
        battle=self.parent.c.state.get('battle')
        if not battle:self.message.set('Battle finished');return
        self.targeting=True
        self.target_mode=mode
        u=next((u for u in battle.get('officer_units',[]) if u['id']==self.selected_officer),None)
        self.target_q=(u or battle['a'])['q'];self.target_r=(u or battle['a'])['r']
        self.message.set('NES D-pad chooses destination; A commits '+mode+', B cancels')
        self.redraw()
    def _player_side(self):
        b=self.parent.c.state.get('battle')
        if not b:return 'A'
        return 'D' if self.parent.c.state.get('human_faction')==b['defender'] else 'A'

    def _officer(self):
        b=self.parent.c.state['battle']
        if not b:return None
        pending=b.get('duel_pending')
        owner=(b['defender'] if pending['side']=='A' else b['attacker']) if pending else (b['defender'] if self._player_side()=='D' else b['attacker'])
        src=b['dst'] if owner==b['defender'] else b['src']
        if self.selected_officer:
            selected=self.parent.c.state['core']['officers'].get(self.selected_officer)
            if selected and self.parent.c.state['officer_faction'].get(self.selected_officer)==owner:
                return self.selected_officer
        return next((oid for oid,o in self.parent.c.state['core']['officers'].items()
                     if o['province_id']==src and self.parent.c.state['officer_faction'][oid]==owner),None)
    def _issue(self,kind,params):
        from og.core.fixed import CoreError
        if not self.parent.c.state['battle']:
            self.message.set('Battle already resolved');return
        actor=self._officer()
        if kind in ('battle_move','battle_move_enemy','battle_attack','battle_bribe','battle_duel','battle_wait','battle_withdraw'):
            # A loaded battle may still carry an attacker-selected officer
            # while the human is defending. Never dispatch enemy orders.
            b=self.parent.c.state['battle']
            own=[u['id'] for u in b.get('officer_units',()) if u['side']==self._player_side()
                 and u['troops100']>=100 and not u.get('reserve') and not u.get('fled')]
            if self.selected_officer not in own:
                self.selected_officer=own[0] if own else None
        if kind in ('battle_move','battle_move_enemy','battle_attack','battle_bribe','battle_duel','battle_wait','battle_withdraw') and self.selected_officer:
            params=dict(params,officer_id=self.selected_officer)
        if kind in ('battle_attack','battle_move_enemy','battle_bribe','battle_duel') and self.selected_target:
            params=dict(params)
            params['target_officer_id']=self.selected_target
        if actor is None:
            self.message.set('No eligible battle officer');return
        self.parent.counter+=1
        rid=f'battle-ui:{self.parent.c.state["month"]}:{self.parent.counter}'
        try:
            staged=self.parent.c.clone()
            quote=staged.preview(kind,actor,params,rid)
            result=staged.execute(kind,actor,params,rid,quote['quote_id'],quote['revision'])
            from .integrated import save_file
            save_file(staged,self.parent.path)  # disk commit first; never mutate memory on failed save
            self.parent.c=staged
            self.message.set(str(result)[:200])
            self.parent.audio.effect('victory' if not staged.state.get('battle') else ('battle' if kind=='battle_attack' else 'order'))
        except (CoreError,OSError) as err:
            self.parent.audio.effect('invalid')
            self.message.set('Rejected: '+str(err))
        self.redraw()
        self.parent.redraw()
    def next_officer(self):
        b=self.parent.c.state.get('battle')
        if not b or not b.get('officer_units'):return
        pending=b.get('duel_pending')
        side=('D' if pending['side']=='A' else 'A') if pending else self._player_side()
        owner=b['defender'] if side=='D' else b['attacker']
        choices=[u['id'] for u in b['officer_units'] if u['side']==side and u['troops']>0
                 and not u.get('reserve') and not u.get('fled')
                 and self.parent.c.state['officer_faction'].get(u['id'])==owner]
        if not choices:return
        index=choices.index(self.selected_officer) if self.selected_officer in choices else -1
        self.selected_officer=choices[(index+1)%len(choices)]
        self.message.set('Selected officer '+self.selected_officer)
        self.redraw()
    def next_target(self):
        b=self.parent.c.state.get('battle')
        if not b:return
        enemy='A' if self._player_side()=='D' else 'D'
        choices=[u['id'] for u in b.get('officer_units',[]) if u['side']==enemy and u['troops100']>=100 and not u['fled']]
        if not choices:return
        index=choices.index(self.selected_target) if self.selected_target in choices else -1
        self.selected_target=choices[(index+1)%len(choices)]
        self.message.set('Selected enemy '+self.selected_target)
        self.redraw()
    def _reinforcement_choice(self):
        b=self.parent.c.state.get('battle')
        if not b:return None
        s=self.parent.c.state
        from .army_cohorts import strength
        candidates=[]
        field={u['id'] for u in b.get('officer_units',[])}
        for pid in sorted(s['neighbors'][b['dst']]):
            if s['owners'][pid]!=(b['defender'] if self._player_side()=='D' else b['attacker']):continue
            for oid,off in sorted(s['core']['officers'].items()):
                if (off['province_id']==pid and s['officer_faction'][oid]==(b['defender'] if self._player_side()=='D' else b['attacker'])
                        and oid not in field and strength(s,oid,pid)>0):
                    candidates.append((pid,oid))
        return candidates[self.reinforce_index%len(candidates)] if candidates else None

    def _withdraw_destination(self):
        b=self.parent.c.state.get('battle')
        if not b:return None
        s=self.parent.c.state
        choices=sorted(pid for pid in s['neighbors'][b['dst']]
                       if s['owners'][pid] in (None,(b['defender'] if self._player_side()=='D' else b['attacker'])))
        return choices[self.withdraw_index%len(choices)] if choices else None

    def _prisoner_choice(self):
        b=self.parent.c.state.get('battle')
        if not b or not b.get('pending_outcome'):return None
        choices=[oid for oid in b.get('prisoner_queue',[])
                 if oid not in b.get('prisoner_decisions',{})]
        return choices[self.prisoner_index%len(choices)] if choices else None

    def _prisoner_options(self):
        oid=self._prisoner_choice()
        if oid is None:return ()
        from . import officer_battle_adapter
        return officer_battle_adapter.prisoner_options(self.parent.c.state,
                                                       self.parent.c.state['battle'],oid)

    def prisoner(self):
        oid=self._prisoner_choice()
        options=self._prisoner_options()
        if oid is None or not options:
            self.message.set('No prisoner awaiting disposition');return
        decision=options[self.prisoner_decision_index%len(options)]
        self._issue('battle_prisoner',{'officer_id':oid,'decision':decision})

    def reinforce(self):
        b=self.parent.c.state.get('battle')
        choice=self._reinforcement_choice()
        if not b or choice is None:
            self.message.set('No eligible stationed reinforcement officer');return
        source,oid=choice
        self._issue('battle_reinforce',{'source':source,'officer_ids':[oid],
                    'rice':self.reinforce_rice,'gold':self.reinforce_gold,
                    'arrival_day':min(30,b['day']+1)})

    def withdraw(self):
        dest=self._withdraw_destination()
        if dest is None:
            self.message.set('Cannot flee: no adjacent friendly/open province');return
        self._issue('battle_withdraw',{'destination':dest,
                                     'whole_army':self.withdraw_whole})

    def move_enemy(self):
        # In-window D-pad targeting (same path as the controller); no OS dialog.
        if not self.parent.c.state.get('battle'):return
        if not self.selected_target:
            self.message.set('Select an enemy with Next Target first');return
        self.start_target('battle_move_enemy')
    def wait(self):
        self._issue('battle_wait',{})
    def duel(self):
        from third_branch import officer_battle_adapter as ba
        b=self.parent.c.state.get('battle')
        if not b or b.get('duel_done') or b['day']!=1:
            self.message.set('Duel already resolved or not day one');return
        pending=b.get('duel_pending')
        if pending:
            human_side='D' if pending['side']=='A' else 'A'
            defender=ba.find(b,human_side,self.selected_officer)
            if defender is None:
                defender=ba.find(b,human_side)
            if self.duel_accept and defender is None:
                self.message.set('No eligible duelist; choose Refuse with SELECT');return
            params={'accepted':bool(self.duel_accept)}
            if self.duel_accept:params['target_officer_id']=defender['id']
            self._issue('battle_duel_response',params)
            return
        challenger=ba.find(b,self._player_side(),self.selected_officer)
        if challenger is None:
            self.message.set('Select an officer on your side first');return
        # The CPU chooses its own responding officer.
        target=ba.choose_ai_duel_acceptor(b,challenger)
        self.selected_target=target['id'] if target else None
        self._issue('battle_duel',{'accepted':target is not None,
                                   'target_officer_id':self.selected_target})
    def bribe(self):
        if not self.selected_target:
            self.message.set('Select enemy with Next Target first');return
        self._issue('battle_bribe',{'gold':self.bribe_gold})
    def move(self):
        # In-window D-pad targeting (same path as the controller); no OS dialog.
        if not self.parent.c.state.get('battle'):return
        self.start_target()
    def attack(self):self._issue('battle_attack',{'mode':'normal'})
    def charge(self):self._issue('battle_attack',{'mode':'charge'})
    def simult(self):self._issue('battle_attack',{'mode':'simult'})
    def fire(self):self._issue('battle_attack',{'mode':'fire'})
    def end_turn(self):self._issue('battle_end_turn',{})
    def auto(self):self._issue('battle_auto',{})
    def redraw(self):
        if not self.parent.c.state['battle']:
            self.root.title('Battle resolved - return to strategic map')
            self.canvas.delete('all')
            self.canvas.create_text(220,110,text='Battle settled. Strategic map and ownership updated.',anchor='nw',fill='white')
            return
        img=render_battle(self.parent.c,selected_unit=self.selected_officer or 'a')
        if self.parent.c.state['battle'].get('pending_outcome'):
            self.message.set('PRISONER DECISIONS REQUIRED: select Prisoner Fate, SELECT cycles officers, LEFT/RIGHT changes fate, A confirms')
        self.photo=self.ImageTk.PhotoImage(img)
        self.canvas.delete('all')
        self.canvas.create_image(0,0,anchor='nw',image=self.photo)
        if self.selected_target:
            b=self.parent.c.state.get('battle')
            u=next((u for u in b.get('officer_units',[]) if u['id']==self.selected_target and u['troops100']>=100 and not u['fled']),None)
            if u:
                x,y=X0+u['q']*CELL,Y0+u['r']*CELL
                self.canvas.create_rectangle(x+1,y+1,x+CELL-2,y+CELL-2,outline='#ffed77',width=3)
                self.canvas.create_text(TX,378,anchor='nw',text='TARGET: '+self.selected_target,fill='#ffed77')
        if self.targeting:
            x,y=X0+self.target_q*CELL,Y0+self.target_r*CELL
            self.canvas.create_rectangle(x+1,y+1,x+CELL-2,y+CELL-2,outline='#fff37d',width=4)
    def close(self):
        self.root.destroy()
        if getattr(self.parent,'tactical',None) is self:self.parent.tactical=None  # no stale reference to a destroyed window
        self.parent.audio.play('map',self.parent.selected)
