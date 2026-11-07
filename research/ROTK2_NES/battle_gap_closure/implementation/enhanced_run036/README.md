# Original-KOEI Enhanced Run036 Q10 live list-head protection

Canonical Original-game project root 1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l. Separate from NES Classic SUP-0016 and DOS.

Exact immutable full-source parent: Run035 SHA256 48b4ed72b99141fe9672dcabbeb32cf417d498cb75461e5270e67bf0aceebf94.
New deterministic full-source SHA256 7b4e03d2f335a12fb5f5950e552b0fcd7e37f21abcfc425b8f68405211a92ecc, 148785676 bytes, 554 CRC-clean entries, 551/553 prior members byte identical. Two production Python modules modified, 1 test file added. No economy/BP source changes. No previous ZIP overwritten.

## Original source rule and defect
Answer Matrix v1.4 Q10 (hsovl1 D5DD, handle_unit_rout $7AC0): cannot bribe the first officer in the opponent's active linked list; that live head also does not undergo daily spontaneous switching. The pre-existing port exempted only an officer with static `commander=True`, leaving an unprotected successor when the original commander fled. That allowed a Q10 bribe against the live list head and a daily spontaneous switch of the head (potentially taking the province's last defensive army). The patch computes the first live member of the same side/four-slot group (with preserved order), before RNG, gold or a turn. Direct engine and Tk confirm/cancel now reject the unavailable head; ordinary nonheads and their bribe odds are unchanged. This corrects scoped observable gameplay but historical four-list phase scheduling is unverified. Tests exercise an existing Q09 escape fallback; its exact NES probability is NOT inferred.

## Executed Linux 3.13 / Xvfb
- Pristine Run035 with new 7 cases: 5 expected FAIL and 2 PASS (includes one unrelated Tcl geometry check which needed root.update_idletasks and was corrected before final GREEN).
- Corrected source: 7 PASS, expanded 200 PASS, cold-extracted ZIP expanded 200 PASS.
- Mutant source with the new engine bribe/daily head checks removed: 2 expected FAIL / 5 PASS, then restored to exact patch; restored 7 PASS.
- GUI actual 1280x720 six root commands, no invalid target confirmation; actual Wait `.ro2` disk save, second independent Python process cold-load fingerprint PASS.
- Source ZIP reconstructed twice byte-for-byte, 554 CRC passed; local 18-part SHA/size verified. Drive remote verification must be performed separately and not assumed.

## Remaining release blockers
Q09 automatic second battle while destination already attacked; original four-list tactical scheduler; 30 original English NES controlled event traces; exact text/pixel UI; full system unfiltered pytest; native Windows exe and physical Xbox; Director acceptance. Source answer matrix is VERIFIED_STATIC reference, not original NES emulator executed this run.

TESTED_PORT scoped, OBSERVED_RUNTIME Linux/Xvfb; all original NES emulator, Windows and physical controller REPORTED_UNREPRODUCED NOT RUN. The original historical source list position is inferred from previously decoded SNES unified mechanics and the existing port's ordered officer_units, and requires original goldens for full parity.
