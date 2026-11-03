"""NEW 36-month determinism + save/replay run (2026-10-28, SUP-NOB-0046 task C). Real RNG, real disaster handler,
two provinces, scripted commands. Prints every hash it compares. Exit 0 only if every comparison holds.
Coefficients are SYNTHETIC-NOT-APPROVED (og.core.world2.FIXTURE_V1). Run from the snapshot root."""
import sys

from og.core import scheduler as S
from og.core import world2 as W2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import add_officer, add_province, new_world, world_hash

KEYS = ("farmers", "town", "artisans", "metalworkers", "resources", "unassigned", "land", "flood")
P = {"p1": dict(cult=90, w=(300, 120, 60, 30, 30, 60, 0, 0)), "p2": dict(cult=25, w=(150, 60, 40, 10, 10, 30, 0, 0))}


def build(seed):
    w = new_world(seed)
    for pid, q in P.items():
        add_province(w, pid, q["cult"])
    add_officer(w, "o1", "p1", (50, 50, 50, 50, 50))
    w = W2.migrate_v1_to_v2(w, start_year=1, start_month=1)
    for pid, q in P.items():
        p = w["provinces"][pid]
        p["workers"] = dict(zip(KEYS, q["w"]))
        p["wealth_milli"] = 40000
        p["support_ticks"] = 1000000
        p["tax_rate_percent"] = 30
        p["warehouse_capacity_units"] = 500
        p["treasury_gold_cents"] = 5000
        p["food_units"] = 300
    return Bus(w)


def scripted(bus, month, log):
    """A fixed command per month (alternating kinds); outcome code is logged so refusals are deterministic too."""
    kinds = [("allocate_workers", dict(province_id="p1", source="unassigned", target="land", count=5)),
             ("recruit", dict(province_id="p1", count=3)),
             ("distribute_food", dict(province_id="p1", units=20))]
    kind, params = kinds[month % 3]
    cmd = {"request_id": "m%d" % month, "actor_officer_id": "o1", "expected_world_revision": bus.data["revision"],
           "kind": kind, "params": params}
    try:
        bus.commit(cmd)
        log.append((month, kind, "ok"))
    except CoreError as e:
        log.append((month, kind, e.code))


def run(seed, months=36, cut=None):
    """cut=None: uninterrupted. cut=(phase_mod): save+cold-load after EVERY phase whose global index % phase_mod == 0."""
    b = build(seed)
    log = []
    n = 0
    for m in range(1, months + 1):
        scripted(b, m, log)
        # run_month executes the 13 phases for the cursor month; use phases individually when cutting
        if cut is None:
            S.run_month(b)
        else:
            cm, cp = S.cursor(b.data)
            for ph in range(cp, 13):
                S.run_phase(b, cm, ph, b.data["revision"])
                n += 1
                if n % cut == 0:
                    b = Bus(load_save(dump_save(b.data)))
    return b, log


def main():
    ok = True
    a, la = run(7)
    h = world_hash(a.data)
    ev = {}
    for e in a.data["event_ledger"]:
        ev[e["kind"]] = ev.get(e["kind"], 0) + 1
    print("run A  seed=7 months=36 revision=%d hash=%s" % (a.data["revision"], h))
    print("       ledger=%d stock=%d events=%s cmd-outcomes=%s" % (len(a.data["ledger"]), len(a.data["stock_ledger"]), ev,
                                                                  sorted({(k, c) for _, k, c in la})))
    b, lb = run(7)
    ok &= world_hash(b.data) == h and la == lb
    print("run B  seed=7 identical hash=%s log-equal=%s -> %s" % (world_hash(b.data) == h, la == lb, "OK" if world_hash(b.data) == h and la == lb else "FAIL"))
    c, _ = run(8)
    diff = world_hash(c.data) != h
    ok &= diff
    print("run C  seed=8 differs from seed=7: %s (hash=%s)" % (diff, world_hash(c.data)))
    for cut in (1, 3, 13):
        d, ld = run(7, cut=cut)
        same = world_hash(d.data) == h and ld == la and d.data["event_ledger"] == a.data["event_ledger"]
        ok &= same
        print("run D  save+cold-load every %2d phase(s): hash=%s equal=%s" % (cut, world_hash(d.data), same))
    raw = dump_save(a.data)
    ok &= dump_save(load_save(raw)) == raw
    print("final save round-trip bytes identical: %s (%d bytes)" % (dump_save(load_save(raw)) == raw, len(raw)))
    print("RESULT", "PASS" if ok else "FAIL")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
