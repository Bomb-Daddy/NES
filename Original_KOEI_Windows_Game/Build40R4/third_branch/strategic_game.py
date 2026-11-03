"""Playable 41-province STRATEGIC VIEW in the original-game integrated engine.

Run: PYTHONPATH=. python -m third_branch.strategic_game
Requires Pillow and Tkinter (both supported on Linux/Windows PCs).
This is an INTEGRATION SLICE, not final game, final emblems, or NES geography.
"""
from __future__ import annotations
import argparse, json, os
from collections import Counter
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
from og.core.fixed import CoreError
from .integrated import new_campaign_41, save_file, load_file
from .historical_s1 import new_campaign_historical_s1
from .ai import take_turn
from .xbox_controller import NesInputRouter, MAPPING_LABEL
from .input_bindings import InputBindings, BindingError
from .context_help import help_for
from .audio_system import GameAudio

ASSETS = Path(__file__).resolve().parent / 'assets' / 'strategic'
SLOTS = json.loads((ASSETS / 'province_slots_41.json').read_text())['province_slots']
COLORS = {'A':(209,76,71,255),'B':(76,123,212,255),'C':(74,179,112,255)}
SEASONS = ('spring','summer','autumn','winter')
W,H=1024,768  # 4:3 (CRT) window, Director ruling 2026-11-02
# Director ruling 2026-11-02: no sidebar. The China map is rescaled (4:3, art aspect kept),
# framed by a border, and a one-line button help strip runs across the bottom.
STRIP_H=48;MARGIN=8;BORDER=6
MAP_DH=H-STRIP_H-2*MARGIN-2*BORDER
MAP_DW=round(MAP_DH*4/3)
MAP_W=MAP_DW
MAP_X=(W-MAP_DW)//2
MAP_Y=MARGIN+BORDER
FRAME_COL=(114,107,76,255)
HINT_DEFAULT='Start: Menu     Y: Help'

def province_at_pixel(x,y):
    """Visible emblem targets only, not inferred concept-art polygons."""
    best=None
    r=18*MAP_DW/960
    for id_,v in SLOTS.items():
        px,py,pw,ph=v['rect_px']
        sx=MAP_X+(px+pw/2)*(MAP_DW/1448)
        sy=MAP_Y+(py+ph/2)*(MAP_DH/1086)
        dist=(x-sx)**2+(y-sy)**2
        if dist<=r**2 and (best is None or dist<best[0]):best=(dist,int(id_))
    return best[1] if best else None

def render(campaign, selected=1, season_override=None, hint=None):
    if sorted(int(k) for k in SLOTS)!=list(range(1,42)):
        raise ValueError('Strategic art missing some province slots')
    season=SEASONS[((campaign.state['month']-1)//3)%4] if season_override is None else season_override
    if season not in SEASONS:raise ValueError('Invalid season')
    bg=Image.open(ASSETS / f'{season}_clean_41.png').convert('RGBA')
    overlay=Image.new('RGBA',bg.size,(0,0,0,0))
    draw=ImageDraw.Draw(overlay)
    for i in range(1,42):
        owner=campaign.state['owners'][f'p{i}']
        if owner is None:continue  # genuine alpha-zero, no baked slot rectangle
        x,y,w,h=SLOTS[str(i)]['rect_px']
        col=COLORS.get(owner,((64+37*int(owner[1:]))%190+30,(75+53*int(owner[1:]))%170+40,(68+71*int(owner[1:]))%150+65,255) if owner.startswith('R') else (170,143,102,255))
        draw.rectangle((x,y,x+w-1,y+h-1),fill=col,outline=(15,15,15,255),width=2)
        draw.polygon(((x+6,y+6),(x+w-7,y+6),(x+w//2,y+h-7)),fill=(245,233,197,255))
    bg=Image.alpha_composite(bg,overlay)
    left=bg.resize((MAP_DW,MAP_DH),Image.Resampling.NEAREST)
    frame=Image.new('RGBA',(W,H),(14,23,35,255))
    d=ImageDraw.Draw(frame)
    d.rectangle((MAP_X-BORDER,MAP_Y-BORDER,MAP_X+MAP_DW+BORDER-1,MAP_Y+MAP_DH+BORDER-1),fill=FRAME_COL)
    d.rectangle((MAP_X-2,MAP_Y-2,MAP_X+MAP_DW+1,MAP_Y+MAP_DH+1),fill=(14,23,35,255))
    frame.alpha_composite(left,(MAP_X,MAP_Y))
    d=ImageDraw.Draw(frame)
    # Selection marker: drawn for every province (vacant ones have no emblem), thick and high-contrast
    # so the map picker always shows which province is highlighted.
    if selected in range(1,42):
        x,y,w,h=SLOTS[str(selected)]['rect_px'];fx=MAP_DW/bg.width;fy=MAP_DH/bg.height
        cx,cy=MAP_X+(x+w/2)*fx,MAP_Y+(y+h/2)*fy;r=max(w*fx,h*fy)/2+9
        d.rectangle((cx-r-2,cy-r-2,cx+r+2,cy+r+2),outline=(20,20,20,255),width=3)
        d.rectangle((cx-r,cy-r,cx+r,cy+r),outline=(255,235,60,255),width=4)
    sy=H-STRIP_H
    d.line((0,sy,W-1,sy),fill=FRAME_COL,width=2)
    d.text((24,sy+(STRIP_H-10)//2),hint if hint else HINT_DEFAULT,fill=(237,237,231,255))
    return frame.convert('RGB')

def readable_month_summary(outcomes, month):
    """Bounded, player-facing summary of actually executed rival orders."""
    if not isinstance(outcomes,(list,tuple)):
        return f'Month {month:03d} complete. Open Journal for campaign details.'
    counts=Counter(str(item.get('kind','unknown')).replace('_',' ') for item in outcomes
                   if isinstance(item,dict))
    acted=sum(n for k,n in counts.items() if k not in ('idle','inactive','delegated idle','no officer'))
    common=counts.most_common(2)
    detail=', '.join(f'{n} {kind}' for kind,n in common) if common else 'no recorded orders'
    return f'Month {month:03d} complete. {acted} rival orders; {detail}. Open Journal for details.'


def readable_journal_entry(entry):
    """Present saved orders in readable terms, not raw nested Python/JSON data."""
    if not isinstance(entry,dict):
        return 'Campaign note recorded.'
    kind=str(entry.get('kind',entry.get('event','order'))).replace('_',' ').title()
    month=entry.get('month')
    when=f'Month {month:03d}' if isinstance(month,int) else 'Campaign'
    actor=entry.get('ai_name') or entry.get('faction') or entry.get('actor') or 'World'
    actor=str(actor).replace('nes:','Officer #')
    parameters=entry.get('params') or {}
    result=entry.get('result') or {}
    if not isinstance(parameters,dict):parameters={}
    if not isinstance(result,dict):result={}
    info=[]
    province=parameters.get('province_id') or entry.get('province') or result.get('province_id')
    if province is not None:info.append('Province '+str(province).lstrip('p'))
    for key,label in (('count','troops'),('units','units'),('target','target'),('commodity','commodity')):
        value=parameters.get(key)
        if isinstance(value,(str,int,float)):
            info.append(f'{label}: {value}')
    if not parameters:
        for key,label in (('troops_after','troops remaining'),('bp_after','BP remaining'),('outcome','outcome')):
            value=result.get(key)
            if isinstance(value,(str,int,float)):
                info.append(f'{label}: {value}')
    reason=entry.get('reason') or (result.get('reason') if isinstance(result,dict) else None)
    if reason and isinstance(reason,str):info.append('Note: '+reason[:90])
    details=' · '.join(str(v) for v in info[:3])
    return f'{when}  |  {actor}  |  {kind}'+('\n'+details if details else '')


class StrategicWindow:
    def __init__(self, save_path):
        import tkinter as tk
        from PIL import ImageTk
        self.tk=tk
        self.ImageTk=ImageTk
        from .platform_paths import prepare_save_path
        self.path=prepare_save_path(save_path)
        self.new_game_pending=not self.path.exists()
        self.save_notice=''
        if not self.new_game_pending:
            try:self.c=load_file(self.path)
            except Exception as exc:
                # A save from an older build (or a damaged file) must never stop the game from starting.
                # Keep it (renamed, not deleted) and begin a new game.
                import time
                aside=self.path.with_name(self.path.name+'.incompatible-'+time.strftime('%Y%m%d-%H%M%S'))
                try:self.path.replace(aside)
                except OSError:pass
                self.new_game_pending=True
                self.save_notice=f'Your old save could not be loaded ({type(exc).__name__}) and was kept as {aside.name}. Starting a new game. '
        if self.new_game_pending:self.c=new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
        self.audio=GameAudio()
        initial='R0' if self.c.state.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED' else 'A'
        self.player=self.c.state.get('human_faction',initial)
        self.bindings=InputBindings.load()
        if self.player not in self.c.state['factions']:
            self.player=next(iter(self.c.state['factions']))
        saved_province=self.c.state.get('human_selected_province')
        self.selected=(int(saved_province[1:]) if not self.new_game_pending and isinstance(saved_province,str)
                       and saved_province in self.c.state['owners']
                       and self.c.state['owners'][saved_province]==self.player
                       else next((int(p[1:]) for p,f in self.c.state['owners'].items() if f==self.player),1))
        if self.new_game_pending:self.selected=1  # neutral initial highlight, never a preselected historical ruler
        self.counter=0
        self.root=tk.Tk();self.root.title('KOEI Enhanced — 41 Provinces')
        self.root.geometry(f'{W}x{H}')
        self.root.resizable(False,False)
        self.canvas=tk.Canvas(self.root,width=W,height=H,highlightthickness=0)
        self.canvas.pack()
        # The China map selects provinces in setup, target and manual-view modes; it never issues orders.
        self.canvas.bind('<Button-1>',self.on_click)
        self.root.bind_all('<KeyPress>',self._keyboard_action,add='+')
        # The China map constructs ZERO buttons, order widgets or command handlers.
        # A province-targeting context may select an ID, but never dispatch an order.
        self.action_items=[];self.action_buttons=[]
        self.command_index=0;self.controller_focus='map'
        self.pause_dialog=None;self.province_stage=None;self.event_dialog=None
        self.info_dialog=None;self.info_text=None;self.help_dialog=None
        self.map_mode='setup' if self.new_game_pending else None
        self.map_target_dialog=None
        self.map_target_field=None
        self.map_return_selected=None
        self.msg=tk.StringVar(value=self.save_notice+'Select a ruler and province. View: focus commands | Y: Help | A: choose | B: back. '+self.audio.status())
        self.message_widget=tk.Label(self.root,textvariable=self.msg,anchor='nw',justify='left',wraplength=285,
                 fg='#f4d5a5',bg='#0e1723')
        # Hide the old command/status strip from the map, including message hints.
        self.redraw()
        self.audio.play('map',self.selected)
        self.gamepad=NesInputRouter(self.on_nes_button,bindings=self.bindings,raw_receiver=self._raw_pad_button)
        self._gamepad_after=self.root.after(75,self._poll_gamepad)
        self.root.protocol('WM_DELETE_WINDOW',self.close)
        # Never silently create an R0/P9 campaign on a fresh save.
        # Existing saves resume directly into the Province UI.
        self.root.after_idle(self._start_new_game_setup if self.new_game_pending else self.open_province)
    def _start_new_game_setup(self):
        self.msg.set(self.save_notice+'NEW GAME: Select a province on China map (D-pad or click, A confirm). Vacant provinces cannot start a historical ruler.')
        self.redraw();self.root.lift();self.root.focus_force()

    def _adopt_loaded(self,loaded):
        """Restore human identity from the exact same authenticated campaign save."""
        initial='R0' if loaded.state.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED' else 'A'
        player=loaded.state.get('human_faction',initial)
        if player not in loaded.state['factions']:
            raise ValueError('Saved human faction is not in the historical roster')
        self.c=loaded;self.player=player;self.new_game_pending=False;self.map_mode=None
        if 'human_faction' not in loaded.state and hasattr(self,'msg'):
            self.msg.set('Legacy save has no ruler selection. Default '+player+'; choose ruler and save to confirm identity.')
        saved=loaded.state.get('human_selected_province')
        self.selected=(int(saved[1:]) if isinstance(saved,str) and saved in loaded.state['owners']
                       and loaded.state['owners'][saved]==player else
                       next((int(pid[1:]) for pid,owner in loaded.state['owners'].items() if owner==player),1))
        self.redraw()

    def _raw_pad_button(self,physical):
        pause=getattr(self,'pause_dialog',None)
        if pause is None or pause.mode!='pad_capture':return False
        if physical=='b':
            pause.status.set('Controller capture cancelled; B always remains available.')
        else:
            try:
                displaced=self.bindings.remap('pad',pause.binding_action,physical)
                self.bindings.save()
                pause.status.set('Saved controller '+pause.binding_action+' = '+physical+(f' (swapped {displaced})' if displaced else ''))
            except (BindingError,OSError) as exc:pause.status.set('Binding rejected: '+str(exc))
        pause.mode='pad_bindings';pause.refresh()
        return True

    def _keyboard_action(self,event):
        pause=getattr(self,'pause_dialog',None)
        key=event.keysym
        if pause is not None and pause.mode=='key_capture':
            if key=='Escape':pause.status.set('Keyboard capture cancelled; Escape always remains Back.')
            else:
                try:
                    displaced=self.bindings.remap('keyboard',pause.binding_action,key)
                    self.bindings.save()
                    pause.status.set('Saved keyboard '+pause.binding_action+' = '+key+(f' (swapped {displaced})' if displaced else ''))
                except (BindingError,OSError) as exc:pause.status.set('Binding rejected: '+str(exc))
            pause.mode='key_bindings';pause.refresh()
            return 'break'
        action=self.bindings.translate('keyboard',key)
        if action=='help':self.on_nes_button('help');return 'break'
        if getattr(event.widget,'winfo_class',lambda:'' )() in ('Entry','TEntry','TCombobox','Text','Listbox') and action not in ('b','start'):
            return None  # Do not turn editing or native listbox navigation into orders; B/Menu still recover.
        if action:
            self.on_nes_button(action)
            return 'break'
        return None

    def show_context_help(self):
        import tkinter as tk
        if self.help_dialog is not None and self.help_dialog.winfo_exists():
            self.help_dialog.destroy();self.help_dialog=None;return
        topic='Map';surface='strategic';field=None
        if getattr(self,'orders_dialog',None) is not None and self.orders_dialog.root.winfo_exists():
            window=self.orders_dialog
            if hasattr(window,'order'):
                topic=window.order.get();surface='orders'
                items=window._controller_items()
                index=min(window.controller_index,len(items)-1)
                if items[index][0] not in ('order','confirm','close'):field=items[index][0]
        elif (self.province_stage is not None and self.province_stage.active()
              and self.province_stage.troop_dialog is not None
              and self.province_stage.troop_dialog.active()):
            troop=self.province_stage.troop_dialog
            topic=(troop.fields[troop.index][0] if troop.index<len(troop.fields)
                   else 'Preview / Commit')
            surface='troops'
        elif self.province_stage is not None and self.province_stage.active():
            from .province_stage import NAV
            stage=self.province_stage
            if stage.submenu is not None:
                topic=stage.submenu[stage.index];surface='orders'
            else:
                topic=NAV[stage.index];surface='province'
        elif getattr(self,'tactical',None) is not None and self.tactical.root.winfo_exists():
            window=self.tactical
            topic=window.controller_actions[window.controller_index][0]
            surface='battle'
        elif self.pause_dialog is not None and self.pause_dialog.root.winfo_exists():
            entries=self.pause_dialog.entries();topic=entries[self.pause_dialog.index] if entries else 'Settings'
            surface='menu'
        elif self.map_mode=='target':topic='Province target selection'
        title,body=help_for(topic,surface,field)
        win=tk.Toplevel(self.root);win.title('HELP — '+title);win.geometry(self._centered(640,480))
        win.transient(self.root);win.configure(bg='#111e32')
        tk.Label(win,text=title,font=('Arial',15,'bold'),bg='#111e32',fg='#f5db99').pack(pady=12)
        tk.Label(win,text=body,font=('Arial',11),bg='#111e32',fg='#e4eaf4',wraplength=570,justify='left').pack(fill='both',expand=True,padx=20)
        hint=f'Pad {self.bindings.pad["help"].upper()} / Key {self.bindings.keyboard["help"]}: Help   A: Continue   B: Back'
        tk.Label(win,text=hint,bg='#111e32',fg='#f5db99').pack(pady=10)
        win.bind('<Escape>',lambda e:(self.show_context_help(),'break')[-1])
        win.protocol('WM_DELETE_WINDOW',self.show_context_help)
        self.help_dialog=win;win.focus_force()

    def _poll_gamepad(self):
        if not self.root.winfo_exists():return
        self.gamepad.poll()
        self._gamepad_after=self.root.after(75,self._poll_gamepad)
    def close(self):
        if getattr(self,'_gamepad_after',None) is not None:
            try:self.root.after_cancel(self._gamepad_after)
            except self.tk.TclError:pass
            self._gamepad_after=None
        if getattr(self,'province_stage',None) is not None and self.province_stage.active():
            self.province_stage.root.destroy()
            self.province_stage=None
        if getattr(self,'event_dialog',None) is not None:
            try:self.event_dialog.destroy()
            except Exception:pass
        self.gamepad.close()
        self.audio.close()
        self.root.destroy()
    def _action_cursor(self):
        return None  # Map has no action controls to select or highlight.
    def on_nes_button(self,button):
        # Modal ruler confirmation takes priority over the setup map.
        if button=='help':return self.show_context_help()
        if getattr(self,'help_dialog',None) is not None and self.help_dialog.winfo_exists():
            if button in ('a','b','start'):return self.show_context_help()
            return
        if getattr(self,'ruler_dialog',None) is not None and self.ruler_dialog.winfo_exists():
            if button=='b':self.ruler_dialog.destroy();return
            if button=='a':self.ruler_confirm();return
            if button in ('up','down'):
                box=self.ruler_listbox;n=box.size()
                old=box.curselection();i=((old[0] if old else 0)+(-1 if button=='up' else 1))%n
                box.selection_clear(0,'end');box.selection_set(i);box.activate(i);box.see(i)
            return
        if self.map_mode is not None:
            if self.map_mode=='setup':
                if button in ('up','down','left','right'):
                    return self.select({'left':-1,'right':1,'up':-10,'down':10}[button])
                if button=='a':return self._finish_new_game_setup()
                if button in ('b','select'):
                    self.msg.set('Choose an occupied province for a historical ruler. No campaign has been saved.')
                return  # no orders, turn advance, or save during first-run setup
            if self.map_mode=='target':
                if button in ('b','select'):return self.return_to_province()
                if button in ('up','down','left','right'):
                    self.select({'left':-1,'right':1,'up':-10,'down':10}[button])
                elif button=='a':return self._finish_map_target()
            elif self.map_mode=='manual':
                if button in ('up','down','left','right'):
                    self.select({'left':-1,'right':1,'up':-10,'down':10}[button])
                elif button=='a':return self.view_selected_province()
                elif button in ('b','select'):return self.return_to_province()
            return
        """NES A confirms, B cancels, Start pauses, Select switches focus."""
        if getattr(self,'info_dialog',None) is not None and self.info_dialog.winfo_exists():
            if button in ('a','b','start'):
                self.info_dialog.destroy();self.info_dialog=None;self.info_text=None
            elif button in ('up','down') and self.info_text is not None:
                self.info_text.yview_scroll(-3 if button=='up' else 3, 'units')
            return
        if getattr(self,'event_dialog',None) is not None and self.event_dialog.winfo_exists():
            if button in ('a','b','start'):self._close_event_dialog()
            return
        if self.pause_dialog is not None and self.pause_dialog.root.winfo_exists():
            return self.pause_dialog.on_nes_button(button)
        if getattr(self,'tactical',None) is not None and self.tactical.root.winfo_exists():
            return self.tactical.on_nes_button(button)
        if getattr(self,'orders_dialog',None) is not None and self.orders_dialog.root.winfo_exists():
            return self.orders_dialog.on_nes_button(button)
        if self.province_stage is not None and self.province_stage.active():
            return self.province_stage.on_nes_button(button)
        if button=='start':self.audio.effect('confirm');return self.pause()
        if button in ('b','select'):return self.return_to_province()
        return None  # No map command cursor, ever.
    def confirm_dialog(self,title,text):
        """In-game 4:3 Yes/No dialog (replaces the OS message box). Returns True/False."""
        import tkinter as tk
        win=tk.Toplevel(self.root);win.title(title);win.geometry(self._centered(640,480))
        win.transient(self.root);win.configure(bg='#111e32');win.resizable(False,False)
        out={'v':False}
        tk.Label(win,text=title,font=('Arial',15,'bold'),bg='#111e32',fg='#f5db99').pack(pady=12)
        tk.Label(win,text=text,bg='#111e32',fg='#e8eef8',wraplength=580,justify='left').pack(padx=20,pady=8,fill='both',expand=True)
        def done(v):
            out['v']=v;win.destroy()
        row=tk.Frame(win,bg='#111e32');row.pack(pady=14)
        yes=tk.Button(row,text='Yes (A / Enter)',width=16,command=lambda:done(True));yes.pack(side='left',padx=12)
        tk.Button(row,text='No (B / Esc)',width=16,command=lambda:done(False)).pack(side='left',padx=12)
        win.bind('<Return>',lambda _e:done(True));win.bind('<Escape>',lambda _e:done(False))
        win.protocol('WM_DELETE_WINDOW',lambda:done(False))
        self.confirm_win=win;yes.focus_set();win.grab_set();win.wait_window()
        self.confirm_win=None
        return out['v']
    def _centered(self,w,h):
        """4:3 dialog geometry centred over the main window."""
        x=self.root.winfo_rootx()+max(0,(self.root.winfo_width()-w)//2);y=self.root.winfo_rooty()+max(0,(self.root.winfo_height()-h)//2)
        return f'{w}x{h}+{x}+{y}'
    def pause(self):
        self.pause_dialog=NesPauseMenu(self)
    def on_click(self,e):
        # Map is selectable only in first-run setup or a live order-target form.
        if self.map_mode not in ('target','setup','manual'):return None
        province=province_at_pixel(e.x,e.y)
        if province is not None:
            again=(province==self.selected)
            self.selected=province
            self.redraw()
            if self.map_mode=='manual' and again:return self.view_selected_province()  # click the highlighted province to open it
        return None
    def select(self,delta):
        self.selected=1+(self.selected-1+delta)%41;self.redraw();self.audio.play('map',self.selected)
    def map_hint(self):
        if self.map_mode=='setup':return 'D-pad / Click: highlight province     A: choose     Y: Help'
        if self.map_mode=='target':return 'D-pad / Click: highlight province     A: choose target     B / Select: cancel     Y: Help'
        if self.map_mode=='manual':return 'D-pad / Click: highlight province     A: view province     B / Select: back     Y: Help'
        return HINT_DEFAULT
    def redraw(self):
        img=render(self.c,self.selected,hint=self.map_hint())
        self.photo=self.ImageTk.PhotoImage(img)
        self.canvas.delete('all');self.canvas.create_image(0,0,anchor='nw',image=self.photo)
        stage=getattr(self,'province_stage',None)
        if stage is not None and stage.active():stage.redraw()
    def show_china_map(self, mode='manual', target_dialog=None, target_field=None):
        """No buttons: manual read-only view or explicit order target picker."""
        if self.map_mode is not None:return
        if mode not in ('manual','target'):raise ValueError('Invalid map state')
        if mode=='target' and (target_dialog is None or target_field not in ('target','objective')):
            raise ValueError('No supported order target field')
        self.map_mode=mode
        self.map_return_selected=self.selected
        self.map_target_dialog=target_dialog if mode=='target' else None
        self.map_target_field=target_field if mode=='target' else None
        stage=getattr(self,'province_stage',None)
        if stage is not None and stage.active():stage.root.withdraw()
        if target_dialog is not None and target_dialog.root.winfo_exists():target_dialog.root.withdraw()
        self.redraw()
        self.root.lift();self.root.focus_force()

    def view_selected_province(self):
        """Map -> highlighted province, shown in the standard province UI (own: full commands; other: Exit only)."""
        if self.map_mode!='manual':return
        self.map_mode=None
        owner=self.c.state['owners'][f'p{self.selected}']
        if owner==self.player:
            self.map_return_selected=self.selected  # own province: normal play continues from here
        stage=getattr(self,'province_stage',None)
        if stage is not None and stage.active():
            stage.index=0;stage.submenu=None
            stage.root.deiconify();stage.redraw();stage.root.lift();stage.root.focus_force()
        else:self.open_province()
        self.map_return_selected=self.map_return_selected if owner!=self.player else None
        return True

    def return_to_map_from_view(self):
        """Exit from a viewed province goes back to the map with that province still highlighted."""
        self.map_mode='manual'
        self.redraw();self.root.lift();self.root.focus_force()

    def _finish_new_game_setup(self):
        if self.map_mode!='setup' or not self.new_game_pending:return
        province='p'+str(self.selected)
        owner=self.c.state['owners'][province]
        if owner is None:
            self.msg.set('Province '+str(self.selected)+' is vacant; select a historical ruler-owned province. No changes saved.')
            self.audio.effect('invalid');return False
        self.choose_ruler(setup_province=self.selected)
        return True

    def _finish_map_target(self):
        if self.map_mode!='target':return
        dialog=self.map_target_dialog
        field=self.map_target_field
        if dialog is None or not dialog.root.winfo_exists():return self.return_to_province()
        province='p'+str(self.selected)
        selected_value=province
        if dialog.order.get() in ('Ally','Joint','Marry','Give','Cancel Alliance','Threat') and field=='target':
            selected_value=self.c.state['owners'][province]
            if selected_value is None or selected_value==self.player:
                dialog.info.set('Select a province controlled by another ruler.')
                self.redraw();return
        if field not in dialog.fields:
            dialog.info.set('Target field no longer exists.')
            return self.return_to_province()
        dialog.fields[field].set(selected_value)
        dialog.pending=None  # No target silently commits an order.
        dialog.info.set('Selected '+province+' on map. Preview and confirm in the order form.')
        self.return_to_province()

    def return_to_province(self):
        self.map_mode=None
        if self.map_return_selected is not None:self.selected=self.map_return_selected
        self.map_return_selected=None
        dialog=self.map_target_dialog
        self.map_target_dialog=None;self.map_target_field=None
        stage=getattr(self,'province_stage',None)
        if stage is not None and stage.active():
            stage.root.deiconify();stage.redraw();stage.root.lift();stage.root.focus_force()
        else:self.open_province()
        if dialog is not None and dialog.root.winfo_exists():
            dialog.root.deiconify();dialog.root.lift();dialog.root.focus_force()

    def open_province(self):
        from .province_stage import ProvinceStage
        if self.province_stage is not None and self.province_stage.active():
            self.province_stage.root.deiconify()
            self.province_stage.redraw()
            self.province_stage.root.lift()
            return
        self.province_stage=ProvinceStage(self)

    def actor(self):
        pid=f'p{self.selected}'
        officer=next((oid for oid,o in self.c.state['core']['officers'].items()
                      if o['province_id']==pid and self.c.state['officer_faction'][oid]==self.c.state['owners'][pid]),None)
        if officer is None:raise CoreError('NO_OFFICER','No eligible officer in this province')
        if self.c.state['owners'][pid]!=self.player:
            raise CoreError('ENEMY_CONTROLLED','You may inspect enemy provinces, but not command their officers')
        if self.c.state['officer_faction'][officer]!=self.player:
            raise CoreError('ENEMY_CONTROLLED','Officer does not belong to your faction')
        return officer
    def issue(self,kind,params,actor=None,confirm=False):
        self.counter+=1;rid=f'ui:{self.c.state["month"]}:{self.counter}:{os.getpid()}'
        try:
            if self.c.state['owners'][f'p{self.selected}']!=self.player:
                raise CoreError('ENEMY_CONTROLLED','Cannot command an opponent province')
            chosen=self.actor() if actor is None else actor
            o=self.c.state['core']['officers'].get(chosen)
            if kind=='reward_governor':
                from .officer_relations import ruler
                # A ruler-wide personnel order can be opened while viewing any
                # owned province; it uses the ruler's actual home treasury/BP.
                if (not o or chosen!=ruler(self.c.state,self.player)
                    or self.c.state['officer_faction'][chosen]!=self.player
                    or self.c.state['owners'].get(o['province_id'])!=self.player):
                    raise CoreError('BAD_ACTOR','Only the ruler may reward a governor')
            elif not o or o['province_id']!=f'p{self.selected}' or self.c.state['officer_faction'][chosen]!=self.player:
                raise CoreError('BAD_ACTOR','Actor must be an officer of the player in the selected province')
            quote=self.c.preview(kind,chosen,params,rid)
            if confirm:
                if not self.confirm_dialog('Confirm live order',
                      f'{kind}: {params}\n\nPREVIEW: {quote["result"]}\n\nCommit this game-state change?'):
                    self.audio.effect('cancel')
                    self.msg.set('Order cancelled without changes')
                    return False
            staged=self.c.clone()
            result=staged.execute(kind,chosen,params,rid,quote['quote_id'],quote['revision'])
            save_file(staged,self.path)  # Disk commit BEFORE activating staged state
            old_event_count=len(self.c.state.get('event_history',[]))
            self.c=staged;self.autosave()
            new_events=self.c.state.get('event_history',[])[old_event_count:]
            self.audio.effect('trade' if kind=='market' else 'order')
            if kind in ('Ally','Joint','Marry','Give','Cancel','Threat'):
                self.audio.play('marriage' if kind=='Marry' else 'diplomacy',loop=False)
            self.msg.set(kind+' applied: '+str(result)[:165])
            stage=getattr(self,'province_stage',None)
            if stage is not None and stage.active():stage.speaker=chosen
            if self.c.state.get('battle') and kind=='attack':
                self.battle()
            self.redraw()
            self._show_committed_events(new_events)
            return True
        except (CoreError,OSError,ValueError) as e:
            self.audio.effect('invalid')
            self.msg.set('Action/Save error: '+str(e))
            self.redraw()
            return False
    def family_info(self):
        import tkinter as tk
        from .historical_s1 import officer_family
        p='p'+str(self.selected)
        eligible=[key for key,rec in sorted(self.c.state['core']['officers'].items())
                  if rec['province_id']==p]
        win=tk.Toplevel(self.root);win.title('Officer family relationships')
        win.geometry('640x480')
        widget=tk.Text(win,wrap='word');widget.pack(fill='both',expand=True)
        for key in eligible:
            if key not in self.c.state.get('officer_catalog',{}):continue
            data=officer_family(self.c.state,key)
            widget.insert('end',data['name']+' ('+key+')\n')
            widget.insert('end','ROM FAMILY CODE: '+str(data['rom_family_mask_candidate'])+'\n')
            widget.insert('end','SAME GROUP: '+', '.join(z['name'] for z in data['shared_group'])+'\n')
            for e in data['curated_relations']:
                a=self.c.state['officer_catalog'][e['from_id']]['name']
                b=self.c.state['officer_catalog'][e['to_id']]['name']
                widget.insert('end',f'  {a} -> {b}: {e["type"]} (curated candidate)\n')
            widget.insert('end','\n')
        if not eligible:widget.insert('end','No active officers currently stationed here.')
        widget.configure(state='disabled')
        self.info_dialog=win;self.info_text=widget
        win.bind('<Escape>',lambda e:win.destroy())
    def choose_ruler(self, setup_province=None):
        import tkinter as tk
        if setup_province is not None and (not self.new_game_pending or self.map_mode!='setup'):
            raise ValueError('Historical ruler setup is only available during a fresh game')
        if getattr(self,'ruler_dialog',None) is not None and self.ruler_dialog.winfo_exists():return
        win=tk.Toplevel(self.root);win.title('Choose historical Scenario 1 ruler')
        win.geometry('700x525')
        choices=sorted(self.c.state['factions'],key=lambda k:(int(k[1:]) if k.startswith('R') and k[1:].isdigit() else k))
        names=[f'{f}: {self.c.state["factions"][f]["name"]}' for f in choices]
        tk.Label(win,text=('Province '+str(setup_province)+' — select its historical ruler' if setup_province is not None
                           else 'S1 RULERS — select then confirm')).pack()
        status=tk.StringVar(value=('Only the ruler owning this province is eligible. Custom ruler creation is not yet implemented.'
                                   if setup_province is not None else 'Choose a historical ruler.'))
        tk.Label(win,textvariable=status,wraplength=385,fg='#8b2900').pack()
        box=tk.Listbox(win,height=18,exportselection=False);box.pack(fill='both',expand=True)
        for name in names:box.insert('end',name)
        owner=self.c.state['owners'][f'p{setup_province}'] if setup_province is not None else self.player
        index=choices.index(owner);box.selection_set(index);box.activate(index);box.see(index)
        def confirm():
            row=box.curselection()
            if not row:return False
            chosen=choices[row[0]]
            if setup_province is not None and self.c.state['owners'][f'p{setup_province}']!=chosen:
                status.set('This ruler does not control Province '+str(setup_province)+'. Select its owner or choose another province.')
                return False
            staged=self.c.clone()
            staged.state['human_faction']=chosen
            selected_province=(f'p{setup_province}' if setup_province is not None else
                               next((pid for pid,f in staged.state['owners'].items() if f==chosen),None))
            staged.state['human_selected_province']=selected_province
            try:
                if setup_province is not None and self.path.exists():
                    raise OSError('A save was created while setup was open; refusing to overwrite it')
                save_file(staged,self.path)
            except (OSError,CoreError,ValueError) as error:
                status.set('Ruler choice could not be saved: '+str(error));return False
            self.c=staged;self.player=chosen;self.selected=int(selected_province[1:])
            self.new_game_pending=False;self.map_mode=None
            self.msg.set('Playing as '+self.c.state['factions'][self.player]['name']+' in Province '+str(self.selected))
            self.audio.play('map',self.selected);self.audio.effect('confirm')
            win.destroy();self.redraw();self.open_province()
            return True
        tk.Button(win,text='Play as selected ruler',command=confirm).pack(pady=4)
        box.bind('<Return>',lambda e:confirm())
        win.protocol('WM_DELETE_WINDOW',win.destroy)
        self.ruler_dialog=win;self.ruler_listbox=box;self.ruler_confirm=confirm
        box.focus_set()
    def feed(self):self.issue('distribute_food',{'province_id':f'p{self.selected}','units':10})
    def recruit(self):self.issue('recruit',{'province_id':f'p{self.selected}','count':10})
    def trade(self):self.issue('market',{'commodity':'food','side':'buy','units':5})
    def attack(self):
        # Use the controller-operable Orders UI for both mouse and gamepad paths.
        # tkinter.simpledialog.askinteger required a keyboard on Windows.
        self.orders()
        if hasattr(self.orders_dialog, 'order'):
            self.orders_dialog.order.set('Attack')
            self.orders_dialog._fields()
    def battle(self):
        if not self.c.state['battle']:
            self.msg.set('No battle active');return
        from .tactical_view import TacticalWindow
        self.audio.play('battle');self.audio.effect('battle')
        self.tactical=TacticalWindow(self)
    AUTOSAVES=3
    def autosave_path(self,n):
        return self.path.with_name(self.path.stem+f'_auto{n}'+self.path.suffix)
    def autosave(self):
        """Rolling autosaves, separate from the five manual slots: auto1 is newest, auto3 oldest. Never raises."""
        try:
            for n in range(self.AUTOSAVES,1,-1):
                if self.autosave_path(n-1).exists():os.replace(self.autosave_path(n-1),self.autosave_path(n))
            save_file(self.c,self.autosave_path(1))
        except OSError:pass
    def month(self):
        from .player_runtime import stage_month_with_opponents
        stage=getattr(self,'province_stage',None)
        self.map_mode='turn'
        if stage is not None and stage.active():stage.root.withdraw()
        self.redraw();self.root.lift();self.root.update()  # Show map before synchronous AI simulation.
        new_events=[]
        try:
            staged,result=stage_month_with_opponents(self.c,player=self.player)
            save_file(staged,self.path)  # Do not advance in memory if disk commit fails
            old_event_count=len(self.c.state.get('event_history',[]))
            self.c=staged;self.autosave()
            new_events=self.c.state.get('event_history',[])[old_event_count:]
            self.last_opponent_actions=result['opponents']
            self.msg.set(readable_month_summary(result['opponents'], self.c.state['month']))
            self.audio.play('map',self.selected);self.audio.effect('month')
        except (CoreError,OSError,ValueError) as e:
            self.audio.effect('invalid')
            self.msg.set('Month/Save error: '+str(e))
        finally:
            # Even failed turns must return to province screen without extra input.
            self.redraw();self.return_to_province()
        self._show_committed_events(new_events)
    def _show_committed_events(self, events):
        """Present actual saved events, using actual asset lookup, never random UI images."""
        from .historical_art import event_still_for_committed_record
        self._pending_events=getattr(self,'_pending_events',[])
        self._pending_events.extend(e for e in events if event_still_for_committed_record(e) is not None)
        if self.event_dialog is None or not self.event_dialog.winfo_exists():
            self._next_event_dialog()

    def _close_event_dialog(self):
        if self.event_dialog is not None:
            try:self.event_dialog.destroy()
            except Exception:pass
            self.event_dialog=None
        self.audio.play('map',self.selected)
        self._next_event_dialog()

    def _next_event_dialog(self):
        import tkinter as tk
        from .historical_art import event_still_for_committed_record
        self._pending_events=getattr(self,'_pending_events',[])
        if not self._pending_events:return
        event=self._pending_events.pop(0)
        art=event_still_for_committed_record(event)
        if art is None:return self._next_event_dialog()
        win=tk.Toplevel(self.root);self.event_dialog=win
        win.title('Campaign Event — '+event['kind'].replace('_',' ').title())
        win.geometry(self._centered(580,435));win.transient(self.root);win.configure(background='#101a2c')
        self._event_photo=self.ImageTk.PhotoImage(art.resize((336,240),Image.Resampling.NEAREST))
        tk.Label(win,image=self._event_photo,background='#101a2c').pack(pady=8)
        event_text=(f"{event['kind'].replace('_',' ').title()} | Province {event['province_id']} | "
                    f"Month {event['month']}\n{event['result'].get('text','')}\n"
                    f"Effects: {event['result'].get('effects',{})}")
        tk.Label(win,text=event_text,fg='#e8d3a0',bg='#101a2c',wraplength=490,justify='center').pack()
        tk.Button(win,text='Continue (A / B)',command=self._close_event_dialog).pack(pady=4)
        win.bind('<Return>',lambda e:self._close_event_dialog())
        win.bind('<Escape>',lambda e:self._close_event_dialog())
        win.protocol('WM_DELETE_WINDOW',self._close_event_dialog)
        self.audio.effect('dialog')
        if event['kind']=='ruler_died':self.audio.play('death',loop=False)
        elif event['kind'].startswith('messenger_') or event['kind']=='secret_letter_delivered':
            self.audio.play('diplomacy',loop=False)
        win.lift();win.update_idletasks();win.focus_force()

    def orders(self, allowed_orders=None, initial_order=None):
        from .orders_ui import OrdersWindow
        try:self.orders_dialog=OrdersWindow(self,allowed_orders=allowed_orders,initial_order=initial_order)
        except ValueError as error:
            self.msg.set(str(error));self.audio.effect('invalid')
    def advisor(self):
        import tkinter as tk
        info=self.c.advice(self.player)
        win=tk.Toplevel(self.root);win.title('Advisor — actual saved game metrics')
        win.geometry('840x630')
        widget=tk.Text(win,wrap='word',width=100,height=38)
        widget.pack(fill='both',expand=True)
        widget.insert('end',f'ADVISOR | FACTION {self.player} | MONTH {info["month"]} | {info["observed_months"]} monthly snapshots\n\n')
        for province in info['advice']:
            widget.insert('end',str(province)+'\n')
        widget.insert('end','\nSIX-MONTH CHANGE FROM SAVED HISTORY:\n')
        for delta in info['six_month_review']:widget.insert('end',str(delta)+'\n')
        if not info['six_month_review']:widget.insert('end','Not enough month history yet\n')
        widget.configure(state='disabled')
        self.info_dialog=win;self.info_text=widget
    def journal(self):
        import tkinter as tk
        win=tk.Toplevel(self.root);win.title('Campaign Journal — readable orders')
        win.geometry(self._centered(960,720))
        win.configure(bg='#101e2c')
        tk.Label(win,text='CAMPAIGN JOURNAL',font=('DejaVu Sans',18,'bold'),
                 bg='#101e2c',fg='#f2ce80',pady=10).pack(fill='x')
        frame=tk.Frame(win,bg='#101e2c');frame.pack(fill='both',expand=True,padx=16,pady=(0,14))
        bar=tk.Scrollbar(frame,orient='vertical')
        bar.pack(side='right',fill='y')
        widget=tk.Text(frame,wrap='word',font=('DejaVu Sans',12),
                       fg='#f5e6c8',bg='#142737',insertbackground='#f5e6c8',
                       borderwidth=1,relief='flat',padx=14,pady=12,
                       spacing1=2,spacing3=9,yscrollcommand=bar.set)
        widget.pack(side='left',fill='both',expand=True)
        bar.config(command=widget.yview)
        widget.tag_configure('heading',font=('DejaVu Sans',13,'bold'),foreground='#f2ce80',spacing1=7,spacing3=9)
        widget.insert('end','LATEST RIVAL TURN ORDERS\n','heading')
        rivals=getattr(self,'last_opponent_actions',None)
        if rivals:
            for entry in rivals:widget.insert('end',readable_journal_entry(entry)+'\n')
        else:widget.insert('end','No rival orders recorded this session.\n')
        widget.insert('end','\nRECENT CAMPAIGN HISTORY\n','heading')
        recent=self.c.state.get('log',[])[-100:]
        if recent:
            for entry in reversed(recent):widget.insert('end',readable_journal_entry(entry)+'\n')
        else:widget.insert('end','No committed actions yet.\n')
        widget.configure(state='disabled')
        self.info_dialog=win;self.info_text=widget
    def save(self):
        try:
            save_file(self.c,self.path);self.audio.effect('save');self.msg.set(f'Saved {self.path}: {self.c.fingerprint}')
        except OSError as e:self.msg.set('Save failed: '+str(e))
    def load(self):
        try:self._adopt_loaded(load_file(self.path));self.msg.set(f'Cold-loaded: {self.c.fingerprint} — {self.player}')
        except (OSError,CoreError) as e:self.msg.set('Load rejected: '+str(e))
        self.redraw();self.audio.play('map',self.selected);self.audio.effect('load')
    def run(self):self.root.mainloop()

class NesPauseMenu:
    """Controller-accessible save management, with multiple manual slots."""
    def __init__(self,parent):
        tk=parent.tk;self.parent=parent;self.index=0;self.slot=1;self.mode='main'
        self.root=tk.Toplevel(parent.root);self.root.title('NES START — GAME MENU');self.library_page=0
        self.root.geometry('870x653');self.root.transient(parent.root)
        self.heading=tk.Label(self.root,text='NES START MENU',font=('Arial',16,'bold'))
        self.heading.pack(pady=12)
        self.status=tk.StringVar(value='D-pad: select | A: confirm | B: back')
        tk.Label(self.root,textvariable=self.status,wraplength=440).pack(pady=8)
        self.buttons=[]
        for i in range(12):
            b=tk.Button(self.root,text='',command=lambda n=i:self.activate(n),width=42)
            b.pack(pady=2);self.buttons.append(b)
        self.root.protocol('WM_DELETE_WINDOW',self.close)
        self.refresh()
    def slot_path(self,n):
        return self.parent.path.with_name(self.parent.path.stem+f'_slot{n}'+self.parent.path.suffix)
    def entries(self):
        if self.mode=='main':return ['Resume','Save Game','Load Game','Continue Last Save','Return to Main Menu','Exit Game',
                                    'Music: '+('Arranged' if self.parent.audio.mode=='arranged' else 'Off'),
                                    'Sound Effects: '+('On' if self.parent.audio.sfx_enabled else 'Off'),
                                    'Audio Status','Soundtrack Library (15 tracks)', 'Controller Bindings', 'Keyboard Bindings']
        if self.mode in ('pad_bindings','key_bindings'):
            device='pad' if self.mode=='pad_bindings' else 'keyboard'
            return [f'{action.title()}: {self.parent.bindings.binding(device,action)}' for action in self.parent.bindings.pad]+['Restore Defaults','Back']
        if self.mode in ('pad_capture','key_capture'):
            return ['Press new physical button/key now (B or Escape cancels)']
        if self.mode=='library':
            from .audio_system import TRACKS
            nums=range(1,9) if self.library_page==0 else range(9,16)
            labels=[f'{n:02d} '+(TRACKS[n].stem[4:] if n in TRACKS else 'MISSING') for n in nums]
            return labels+(['Next Page'] if self.library_page==0 else ['Previous Page','Back'])
        if self.mode in ('save','load'):
            rows=[f'Slot {n}: '+('Occupied' if self.slot_path(n).exists() else 'Empty') for n in range(1,6)]
            if self.mode=='load':   # rolling autosaves are loadable, never manually writable
                rows+=[f'Autosave {n}: '+('Occupied' if self.parent.autosave_path(n).exists() else 'Empty') for n in range(1,self.parent.AUTOSAVES+1)]
            return rows+['Back']
        return ['Confirm','Cancel']
    def refresh(self):
        labels=self.entries();self.index%=len(labels)
        for i,b in enumerate(self.buttons):
            b.config(text=labels[i] if i<len(labels) else '',state='normal' if i<len(labels) else 'disabled',relief='sunken' if i==self.index else 'raised')
        self.heading.config(text='NES START — '+self.mode.upper())
    def activate(self,n):
        self.index=n;self.choose()
    def choose(self):
        if self.mode=='main':
            if self.index==0:return self.close()
            if self.index in (1,2):self.mode='save' if self.index==1 else 'load';self.index=0
            elif self.index==3:
                return self.load_path(self.parent.path)
            elif self.index==4:self.mode='return_confirm';self.index=1
            elif self.index==5:self.mode='exit_confirm';self.index=1
            elif self.index==6:
                mode='off' if self.parent.audio.mode=='arranged' else 'arranged'
                played=self.parent.audio.mode_set(mode,self.parent.selected)
                self.status.set(self.parent.audio.status() if played else self.parent.audio.error or self.parent.audio.status())
            elif self.index==7:
                enabled=self.parent.audio.effects_set(not self.parent.audio.sfx_enabled)
                self.status.set('Sound effects '+('enabled' if enabled else 'muted'))
                self.parent.audio.effect('confirm')
            elif self.index==8:self.status.set(self.parent.audio.status())
            elif self.index==9:self.mode='library';self.library_page=0;self.index=0
            elif self.index in (10,11):self.mode='pad_bindings' if self.index==10 else 'key_bindings';self.index=0
        elif self.mode in ('pad_bindings','key_bindings'):
            device='pad' if self.mode=='pad_bindings' else 'keyboard'
            if self.index<9:
                self.binding_action=list(self.parent.bindings.pad)[self.index]
                self.mode='pad_capture' if device=='pad' else 'key_capture'
                self.status.set('Press a NEW '+device+' control for '+self.binding_action+'; B/Escape cancels')
            elif self.index==9:
                self.parent.bindings.reset(device);self.parent.bindings.save()
                self.status.set(device+' restored to safe defaults')
            else:self.mode='main';self.index=10 if device=='pad' else 11
        elif self.mode in ('pad_capture','key_capture'):
            return  # physical input capture owns the next event
        elif self.mode=='library':
            size=8 if self.library_page==0 else 7
            if self.index<size:
                n=1+self.index+8*self.library_page
                self.parent.audio.mode_set('arranged')
                ok=self.parent.audio.play('track:'+str(n))
                self.status.set(f'Track {n:02d}: '+('playing' if ok else self.parent.audio.error or 'unavailable'))
            elif self.library_page==0:self.library_page=1;self.index=0
            elif self.index==7:self.library_page=0;self.index=0
            else:self.mode='main';self.index=9;self.parent.audio.play('map',self.parent.selected)
        elif self.mode in ('save','load'):
            back=5 if self.mode=='save' else 5+self.parent.AUTOSAVES
            if self.index==back:self.mode='main';self.index=0
            elif self.index>=5:   # load mode only: Autosave 1..3
                return self.load_path(self.parent.autosave_path(self.index-4))
            else:
                self.slot=self.index+1
                if self.mode=='save' and self.slot_path(self.slot).exists():self.mode='overwrite_confirm';self.index=1
                elif self.mode=='save':return self.save_path(self.slot_path(self.slot))
                else:return self.load_path(self.slot_path(self.slot))
        elif self.index==1:self.mode='save' if self.mode=='overwrite_confirm' else 'main';self.index=0
        elif self.mode=='overwrite_confirm':return self.save_path(self.slot_path(self.slot))
        elif self.mode in ('exit_confirm','return_confirm'):
            self.close();return self.parent.close()
        self.refresh()
    def save_path(self,path):
        try:
            save_file(self.parent.c,path)
            self.parent.audio.effect('save')
            self.status.set('Saved '+path.name+' — '+self.parent.c.fingerprint)
            self.mode='main';self.index=0
        except (OSError,CoreError) as e:self.status.set('Save failed: '+str(e))
        self.refresh()
    def load_path(self,path):
        try:
            loaded=load_file(path)  # Validate before mutating live state
            if getattr(self.parent,'tactical',None) is not None and self.parent.tactical.root.winfo_exists():
                self.parent.tactical.root.destroy();self.parent.tactical=None
            self.parent._adopt_loaded(loaded);self.parent.audio.play('map',self.parent.selected);self.parent.audio.effect('load')
            self.status.set('Loaded '+path.name+' — '+loaded.fingerprint)
            self.mode='main';self.index=0
        except (OSError,CoreError,ValueError) as e:self.status.set('Load failed: '+str(e))
        self.refresh()
    def on_nes_button(self,button):
        if self.mode in ('pad_capture','key_capture'):return
        if button=='b':
            if self.mode=='main':return self.close()
            self.mode='main';self.index=0;self.parent.audio.play('map',self.parent.selected);self.refresh();return
        if button=='start' and self.mode=='main':return self.close()
        if button in ('up','left'):self.index=(self.index-1)%len(self.entries());self.refresh()
        elif button in ('down','right'):self.index=(self.index+1)%len(self.entries());self.refresh()
        elif button=='a':self.choose()
    def close(self):
        self.parent.audio.effect('cancel')
        self.parent.audio.play('map',self.parent.selected)
        self.root.destroy()

def enable_windows_dpi_awareness(platform=None):
    # On high-DPI Windows displays, Tk's nominal window pixels must match
    # our map's pointer hitboxes, not a 150%-scaled bitmap.
    import sys
    platform=sys.platform if platform is None else platform
    if platform!='win32':return False
    try:
        import ctypes
        # per-monitor DPI awareness for Tk can yield unstable geometry; use system aware.
        try:return ctypes.windll.shcore.SetProcessDpiAwareness(1) in (0, 0x80070005)
        except (AttributeError,OSError):return bool(ctypes.windll.user32.SetProcessDPIAware())
    except (AttributeError,OSError):return False

def main():
    enable_windows_dpi_awareness()
    from .platform_paths import default_save_path, prepare_save_path
    ap=argparse.ArgumentParser();ap.add_argument('--save',default=None)
    ap.add_argument('--smoke',action='store_true');ap.add_argument('--screenshot',default='41_province_spring.png')
    args=ap.parse_args()
    # Use per-user writable saves on Windows/Linux; older CWD saves are recognized.
    if args.save is None:args.save=str(default_save_path())
    prepare_save_path(args.save)
    if args.smoke:
        c=new_campaign_41();h=c.fingerprint
        c.advance_month();save_file(c,args.save);c2=load_file(args.save)
        assert c.fingerprint==c2.fingerprint and c.fingerprint!=h
        render(c2,41).save(args.screenshot)
        print(json.dumps({'status':'PASS','provinces':len(c2.state['owners']),
                          'graph_edges':sum(map(len,c2.state['neighbors'].values()))//2,
                          'from':h,'after':c2.fingerprint,'season':'spring',
                          'image':args.screenshot,'save':args.save},sort_keys=True))
    else:StrategicWindow(args.save).run()

if __name__=='__main__':main()
