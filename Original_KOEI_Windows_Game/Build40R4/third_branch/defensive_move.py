"""Director defense-first policy for legal neutral strategic Move Army.

Shared by command engine and AI. Conservative policy, not a fabricated original
ROTK II combat constant. Never reads hidden future turns or modifies state.
"""
from __future__ import annotations
from . import army_cohorts as ac


def source_reserve(s, pid, faction, departing):
    hostiles = [s['core']['provinces'][other]['troops'] for other in s['neighbors'][pid]
                if s['owners'][other] not in (None, faction)]
    if hostiles:
        return max(15, (max(hostiles)*4+4)//5 + 5*len(hostiles))
    return max(15, (departing+3)//4)


def source_safe(s, pid, faction, departing, commander):
    if departing <= 0:return False
    left = s['core']['provinces'][pid]['troops']-departing
    if left < source_reserve(s,pid,faction,departing):return False
    # A reserve cannot consist solely of detached troops with no officer on
    # site; require a second physically present, real commanding officer.
    return any(oid != commander and s['officer_faction'].get(oid)==faction
               and off['province_id']==pid and ac.strength(s,oid,pid)>0
               for oid,off in s['core']['officers'].items())
