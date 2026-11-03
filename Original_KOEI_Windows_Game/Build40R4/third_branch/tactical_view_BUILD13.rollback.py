"""State-driven tactical battle presentation for the original-game campaign.

Reads a real Campaign.battle (terrain, position, troops, day, owner) and renders
separate terrain and living-army layers. In 41-province scenarios the actual
seasonal 624x576 per-province PNG is rendered under dynamic live army overlays.
Original source offset remains partially unverified in province 41 (two cells).
Legacy seven-province saves retain the synthetic fallback for compatibility.
"""
from __future__ import annotations
from PIL import Image, ImageDraw, ImageFont

WIDTH, HEIGHT = 1280, 720
GRID_W, GRID_H = 13, 12
CELL = 42
X0, Y0 = 70, 94
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
    """Returns a full 1280x720 rendered battle from LIVE serialized state."""
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
    d.text((70,39),f'BATTLE  {battle["src"].upper()} > {battle["dst"].upper()}  DAY {battle["day"]}',font=HEADING_FONT, fill=(243,220,142))
    d.text((685,95),f'ATTACKER {battle["attacker"]}',font=UI_FONT, fill=(246,179,144))
    d.text((685,125),f'Troops {battle["a"]["troops"]}  Morale {battle["a"]["morale"]}',font=UI_FONT, fill=(237,238,228))
    d.text((685,175),f'DEFENDER {battle["defender"]}',font=UI_FONT, fill=(161,205,255))
    d.text((685,205),f'Troops {battle["d"]["troops"]}  Morale {battle["d"]["morale"]}',font=UI_FONT, fill=(237,238,228))
    d.text((685,263),f'Province {battle["dst"][1:]}  Season {((campaign.state["month"]-1)//3)%4+1}/4  Weather {battle.get("weather","?")}',font=UI_FONT, fill=(235,215,166))
    d.text((685,297),'Move / Normal / Charge / Simult. / Fire',font=UI_FONT, fill=(218,224,226))
    if battle.get('officer_units'):
        selected=selected_unit if selected_unit not in ('a','d') else battle.get('selected_officer')
        u=next((u for u in battle['officer_units'] if u['id']==selected),None)
        if u:
            d.text((685,378),f'Officer: {selected}   Troops {u["troops"]}   Mobility {u["mp"]}',font=UI_FONT,fill=(247,234,173))
    if real_asset:
        d.text((685,325),'41 x 4 SEASONAL MAP: '+season.upper(),font=UI_FONT, fill=(225,207,142))
        d.text((685,348),'P41 last two cells inferred (not ROM-proof)',font=UI_FONT, fill=(225,182,132))
    else:
        d.text((685,325),'Legacy 7-province provisional grid',font=UI_FONT, fill=(225,182,132))
    d.text((70,636),'LIVE ARMY MARKERS: troop count / commander; zero-strength armies vanish.',font=UI_FONT, fill=(236,235,210))
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
        self.root.geometry('1280x720')
        self.root.resizable(False,False)
        self.canvas=tk.Canvas(self.root,width=1280,height=720,highlightthickness=0)
        self.canvas.pack()
        self.message=tk.StringVar(value='Battle commands change the real campaign ledger.')
        tk.Label(self.root,textvariable=self.message,bg='#12202c',fg='#fce4b9',wraplength=450,justify='left').place(x=690,y=400,width=450,height=65)
        commands=[('Move',self.move),('Normal',self.attack),('Charge',self.charge),('Simult.',self.simult),('Fire',self.fire),('End Turn',self.end_turn),('Auto Resolve',self.auto),('Close',self.close)]
        commands.insert(1,('Next Officer',self.next_officer))
        self.controller_actions=commands;self.controller_index=0;self.targeting=False
        self.selected_officer=parent.c.state.get('battle',{}).get('selected_officer')
        self.target_q=0;self.target_r=0
        bar=tk.Frame(self.root)
        bar.place(x=690,y=475)
        self.action_buttons=[]
        for n,(name,action) in enumerate(commands):
            btn=tk.Button(bar,text=name,command=action);btn.grid(row=n//2,column=n%2)
            self.action_buttons.append(btn)
        self._cursor()
        self.redraw()
    def _cursor(self):
        for idx,button in enumerate(self.action_buttons):
            button.config(relief='sunken' if idx==self.controller_index else 'raised')
    def on_nes_button(self,button):
        if button=='start':return self.parent.pause()
        if button=='select':
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
                self._issue('battle_move',{'q':q,'r':r})
            return
        if button in ('up','down','left','right'):
            self.controller_index=(self.controller_index+{'left':-1,'right':1,'up':-2,'down':2}[button])%len(self.controller_actions)
            self._cursor();self.message.set('NES A: '+self.controller_actions[self.controller_index][0])
        elif button=='a':
            if self.controller_index==0:return self.start_target()
            self.controller_actions[self.controller_index][1]()
    def start_target(self):
        battle=self.parent.c.state.get('battle')
        if not battle:self.message.set('Battle finished');return
        self.targeting=True
        u=next((u for u in battle.get('officer_units',[]) if u['id']==self.selected_officer),None)
        self.target_q=(u or battle['a'])['q'];self.target_r=(u or battle['a'])['r']
        self.message.set('NES D-pad chooses target; A commits move, B cancels')
        self.redraw()
    def _officer(self):
        b=self.parent.c.state['battle']
        if not b:return None
        src=b['src'];owner=b['attacker']
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
        if kind in ('battle_move','battle_attack') and self.selected_officer:
            params=dict(params,officer_id=self.selected_officer)
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
        choices=[u['id'] for u in b['officer_units'] if u['side']=='A' and u['troops']>0
                 and self.parent.c.state['officer_faction'].get(u['id'])==b['attacker']]
        if not choices:return
        index=choices.index(self.selected_officer) if self.selected_officer in choices else -1
        self.selected_officer=choices[(index+1)%len(choices)]
        self.message.set('Selected officer '+self.selected_officer)
        self.redraw()
    def move(self):
        from tkinter import simpledialog
        if not self.parent.c.state['battle']:return
        b=self.parent.c.state['battle']
        u=next((x for x in b.get('officer_units',[]) if x['id']==self.selected_officer),b['a'])
        q=simpledialog.askinteger('Army move','Destination column 0-12',parent=self.root,initialvalue=u['q'],minvalue=0,maxvalue=12)
        if q is None:return
        r=simpledialog.askinteger('Army move','Destination row 0-11',parent=self.root,initialvalue=u['r'],minvalue=0,maxvalue=11)
        if r is not None:self._issue('battle_move',{'q':q,'r':r})
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
        self.photo=self.ImageTk.PhotoImage(img)
        self.canvas.delete('all')
        self.canvas.create_image(0,0,anchor='nw',image=self.photo)
        if self.targeting:
            x,y=X0+self.target_q*CELL,Y0+self.target_r*CELL
            self.canvas.create_rectangle(x+1,y+1,x+CELL-2,y+CELL-2,outline='#fff37d',width=4)
    def close(self):
        self.root.destroy()
        self.parent.audio.play('map',self.parent.selected)
