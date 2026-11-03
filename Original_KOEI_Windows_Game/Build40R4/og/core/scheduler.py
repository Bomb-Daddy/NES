"""ECON-R1 13-phase monthly scheduler.

Phase table is data-keyed (ids 0..12). It is the ECON-001 v1.2 section 6 table; phase identity and order are
SETTLED by SUP-ECON-0044 (13 rows, ids 0..12) and are not placeholders.
Implemented for schema v2 worlds (no-ops on v1): phase 0 March freeze, phase 3 Wealth change and September credit,
phase 6 March tax (with support change) and September payout, phase 8 January population growth, phase 2 disasters (see economy.py). Phase 1 draws one world-RNG value (placeholder behavior).
Every other phase still has a PLACEHOLDER no-op handler; see PLACEHOLDER_HANDLERS_REMAINING.

State lives in the SAME authoritative world. No schema change: the phase cursor and markers are
the journal itself. Each phase commit appends one journal event with id "phase:mMMMMM:pNN" and the
same id is appended to processed_ids (unique journaled request id). Revision +1 per phase.
"""
import copy

from . import economy, bp_policy
from .fixed import CoreError
from .rng import randrange
from .world import validate_world

PHASE_IDENTITY_SETTLED = True

PHASE_TABLE = (
    (0, "freeze", "Freeze"),
    (1, "weather_rng", "Weather RNG"),
    (2, "disaster", "Disaster"),
    (3, "civilian", "Civilian development/extraction"),
    (4, "metalworking", "Metalworking"),
    (5, "transport", "Transport arrival"),
    (6, "fiscal", "Fiscal collection"),
    (7, "upkeep", "Upkeep"),
    (8, "demographics", "Demographics"),
    (9, "political", "Political feedback"),
    (10, "next_market", "Next-market"),
    (11, "commit", "Commit"),
    (12, "reports", "Reports"),
)
LAST_PHASE = PHASE_TABLE[-1][0]


def _noop(world, month):
    return None


def _weather(world, month):
    randrange(world, 6)  # PLACEHOLDER: consumes one deterministic draw


PLACEHOLDER_HANDLERS = {key: _noop for _, key, _ in PHASE_TABLE}
PLACEHOLDER_HANDLERS["weather_rng"] = _weather
# Implemented for schema v2 worlds only (no-ops on v1): March frozen tax base. Everything else stays a placeholder.
PLACEHOLDER_HANDLERS["freeze"] = economy.h_freeze
PLACEHOLDER_HANDLERS["civilian"] = economy.h_civilian
PLACEHOLDER_HANDLERS["fiscal"] = economy.h_fiscal
PLACEHOLDER_HANDLERS["demographics"] = economy.h_demographics
PLACEHOLDER_HANDLERS["disaster"] = economy.h_disaster
PLACEHOLDER_HANDLERS["reports"] = bp_policy.recover_next_month  # monthly rollover for next player turn
# Phases whose behavior is still a placeholder (weather draws one RNG value; the rest are no-ops).
PLACEHOLDER_HANDLERS_REMAINING = tuple(key for _, key, _ in PHASE_TABLE if key not in ("freeze", "disaster", "civilian", "fiscal", "demographics", "reports"))


def phase_event_id(month, phase):
    return "phase:m%05d:p%02d" % (month, phase)


def _parse(ev_id):
    # "phase:m00001:p03" -> (1, 3) or None
    if not ev_id.startswith("phase:m"):
        return None
    try:
        m, p = ev_id[7:].split(":p")
        return int(m), int(p)
    except ValueError:
        return None


def cursor(world):
    """Return (month, next_phase) derived from the journal. Months start at 1."""
    last = None
    for ev in world["journal"]:
        parsed = _parse(ev["id"])
        if parsed is not None:
            last = parsed
    if last is None:
        return 1, 0
    month, phase = last
    if phase == LAST_PHASE:
        return month + 1, 0
    return month, phase + 1


def run_phase(bus, month, phase, expected_revision, handlers=None, fault_at=None):
    """Run exactly one phase atomically on bus.data. Raises CoreError; on error the world is untouched."""
    data = bus.data
    table = dict(PLACEHOLDER_HANDLERS)
    if handlers:
        table.update(handlers)
    if not isinstance(month, int) or isinstance(month, bool) or not isinstance(phase, int) or isinstance(phase, bool):
        raise CoreError("BAD_ENVELOPE", "month/phase must be int")
    rid = phase_event_id(month, phase) if 0 <= phase <= LAST_PHASE and month >= 1 else None
    if rid is None:
        raise CoreError("BAD_PHASE", "month=%r phase=%r" % (month, phase))
    if rid in data["processed_ids"]:
        raise CoreError("DUPLICATE_ID", rid)
    if expected_revision != data["revision"]:
        raise CoreError("STALE_REVISION", "expected %r have %r" % (expected_revision, data["revision"]))
    want = cursor(data)
    if (month, phase) != want:
        raise CoreError("PHASE_ORDER", "requested %r, next is %r" % ((month, phase), want))
    key = PHASE_TABLE[phase][1]
    staged = copy.deepcopy(data)
    table[key](staged, month)
    if fault_at == phase:
        raise CoreError("FAULT", "injected at phase %d" % phase)
    staged["revision"] += 1
    staged["journal"].append({"seq": len(staged["journal"]) + 1, "id": rid, "kind": "phase:" + key,
                              "revision": staged["revision"]})
    staged["processed_ids"].append(rid)
    validate_world(staged)
    bus._data = staged
    return rid


def run_month(bus, handlers=None, fault_at=None):
    """Run all remaining phases of the current month in order."""
    month, phase = cursor(bus.data)
    ids = []
    while True:
        ids.append(run_phase(bus, month, phase, bus.data["revision"], handlers, fault_at))
        if phase == LAST_PHASE:
            return ids
        phase += 1
