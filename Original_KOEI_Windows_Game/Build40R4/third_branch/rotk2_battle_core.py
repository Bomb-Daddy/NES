"""Shared ROTK II tactical combat primitives, v1.4 SNES/NES unified rules.

Source: Nobunaga ROTK2_Battle_Answer_Matrix_v1_4, Q01/Q02/Q03/Q05/Q08/Q13.
The original-game integration currently has TWO aggregated armies, not the NES's
four officer lists; this module is not a byte-for-byte NES battle emulator.
No Enhanced modifiers are applied inside these primitives.
"""
from __future__ import annotations
import heapq

W,H=13,12
COST={'.':2,'F':3,'H':3,'M':7,'W':5,'T':3,'C':3,'S':0}
DEF_DIV={'.':10,'F':10,'H':12,'M':10,'W':8,'T':15,'C':20,'S':5}
FIRE_CHANCE={'.':80,'F':90,'H':70,'M':60,'W':0,'T':50,'C':20,'S':10}
# The six directions exactly match SNES hex:$C1EF, Q01: odd columns shift down.
def neighbors(q,r):
    offsets=(((-1,1),(0,1),(1,1),(-1,0),(0,-1),(1,0)) if q&1 else
             ((-1,0),(0,1),(1,0),(-1,-1),(0,-1),(1,-1)))
    for dq,dr in offsets:
        x,y=q+dq,r+dr
        if 0<=x<W and 0<=y<H:yield x,y

def distance(a,b):
    q,r=a; x,y=b
    az=r-(q-(q&1))//2
    bz=y-(x-(x&1))//2
    return max(abs(q-x),abs(az-bz),abs((q+az)-(x+bz)))

def refill(training,remaining=0):
    return max(remaining, max(2,min(6,(max(1,training)-1)//20+2)))

def reachable(grid,start,mobility,blocked=(),burning=()):
    blocked=set(map(tuple,blocked))|set(map(tuple,burning))
    best={tuple(start):0}; queue=[(0,*start)]
    while queue:
        cost,q,r=heapq.heappop(queue)
        if cost!=best[(q,r)]:continue
        for x,y in neighbors(q,r):
            if (x,y) in blocked:continue
            terrain=grid[y][x]
            new=cost+COST[terrain]
            if new<=mobility and new<best.get((x,y),10**9):
                best[(x,y)]=new;heapq.heappush(queue,(new,x,y))
    del best[tuple(start)]
    return best

def shortest_route(grid,start,target,mobility,blocked=(),burning=()):
    """Q02 lowest-cost ordered hex steps; Q07 reactions execute at each step.

    Uses the same terrain costs, occupied cells, fire exclusion and strict
    improvement rule as reachable(), so no alternate teleport movement exists.
    """
    start,target=tuple(start),tuple(target)
    if start==target:raise ValueError('ILLEGAL_MOVE')
    forbidden=set(map(tuple,blocked))|set(map(tuple,burning))
    best={start:0};prev={};heap=[(0,*start)]
    while heap:
        cost,q,r=heapq.heappop(heap)
        pos=(q,r)
        if cost!=best.get(pos):continue
        if pos==target:
            route=[]
            while pos!=start:
                route.append(pos);pos=prev[pos]
            route.reverse()
            return route
        for nxt in neighbors(q,r):
            if nxt in forbidden:continue
            nc=cost+COST[grid[nxt[1]][nxt[0]]]
            if nc<=mobility and nc<best.get(nxt,10**9):
                best[nxt]=nc;prev[nxt]=(q,r)
                heapq.heappush(heap,(nc,*nxt))
    raise ValueError('ILLEGAL_MOVE')

def _power(unit):
    return 3*(unit['war']+(20 if unit.get('lu_bu',False) else 0))+unit['morale']+unit.get('troop_ratio',100)

def exchange(att,dfn,att_tile,dfn_tile,rand,*,difficulty=2,att_ai=False,dfn_ai=False,charge=False):
    """One Q05 simultaneous exchange, with exact order of per-side loss calculations.

    `troops100` stores the original battle's hundredfold field troop units.
    `rand(n)` must produce 0..n-1 and be fed by the campaign's serialized RNG.
    The difficulty reduction applies only to the CPU unit facing a human.
    """
    if not (1<=difficulty<=5):raise ValueError('bad difficulty')
    def loss(receiver,opponent,terrain,opponent_tile,receiver_ai,opponent_ai,opponent_active):
        divisor=5 if opponent_active and opponent_tile=='C' else DEF_DIV[terrain]
        # Python // rounds negative down; original truncates signed division toward zero.
        numerator=(_power(receiver)-_power(opponent)-min(opponent['troops100'],rand(300)))*10
        d=difficulty if receiver_ai and not opponent_ai else 1
        v=abs(numerator)//(divisor*d)
        v=v if numerator>=0 else -v
        raw=rand(30)+1 if v>=0 else -v
        return min(receiver['troops100'],raw)*receiver.get('troop_ratio',100)//100
    # Attacker's loss computed before defender's; neither is applied until both are known.
    aa=loss(att,dfn,att_tile,dfn_tile,att_ai,dfn_ai,False)
    dd=loss(dfn,att,dfn_tile,att_tile,dfn_ai,att_ai,True)
    att['troops100']=max(0,att['troops100']-aa)
    dfn['troops100']=max(0,dfn['troops100']-dd)
    return aa,dd

def projected_soldiers(unit):
    # Original normalizes the scaled troop record on leaving the field.
    return max(0,unit['troops100']//100)

def rice_tick(rice_hundredths,field_troops100,extra_defenders100=0):
    consumption=max(1,(field_troops100+extra_defenders100)//30)
    return max(0,rice_hundredths-consumption)

def ignite(terrain,actor_int,target_int,occupied,weather,rand):
    if weather==3 or terrain=='W':return False
    chance=FIRE_CHANCE[terrain]+rand(10)
    if occupied:
        chance=(101-target_int)*chance//100 + (rand(max(1,actor_int-target_int))//2 if actor_int>target_int else 0)
    return rand(100)<min(100,chance)
