"""Approved ECON-MASTER v2.1 Distribute economics as third-game Bus override.
BP cost is still a provisional per-action parameter from the current core.
"""
from og.core.commands import _check_amount, _officer_in
from og.core.fixed import CoreError


def _distribute(world, actor, params, key, multiplier):
    _check_amount(world, params, ('province_id','units'), 'units')
    n=params['units']
    if n>100:raise CoreError('BAD_PARAMS','Distribute maximum is 100 units')
    o, cost = _officer_in(world,actor,params['province_id'])
    p=world['provinces'][params['province_id']]
    if p[key]<n:raise CoreError('NO_STOCK',key)
    remaining=5000000-p['support_ticks']
    if remaining<=0:raise CoreError('AT_CAP','public support maximum')
    per=multiplier*(1000+2*o['charisma'])
    spend=min(n,(remaining+per-1)//per)
    gain=min(remaining,spend*per)
    p[key]-=spend
    p['support_ticks']+=gain
    o['bp']-=cost
    return {'province_id':params['province_id'],'requested_units':n,
            'consumed_units':spend,'support_gain_ticks':gain,'support_ticks_after':p['support_ticks'],
            'bp_after':o['bp']}


def food(w,actor,p):return _distribute(w,actor,p,'food_units',1)
def goods(w,actor,p):return _distribute(w,actor,p,'specialty_units',5)
HANDLERS={'distribute_food':food,'distribute_goods':goods}
