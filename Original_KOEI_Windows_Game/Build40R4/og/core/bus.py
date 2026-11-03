"""Atomic command bus: preview (pure) and commit (stage on a copy, validate, swap)."""
import copy
import hashlib

from . import commands
from .fixed import CoreError, check_int, check_tree
from .world import canonical_bytes, validate_world

ENVELOPE_KEYS = {"request_id", "actor_officer_id", "expected_world_revision", "kind", "params"}


class Bus:
    def __init__(self, data, handlers=None):
        validate_world(data)
        self._data = data
        self._handlers = dict(commands.HANDLERS)
        if handlers:
            self._handlers.update(handlers)

    @property
    def data(self):
        return self._data

    def _check_envelope(self, cmd):
        if not isinstance(cmd, dict):
            raise CoreError("BAD_ENVELOPE", "command must be an object")
        check_tree(cmd)
        if set(cmd) != ENVELOPE_KEYS:
            raise CoreError("BAD_ENVELOPE", "keys")
        rid = cmd["request_id"]
        if not isinstance(rid, str) or not rid:
            raise CoreError("BAD_ENVELOPE", "request_id")
        if not isinstance(cmd["actor_officer_id"], str) or not isinstance(cmd["kind"], str) or not isinstance(cmd["params"], dict):
            raise CoreError("BAD_ENVELOPE", "field types")
        check_int(cmd["expected_world_revision"], 0, name="expected_world_revision")
        if cmd["kind"] not in self._handlers:
            raise CoreError("UNKNOWN_KIND", cmd["kind"])
        if cmd["actor_officer_id"] not in self._data["officers"]:
            raise CoreError("UNKNOWN_ACTOR", cmd["actor_officer_id"])
        if rid in self._data["processed_ids"]:
            raise CoreError("DUPLICATE_ID", rid)
        if cmd["expected_world_revision"] != self._data["revision"]:
            raise CoreError("STALE_REVISION", "expected " + str(cmd["expected_world_revision"]) + " have " + str(self._data["revision"]))

    def _stage(self, cmd):
        staged = copy.deepcopy(self._data)
        result = self._handlers[cmd["kind"]](staged, cmd["actor_officer_id"], cmd["params"])
        check_tree(result)
        return staged, result

    def quote_id(self, cmd):
        return hashlib.sha256(canonical_bytes({"cmd": cmd, "revision": self._data["revision"]})).hexdigest()

    def preview(self, cmd):
        self._check_envelope(cmd)
        staged, result = self._stage(cmd)
        return {"quote_id": self.quote_id(cmd), "request_id": cmd["request_id"],
                "world_revision": self._data["revision"], "result": result}

    def commit(self, cmd, quote_id=None):
        self._check_envelope(cmd)
        if quote_id is not None and quote_id != self.quote_id(cmd):
            raise CoreError("STALE_QUOTE", "quote does not match command")
        staged, result = self._stage(cmd)
        staged["revision"] += 1
        staged["journal"].append({"seq": len(staged["journal"]) + 1, "id": "cmd:" + cmd["request_id"],
                                  "kind": cmd["kind"], "revision": staged["revision"]})
        staged["processed_ids"].append(cmd["request_id"])
        validate_world(staged)
        self._data = staged
        return {"request_id": cmd["request_id"], "world_revision": staged["revision"], "result": result}
