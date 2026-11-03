"""Own deterministic 64-bit generator (SplitMix64). State lives in world['rng_state'] as an int."""
from .fixed import check_int

MASK = (1 << 64) - 1


def next_u64(data):
    s = (data["rng_state"] + 0x9E3779B97F4A7C15) & MASK
    data["rng_state"] = s
    z = ((s ^ (s >> 30)) * 0xBF58476D1CE4E5B9) & MASK
    z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & MASK
    return z ^ (z >> 31)


def randrange(data, n):
    check_int(n, 1, MASK, "n")
    limit = (1 << 64) - ((1 << 64) % n)
    while True:
        r = next_u64(data)
        if r < limit:
            return r % n
