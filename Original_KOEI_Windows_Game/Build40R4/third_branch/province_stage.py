"""Director-approved province-management stage v1.1 for the LIVE 41-province game.

Separate UI projection, art provider and interactions; there is NO separate province
simulation. All action and persistence calls delegate to the parent StrategicWindow.
The approved concept artwork's illustrative numbers are never inserted into state.
"""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import logging
from PIL import Image, ImageDraw, ImageFont
from .historical_art import ruler_portrait, seasonal_still, event_still_for_committed_record

SEASONS = ('Spring', 'Summer', 'Autumn', 'Winter')
ART = Path(__file__).resolve().parent / 'assets' / 'nes_illustrations' / 'story' / 'story_palace_10x8.png'
PORTRAIT_DIR = Path(__file__).resolve().parent / 'assets' / 'province_ui'
PORTRAIT_ROLES = ('ruler', 'advisor', 'general')


def get_role_portrait(role, path=None):
    """Role-only DEVELOPMENT portrait, never a claim of historical identity.

    Ruler/advisor appointment records are not yet part of this campaign schema.
    General is a neutral officer/speaker fallback, not an officer ID portrait.
    Missing or corrupted assets degrade visibly, without affecting game state.
    """
    if role not in PORTRAIT_ROLES:
        raise ValueError('Unknown portrait role')
    source = Path(path) if path is not None else PORTRAIT_DIR / f'{role}_placeholder.png'
    try:
        with Image.open(source) as im:
            return im.convert('RGB'), str(source), True
    except (OSError, ValueError) as exc:
        logging.getLogger(__name__).warning('Portrait placeholder %s unavailable: %s', role, type(exc).__name__)
        im = Image.new('RGB', (128, 160), (27, 38, 54))
        d = ImageDraw.Draw(im)
        d.rectangle((3, 3, 124, 156), outline=(176, 147, 90), width=3)
        d.ellipse((43, 24, 85, 70), fill=(102, 109, 119))
        d.polygon(((22, 137), (36, 81), (91, 81), (107, 137)), fill=(102, 109, 119))
        d.text((12, 143), f'{role.upper()} UNKNOWN', font=font(10, True), fill=(242, 215, 162))
        return im, 'procedural-neutral-portrait-fallback', True
NAV = ('Domestic', 'Military', 'Troop Assignments', 'Personnel', 'Market',
       'Foreign Affairs', 'Information', 'Advisor', 'Save / Load', 'Options / Help',
       'End Month', 'Back to Strategic Map')
# Only approved, actually implemented engine commands may appear. Old All Orders,
# direct Cultivate and Occupy Vacant commands are not exposed.
CATEGORY_ORDERS = {
    'Domestic': ('Allocate Workers', 'Distribute Food', 'Distribute Goods'),
    'Military': ('Recruit', 'Train Troops', 'Equip Arms', 'Move Army',
                 'Transport', 'Plan Attack', 'Attack'),
    'Personnel': ('Recruit Officer', 'Train Officer', 'Reward Governor',
                  'Reward Officer', 'Buy Horse', 'Appoint Advisor',
                  'Swear Brotherhood', 'Feast', 'Rest', 'Appoint Governor'),
    'Market': ('Market',),
    'Foreign Affairs': ('Ally', 'Joint', 'Marry', 'Give',
                        'Cancel Alliance', 'Threat'),
}
assert len(NAV) == 12

WORKERS = (('farmers','Farmers'), ('town','Town Workers'), ('artisans','Artisans'),
           ('metalworkers','Metalworkers'), ('resources','Resource Workers'),
           ('land','Land Crews'), ('flood','Flood Crews'))
SIZE = (960, 720)


def font(sz=15, bold=False):
    paths = (('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf',
              '/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf') if bold else
             ('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',))
    for p in paths:
        if Path(p).exists():
            return ImageFont.truetype(p,sz)
    return ImageFont.load_default()


def get_province_capital_art(province_id, season, path=None):
    """Every province/season uses the SAME genuine NES palace illustration.

    The historical image does not claim to represent the capital of any specific
    province. Missing art gets a neutral fallback, never a fictitious castle.
    """
    if type(province_id) is not int or not 1 <= province_id <= 41:
        raise ValueError('Province ID must be 1..41')
    if season not in SEASONS:
        raise ValueError('Unknown season')
    source = Path(path) if path is not None else ART
    try:
        with Image.open(source) as im:
            return im.convert('RGB'), str(source), True
    except (OSError, ValueError) as exc:
        logging.getLogger(__name__).warning('Province shared placeholder unavailable: %s', type(exc).__name__)
        im=Image.new('RGB',(360,218),(25,35,52))
        d=ImageDraw.Draw(im)
        d.rectangle((8,8,351,209),outline=(142,123,75),width=3)
        d.text((47,93),'PROVINCE VIEW UNAVAILABLE',font=font(15,True),fill=(220,208,161))
        return im, 'procedural-neutral-fallback', True


@dataclass(frozen=True)
class ProvinceVM:
    id: int
    date_month: int
    season: str
    faction: str
    revision: int
    fingerprint: str
    metrics: tuple
    workers: tuple
    officers: tuple
    feedback: str
    ruler_nes_id: int | None = None
    ruler_name: str | None = None
    event_kind: str | None = None


def snapshot(campaign, province_id, feedback=''):
    """Read-only copy of values backed by live engine fields, not a shadow world."""
    if type(province_id) is not int or not 1 <= province_id <= 41:
        raise ValueError('Province ID must be 1..41')
    s=campaign.state;pid=f'p{province_id}'
    if pid not in s['owners']:raise ValueError('Province is absent from actual campaign')
    p=s['core']['provinces'][pid]
    month=s['month']
    season=SEASONS[((month-1)//3)%4]
    owner=s['owners'][pid] or 'Unoccupied'
    workers=p.get('workers',{})
    # Counts are separate disjoint engine buckets. Masons are NOT double-counted
    # inside Town Workers without a real town-task subassignment ledger.
    # No invented Unassigned Workers row in the Director's historical UI.
    wr=tuple((name, workers.get(key)) for key,name in WORKERS)
    metrics=(
        ('Gold',p.get('treasury_gold_cents')/100 if p.get('treasury_gold_cents') is not None else None),
        ('Food',p.get('food_units')),
        ('Support',p.get('support_ticks')//50000 if p.get('support_ticks') is not None else None),
        ('Wealth',p.get('wealth_milli')/1000 if p.get('wealth_milli') is not None else None),
        ('Cultivation',p.get('cultivation')),
        ('Flood Control',p.get('flood_control')),
        ('Soldiers',p.get('troops')))
    extras=s.get('extras',{}).get(pid,{})
    metrics += tuple((name,extras[key]) for key,name in
                    (('metal','Metal'),('arms','Arms'),('furs','Furs'),
                     ('specialty','Specialty')) if key in extras)
    officers=tuple((oid, s.get('officer_loyalty',{}).get(oid), o.get('bp'),
                    s.get('governors',{}).get(pid,{}).get('officer') == oid)
                   for oid,o in sorted(s['core']['officers'].items()) if o['province_id']==pid)
    ruler_code=s.get('ruler_slots',{}).get(owner)
    ruler_id=int(ruler_code.split(':')[1]) if isinstance(ruler_code,str) and ruler_code.startswith('nes:') else None
    ruler_name=s.get('factions',{}).get(owner,{}).get('name') if ruler_id is not None else None
    # Present only a recorded event for this province. No synthetic monthly events.
    events=s.get('event_feed',[]) or s.get('log',[])[-1:]
    ev=next((x for x in reversed(events) if event_still_for_committed_record(x,province_id) is not None),None)
    return ProvinceVM(province_id,month,season,owner,s['revision'],campaign.fingerprint,
                      metrics,wr,officers,str(feedback),ruler_id,ruler_name,ev.get('kind') if ev else None)


def fmt(v):
    if v is None:return 'Not tracked'
    if type(v) is float:return f'{v:,.2f}'.rstrip('0').rstrip('.')
    return f'{v:,}' if type(v) is int else str(v)


def panel(d, box, title):
    x0,y0,x1,y1=box
    d.rectangle(box,fill=(27,41,61),outline=(218,184,112),width=2)
    d.text((x0+10,y0+8),title,font=font(15,True),fill=(243,214,147))


def render_province(vm:ProvinceVM, nav_index=0, message='', art_path=None, nav_choices=None, view_only=False):
    """960x720 logical 4:3, pixel-sharp and fills a 1024x768 (4:3) window."""
    im=Image.new('RGB',SIZE,(15,28,45))
    d=ImageDraw.Draw(im)
    panel(d,(6,6,168,710),'PROVINCE')
    d.text((17,38),f'P{vm.id:02d} | {vm.faction}',font=font(14,True),fill=(245,231,184))
    d.text((17,62),f'Month {vm.date_month:03d}',font=font(15),fill=(210,216,225))
    d.text((17,84),vm.season,font=font(14),fill=(210,216,225))
    d.line((12,108,164,108),fill=(145,119,68),width=2)
    d.text((16,118),'COMMANDS' if nav_choices is None or view_only else 'CHOOSE ORDER',font=font(13,True),fill=(245,220,154))
    choices=(('Exit',) if view_only else NAV) if nav_choices is None else nav_choices
    for i,label in enumerate(choices):
        y=141+i*(44 if nav_choices is None else 34)
        if i==nav_index:d.rectangle((11,y-3,163,y+28),fill=(65,83,110),outline=(244,214,124))
        size=11;lf=font(size,True if nav_index==i else False)
        while d.textlength(label,font=lf)>146 and size>7:
            size-=1;lf=font(size,True if nav_index==i else False)
        d.text((17,y+3),label,font=lf,fill=(245,231,177) if i==nav_index else (229,234,242))
    d.text((16,682),'Select: Exit' if view_only else 'Select: Map',font=font(12),fill=(230,231,239))

    panel(d,(176,6,624,264),'PROVINCE CAPITAL')
    art, provenance, _=get_province_capital_art(vm.id,vm.season,art_path)
    art=art.resize((428,211),Image.Resampling.NEAREST)
    im.paste(art,(186,40))
    panel(d,(632,6,954,264),'RULER                  ADVISOR')
    # Historical ruler is selected by the actual faction's original NES officer ID.
    # Advisor remains explicitly unappointed rather than inventing an identity.
    for x,role in ((649,'ruler'), (800,'advisor')):
        historical=ruler_portrait(vm.ruler_nes_id) if role=='ruler' else None
        portrait=historical if historical is not None else get_role_portrait(role)[0]
        im.paste(portrait.resize((128,152),Image.Resampling.NEAREST),(x+2,48))
        d.rectangle((x,48,x+132,201),outline=(147,130,89),width=2)
        tag=(vm.ruler_name or '') if historical is not None else 'Unassigned'
        d.text((x+3,212),tag[:17],font=font(13),fill=(219,209,181))
    # Director-approved gameplay surface: no officer roster, no phantom facilities,
    # and no unassigned-worker row. Named officers appear in command forms ONLY.
    panel(d,(176,272,624,514),'WORKFORCE ALLOCATIONS')
    for j,(name,value) in enumerate(vm.workers):
        y=310+j*26
        d.text((191,y),name,font=font(13),fill=(237,238,240))
        d.text((479,y),fmt(value),font=font(13,True),fill=(247,219,150))
    panel(d,(632,272,954,710),'PROVINCE RESOURCES')
    metrics=dict(vm.metrics)
    keys=('Gold','Food','Support','Wealth','Cultivation',
          'Flood Control','Soldiers','Metal','Arms','Furs','Specialty')
    for j,key in enumerate(k for k in keys if k in metrics):
        y=300+j*28
        d.text((644,y),key,font=font(13),fill=(232,233,239))
        d.text((818,y),fmt(metrics[key]),font=font(13,True),fill=(248,221,158))
    panel(d,(176,522,363,710),'SEASON / SPEAKER')
    # Only a real recorded speaker ID can trigger the portrait fallback.
    if vm.feedback.startswith('OFFICER SPEAKING:'):
        speaker_art, _, _ = get_role_portrait('general')
        im.paste(speaker_art.resize((153,107),Image.Resampling.NEAREST),(192,560))
        d.rectangle((192,560,345,667),outline=(158,139,91),width=2)
        d.text((190,680),vm.feedback[len('OFFICER SPEAKING:'):][:19],font=font(11),fill=(223,214,175))
    else:
        key=({'locust_plague':'event_01_locust_plague','typhoon':'event_02_typhoon',
              'flood':'event_03_flood','epidemic':'event_04_epidemic',
              'riot':'event_05_riot','rebellion':'event_06_rebellion',
              'falling_star':'event_07_falling_star_evil_omen',
              'ruler_died':'event_08_ruler_died_funeral',
              'secret_letter_delivered':'event_09_secret_letter_delivered',
              'messenger_caught_secret_letter':'event_10_messenger_caught_with_secret_letter',
              'messenger_caught_gold_rice':'event_11_caught_transporting_gold_rice'}).get(vm.event_kind)
        event=event_still_for_committed_record({'kind':vm.event_kind}) if key else None
        season_image=seasonal_still(vm.season)
        shown=event if event is not None else season_image
        if shown is None:shown=art
        im.paste(shown.resize((163,108),Image.Resampling.NEAREST),(188,559))
        d.text((186,678),(f'EVENT: {vm.event_kind}' if event is not None else f'{vm.season} season')[:25],
               font=font(11),fill=(218,216,175))
    panel(d,(371,522,624,710),'MESSAGE / REPORT')
    mf=font(12);words=(message or vm.feedback or 'Select a command').split();lines=[];cur=''
    for w in words:  # word-wrap inside the 230px panel instead of hard-cutting at 29 characters
        trial=(cur+' '+w).strip()
        if d.textlength(trial,font=mf)<=230:cur=trial
        else:
            if cur:lines.append(cur)
            cur=w
    if cur:lines.append(cur)
    if len(lines)>2:
        lines=lines[:2];lines[1]=lines[1][:max(0,len(lines[1])-1)]+'…'
    for n,line in enumerate(lines):d.text((386,550+n*16),line,font=mf,fill=(227,221,192))
    hints=(('A / B / Select: exit to map','Start: Save / Load menu') if view_only else
           ('A: execute selected order','D-pad: menu','Select: China map; B: back','Start: Save / Load menu'))
    for n,line in enumerate(hints):d.text((386,590+n*24),line,font=font(12),fill=(203,218,231))
    return im


class ProvinceStage:
    """Real Tk layer attached to the current StrategicWindow campaign reference."""
    def __init__(self, parent):
        self.parent=parent
        self.tk=parent.tk
        self.ImageTk=parent.ImageTk
        self.index=0
        self.submenu=None
        self.message='Province information is read from the live campaign.'
        self.speaker=None
        self.troop_dialog=None
        self.root=self.tk.Toplevel(parent.root)
        self.root.title('KOEI Enhanced — Province')
        self.root.geometry('1024x768')
        self.root.resizable(False,False)  # fixed 4:3
        self.canvas=self.tk.Canvas(self.root,bg='#030a11',highlightthickness=0)
        self.canvas.pack(fill='both',expand=True)
        self.canvas.bind('<Configure>',lambda _:self.redraw())
        self.canvas.bind('<Button-1>',self._on_click)
        self.root.protocol('WM_DELETE_WINDOW',self.close)
        self.vm=None
        self.redraw()
        self.root.focus_force()
    def active(self):
        return self.root.winfo_exists()
    @property
    def view_only(self):
        """A province the player does not control is only viewed; its single command is Exit."""
        return self.parent.c.state['owners'][f'p{self.parent.selected}']!=self.parent.player
    def exit_view(self):
        self.root.withdraw()
        return self.parent.return_to_map_from_view()
    def _fitted(self):
        width=max(1,self.canvas.winfo_width());height=max(1,self.canvas.winfo_height())
        scale=min(width/960,height/720)
        sw,sh=round(960*scale),round(720*scale)
        return (int((width-sw)//2),int((height-sh)//2),sw,sh)
    def redraw(self):
        if not self.active():return
        self.vm=snapshot(self.parent.c,self.parent.selected,
                         ('OFFICER SPEAKING:'+self.speaker) if self.speaker else '')
        view=self.view_only
        if view:self.submenu=None;self.index=0
        img=render_province(self.vm,self.index,('Not your province. Exit returns to the map.' if view else self.message),
                            nav_choices=self.submenu,view_only=view)
        x,y,w,h=self._fitted()
        self.photo=self.ImageTk.PhotoImage(img.resize((w,h),Image.Resampling.NEAREST))
        self.canvas.delete('all')
        self.canvas.create_image(x,y,anchor='nw',image=self.photo)
    def _on_click(self,event):
        x,y,w,h=self._fitted()
        lx=(event.x-x)*960/w;ly=(event.y-y)*720/h
        choices=('Exit',) if self.view_only else (self.submenu if self.submenu is not None else NAV)
        stride=34 if self.submenu is not None else 44
        if 12<=lx<=168 and 141<=ly<141+stride*len(choices):
            self.index=int((ly-141)//stride)
            self.choose()
    def on_nes_button(self,button):
        if self.troop_dialog is not None and self.troop_dialog.active():
            return self.troop_dialog.on_nes_button(button)
        if self.view_only:
            if button in ('a','b','select'):return self.exit_view()
            if button=='start':return self.parent.pause()
            return None  # a viewed province offers exactly one command: Exit
        if button=='select':return self.parent.show_china_map()
        if button=='b':
            if self.submenu is not None:
                self.submenu=None;self.index=0;self.redraw();return
            if self.speaker:
                self.speaker=None;self.redraw();return
            self.message='Province is the default screen. Select opens the China map.'
            return self.redraw()
        if button=='start':return self.parent.pause()
        if button in ('up','down'):
            self.index=(self.index+(-1 if button=='up' else 1))%len(self.submenu if self.submenu is not None else NAV)
            self.redraw()
        elif button=='a':return self.choose()  # provinces are chosen on the China map only
    def choose(self):
        from .orders_ui import SPECS
        if self.view_only:return self.exit_view()
        if self.submenu is not None:
            chosen=self.submenu[self.index]
            if chosen not in SPECS:
                self.message='Command is not implemented; no order issued.'
                self.redraw();return
            # All commands in a category are accessible, not a hard-coded default.
            self.parent.orders(allowed_orders=self.submenu, initial_order=chosen)
            return
        name=NAV[self.index]
        if name=='Back to Strategic Map':return self.parent.show_china_map()
        if name in ('Save / Load','Options / Help'):
            return self.parent.pause() if name=='Save / Load' else self.parent.show_context_help()
        if name=='End Month':
            self.parent.month()
            return self.redraw()
        if name=='Troop Assignments':
            from .troop_assignments_ui import TroopAssignmentsWindow
            if self.troop_dialog is not None and self.troop_dialog.active():
                self.troop_dialog.root.lift()
            else:
                self.troop_dialog=TroopAssignmentsWindow(self)
            return
        if name=='Information':return self.parent.journal()
        if name=='Advisor':return self.parent.advisor()
        if name in CATEGORY_ORDERS:
            self.submenu=CATEGORY_ORDERS[name]
            # Each visible menu item must have a true executable engine adapter.
            if any(command not in SPECS for command in self.submenu):
                self.message='This category has an unimplemented command.'
            else:self.message=name+': select a command; B returns.'
            self.index=0;self.redraw();return
        raise AssertionError('Unhandled approved menu category: '+name)
    def close(self):
        if self.troop_dialog is not None and self.troop_dialog.active():
            self.troop_dialog.close()
        if self.active():self.root.destroy()
        if getattr(self.parent,'province_stage',None) is self:self.parent.province_stage=None
        if self.parent.root.winfo_exists():self.parent.root.after_idle(self.parent.open_province)
