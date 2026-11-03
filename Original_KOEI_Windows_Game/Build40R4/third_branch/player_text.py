"""Read-only human-readable projection of authoritative Original game data.
Never modifies a campaign or claims provisional genealogy as canonical.
"""
from __future__ import annotations
from collections import Counter

def province_name(state, province):
    if province is None:return 'Unknown province'
    code=str(province)
    if code.startswith('p') and code[1:].isdigit():return 'Province '+str(int(code[1:]))
    if code.isdigit():return 'Province '+str(int(code))
    return 'Province (unidentified)'

def faction_name(state, fid):
    if fid is None:return 'Unoccupied'
    rec=state.get('factions',{}).get(fid)
    return str(rec.get('name','Unidentified faction')) if isinstance(rec,dict) else 'Unidentified faction'

def officer_name(state, oid):
    rec=state.get('officer_catalog',{}).get(oid)
    if isinstance(rec,dict) and rec.get('name'):return str(rec['name'])
    rec=state.get('core',{}).get('officers',{}).get(oid)
    return str(rec.get('name')) if isinstance(rec,dict) and rec.get('name') else 'Unidentified officer'

def describe_effects(state, effects):
    if not isinstance(effects,dict) or not effects:return 'No additional effects recorded.'
    label={'support_ticks':'Public support','unassigned_workers_lost':'Unassigned workers lost',
           'food_units':'Food','gold_cents':'Gold (cents)','troops':'Troops',
           'support_change_ticks':'Public support change','troops_lost':'Troops lost',
           'workers_lost':'Workers lost','food_lost':'Food lost','cultivation':'Cultivation'}
    result=[]
    for key,value in effects.items():
        named=label.get(key, key.replace('_',' ').capitalize())
        if key in ('support_ticks','support_change_ticks') and type(value) is int:
            # Engine stores 50,000 ticks per visible point. Integer floor division
            # misreports -25,000 ticks as -1 point rather than -0.5.
            points=value/50000
            result.append(f'{named}: {points:+,.4f}'.rstrip('0').rstrip('.')+' points')
        elif type(value) is int:result.append(f'{named}: {value:+,}')
        elif type(value) in (str,float):result.append(f'{named}: {value}')
        else:result.append(f'{named}: recorded')
    return '; '.join(result)

def order_summary(state, kind, params, result, actor=None):
    """The result dictionary is diagnostic; display a bounded, faithful subset."""
    action=str(kind).replace('_',' ').title()
    who=officer_name(state,actor) if actor is not None else 'Officer'
    pid=params.get('province_id') or (result.get('province_id') if isinstance(result,dict) else None)
    place=province_name(state,pid) if pid is not None else ''
    units=params.get('units',params.get('count',params.get('troops')))
    summary=f'{who}: {action}'
    if place:summary+=' in '+place
    if units is not None:summary+=f' ({units} units)'
    if isinstance(result,dict):
        for key,title in (('cultivation_after','cultivation'),('food_after','food'),
                          ('troops_after','troops'),('bp_after','BP remaining'),
                          ('gold_after_cents','gold cents'),('gold_cents_after','gold cents')):
            if key in result and type(result[key]) in (int,float):
                summary+=f'; {title} {result[key]:,}'
    return summary+'.'

def monthly_summary(opponents, month):
    if not isinstance(opponents,list):return f'Month {month} complete. See Journal for details.'
    names=Counter(str(x.get('kind','unknown')).replace('_',' ') for x in opponents if isinstance(x,dict))
    return f'Month {month} complete. {len(opponents)} rival decisions; '+', '.join(
        f'{n} {name}' for name,n in names.most_common(2))+'.'

def _player_value(state, key, value):
    """Resolve IDs from committed records. The raw log remains untouched."""
    if value is None:return 'Unknown'
    if key in ('province_id','province','src','dst','target','objective','location'):
        if str(value) in state.get('factions',{}):return faction_name(state,value)
        if str(value) in state.get('core',{}).get('officers',{}):return officer_name(state,value)
        if str(value).startswith('p') and str(value)[1:].isdigit():return province_name(state,value)
    if key in ('officer_id','target_officer','officer'):
        return officer_name(state,value)
    if key in ('faction','ruler','ally'):
        return faction_name(state,value)
    if key in ('gold_cents','gold_after_cents','gold_cents_after','gold_change_cents') and type(value) is int:
        return f'{value/100:,.2f} gold'
    if key in ('support_ticks','support_change_ticks') and type(value) is int:
        return f'{value/50000:+,.4f}'.rstrip('0').rstrip('.')+' support points'
    if type(value) is int:return f'{value:,}'
    if type(value) is float:return f'{value:,.2f}'
    # Internal unknown identifiers, diagnostics and complex objects must not be
    # copied verbatim into the player-facing Journal.
    if isinstance(value,(dict,list,tuple,set)):return 'See detailed campaign log'
    if isinstance(value,str) and (value.startswith(('nes:','rec')) or (len(value)>1 and value[0]=='R' and value[1:].isdigit())):
        return 'Unidentified'
    return str(value)


def journal_entry(state,entry):
    """Persistent chronological account from saved committed actions and outcomes."""
    if not isinstance(entry,dict):return 'Campaign event recorded.'
    month=entry.get('month')
    when=f'Month {month}' if isinstance(month,int) else 'Campaign'
    kind=str(entry.get('kind','event')).replace('_',' ').title()
    actor=entry.get('actor')
    actor_label=(officer_name(state,actor) if actor in state.get('core',{}).get('officers',{})
                 else faction_name(state,actor) if actor in state.get('factions',{}) else 'Campaign')
    result=entry.get('result',{}) if isinstance(entry.get('result'),dict) else {}
    params=entry.get('params',{}) if isinstance(entry.get('params'),dict) else {}
    fields={**params,**result}
    captions={'province_id':'Province','target':'Target','src':'From','dst':'To',
              'count':'Count','units':'Units','troops':'Troops','outcome':'Outcome',
              'cultivation_before':'Cultivation before','cultivation_after':'Cultivation after',
              'food_before':'Food before','food_after':'Food after','troops_before':'Troops before',
              'troops_after':'Troops after','bp_after':'BP remaining','gold_cents_after':'Gold after',
              'support_ticks':'Support change','target_officer':'Target officer',
              'officer_id':'Officer','commodity':'Commodity','side':'Trade'}
    segments=[]
    for key,caption in captions.items():
        if key in fields:
            segments.append(f'{caption}: {_player_value(state,key,fields[key])}')
    return f'{when} — {actor_label} — {kind}'+(' — '+', '.join(segments[:7]) if segments else '')


def _points(ticks, signed=False):
    """50,000 internal ticks per displayed public-support point, no floor bias."""
    number=ticks/50000
    return (f'{number:+,.3f}' if signed else f'{number:,.3f}').rstrip('0').rstrip('.')


def advisor_text(state,info):
    """Ten actual domestic indicators and saved-state six-month comparisons.

    This is read-only presentation; no imaginary prices, income or officer stats.
    An unavailable baseline is never represented as zero change.
    """
    ruler=faction_name(state,info.get('ruler'))
    out=[f'Advisor report — {ruler}',
         f'Month {info.get("month")} • {info.get("observed_months",0)} recorded month snapshots',
         'Ten domestic measures: troops, food, gold, public support, metal, arms,',
         'cultivation, flood control, wealth and assigned workforce.', '']
    for rec in info.get('advice',[]):
        out.append(province_name(state,rec.get('province')))
        out.append('  Troops: {:,}   Food: {:,}   Gold: {:,.2f}'.format(rec['troops'],rec['food'],rec['gold_cents']/100))
        out.append('  Public support: {}   Metal: {:,}   Arms: {:,}'.format(
                   _points(rec['support_ticks']),rec['metal'],rec['arms']))
        p=state['core']['provinces'].get(rec['province'],{})
        out.append('  Cultivation: {}   Flood control: {}   Wealth: {}'.format(
            p.get('cultivation','Unavailable'),p.get('flood_control','Unavailable'),
            ('{:,}'.format(p['wealth_milli']/1000) if 'wealth_milli' in p else 'Unavailable')))
        workers=p.get('workers',{})
        out.append('  Assigned workforce: '+(f'{sum(v for k,v in workers.items() if k!="unassigned"):,}' if workers else 'Unavailable'))
        out.append('  '+('; '.join(rec.get('warnings',[])) or 'No immediate warnings.'))
    out+=['','Six-month change (where continuously held and recorded):']
    for rec in info.get('six_month_review',[]):
        out.append('{}: Gold {:+,.2f}; Food {:+,}; Support {}; Troops {:+,}'.format(
            province_name(state,rec['province']),rec['gold_change_cents']/100,
            rec['food_change'],_points(rec['support_change_ticks'],signed=True),rec['troop_change']))
    if not info.get('six_month_review'):
        out.append('No six-month comparison for continuously held provinces yet.')
    if info.get('excluded_new_provinces'):
        out.append('{} recently gained province(s) excluded from six-month comparisons.'.format(info['excluded_new_provinces']))
    return '\n'.join(out)


def explain_error(exc):
    """User-facing command failure. Preserve typed diagnostic code for logs, not UI.

    Do not silently auto-correct a rejected order or change engine state.
    """
    codes={
       'NO_DAUGHTER':'No eligible daughter is recorded for your ruler; marriage is unavailable.',
       'NOT_ALLIED':'An active alliance with that ruler is required for this order.',
       'NOT_FRIENDLY_ADJACENT':'Choose a neighboring province controlled by your own ruler.',
       'NO_BP':'The selected officer does not have enough Body Points.',
       'NO_STOCK':'There are not enough goods or troops available.',
       'NO_GOLD':'The treasury does not have enough gold.',
       'NO_FOOD':'There is not enough food for this action.',
       'NO_TROOPS':'There are not enough troops available.',
       'NO_ARMY_TROOPS':'The selected officer does not command enough troops.',
       'NO_OFFICER':'An eligible officer is required.',
       'NO_BORDER':'The target does not share a reachable border.',
       'NOT_VACANT':'That province is already controlled by another ruler.',
       'ARMY_NOT_ALL_AT_HOME':'The commander must have their entire available army stationed here.',
       'COOLDOWN':'This diplomatic action is not available again yet.',
       'BAD_TARGET':'Choose an eligible target for this order.',
       'INVALID_WAR_OBJECTIVE':'Choose a neighboring opponent province for this attack plan.',
       'NO_SCHEDULED_ATTACK':'Plan a legal attack on that objective before requesting joint operations.',
       'BATTLE_ACTIVE':'Finish the current battle before starting another order.',
       'NO_MARKET_STOCK':'The market cannot supply that quantity.',
       'WAREHOUSE_FULL':'The warehouse has no capacity for that quantity.',
       'STALE_QUOTE':'The campaign changed after preview. Preview the order again.',
       'STALE_REVISION':'The campaign changed after preview. Preview the order again.',
       'DUPLICATE_REQUEST':'That order was already committed; it will not be charged again.',
       'NO_SUITABLE_OFFICER':'No eligible officer is available for this order.',
       'NO_RECRUITABLE_OFFICER':'No officer is available for recruitment here.',
    }
    code=getattr(exc,'code',None)
    if code is not None:
        return codes.get(code,'The order cannot be completed with the current resources or conditions.')
    if isinstance(exc,ValueError):return str(exc)
    if isinstance(exc,OSError):return 'The game could not access the save file. Check storage and try again.'
    return 'The order could not be completed. See the technical log for details.'
