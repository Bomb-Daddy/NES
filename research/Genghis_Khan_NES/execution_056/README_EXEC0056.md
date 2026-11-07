# EXEC0056 — Genuine native controller draft correction (additive)

Continues unchanged pre-setup templates and existing 1280×720 cross-platform C11 source.
On the six-ability, five-character manual bonus *draft* editor, X now removes
one point and Y removes up to ten from the selected draft field (clamped at 0).
A (+1) and B (+10) remain unchanged as verified English NES manual inputs.
X/Y outside active manual editing still save/load using the prior 2,734-byte
source-gated CRC32 exclusive-create setup draft store. No new save version,
original NES mechanic, first playable Spring, or gameplay result is inferred.

QA: tests from same native shell plus standalone pad->app interaction; Debug,
Release and sanitizer CTest; separate-process native shell save/load; attempted
second save must refuse overwrite and invalid load must refuse without mutation.
SYNTHETIC controller inputs; no actual Windows/Xbox/original NES execution.
