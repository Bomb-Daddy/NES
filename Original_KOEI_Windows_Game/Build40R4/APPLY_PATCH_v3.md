# Fork4 P0 settlement fix — isolated v3 overlay

## Baseline contract
Apply to the full **Reward Governor Experimental v2** build only.
Before writing, SHA256-check its `third_branch/integrated.py` against `db4c9293a087bbf867663c10a7eea52eeb28bad8f3c54ec35e44e16235b8c61c`. If it differs, **stop**; do not blindly replace a newer branch.
Copy `third_branch/integrated.py` into that relative path and `tests/test_p0_multistation_capture_fix_v3.py` to `tests/`; then validate patched SHA256 `6a6e003577162ee1559d194f4cf9c7c79e00d6d125ec6e2714151e8be8ad7aae`. Run `python -m pytest -q -ra` and new focused tests.

## Behavior implemented
On attacker victory, if defeated officer has off-field army detachments and no standard retreat province, reassign these off-field soldiers to a surviving legal same-faction officer, preferentially located in the same remote province. If there is no surviving alternative, the officer withdraws to the distant garrison instead of being converted. The after-action history lists dispositions. The existing army and state validation run before commit, save/load and next-month advancement tested. This is an **interim ledger safety policy**, not original ROM battle parity or proof of prisoner/defection rules.

## Boundaries
No production files modified. Historic seed100005 month-47 saved replay has not been retested in this current workspace. Advisor AI handling of active battle must still be tested. Other P1/P2 QA issues remain. Do not reuse archived 20% campaign trajectories without version parity confirmation.
