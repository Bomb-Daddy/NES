"""Integer-only authority helpers. No floats, no bools, 64-bit bounded."""

INT_MIN = -(1 << 63)
INT_MAX = (1 << 64) - 1


class CoreError(Exception):
    def __init__(self, code, msg=""):
        super().__init__(code + ": " + msg)
        self.code = code


def check_int(v, lo=INT_MIN, hi=INT_MAX, name="value"):
    if isinstance(v, bool) or not isinstance(v, int):
        raise CoreError("NOT_INT", name + " must be int, got " + type(v).__name__)
    if v < lo or v > hi:
        raise CoreError("OUT_OF_RANGE", name + "=" + str(v) + " outside " + str(lo) + ".." + str(hi))
    return v


def check_tree(obj, path="$"):
    """Allow only dict(str keys), list, str, bounded int. Rejects float, bool, None."""
    if isinstance(obj, dict):
        for k, v in obj.items():
            if not isinstance(k, str):
                raise CoreError("BAD_KEY", path)
            check_tree(v, path + "." + k)
    elif isinstance(obj, list):
        for i, v in enumerate(obj):
            check_tree(v, path + "[" + str(i) + "]")
    elif isinstance(obj, str):
        pass
    else:
        check_int(obj, name=path)
