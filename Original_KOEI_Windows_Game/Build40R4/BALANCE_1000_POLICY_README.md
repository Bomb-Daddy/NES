# Third Branch v1.11 — Slower economic growth (isolated 1000-point policy)

The Director requested land/farm and related development ceilings of 1,000 and a slower economy. This is a *reversible experimental candidate*, not an update to NES Classic, DOS or an accepted release. New Scenario 1 games launched through `third_branch.strategic_game` use the candidate. Existing saved games retain the old policy in their state; no silent conversion.

* Cultivation (land/farm improvement), wealth, flood control each range 0–1000. Officers remain 0–100, Body Points remain capped at 200, public support remains 0–100. Worker populations are persons, not cap-bounded scores.
* Existing historical ROM values 0–100 translate to 0–1000 by multiplying only on **new candidate game** creation (e.g. 55 -> 550). The source JSON stays unchanged. Starting Wealth 34 -> 340; historical flood-control 30 -> 300.
* Passive Wealth development +1 point/month on the 1000 scale; formerly +1 point/month on 100. Cap 1000, after 120 months starting 340 -> 460, never reaches cap.
* Cultivate order grants +1 point with the unchanged Body Point cost 2; crew progress is 5 milli-points/worker/month, from 10. All improvements use the 0–1000 cap.
* September food credit per farmer = 300 + 1*Cultivation millifood, instead of 500 + 10*Cultivation under 100 points. Effective initial harvest falls ~19% for cultivation 55/550; still respects worker allocation.
* Wealth tax denominator scaled 10x to preserve the nominal economic unit after 10x score conversion; March frozen snapshot and no-double-tax semantics preserved.
* New monthly consumption: ceil(total civilian workers/120) food units + ceil(troops/60), with nonnegative stock and reduced public support when food is insufficient. No phantom creation.
* Metal extraction each governed province = resources_workers // 35 per month; arms forging = min(metal, metalworkers // 25). The slower output is still infinite-deposit/simplified and must not be mistaken for the approved final finite-scarcity system.
* Additional monthly soldier wages = min(10 cents*troops, treasury), alongside existing annual March pay. Underfunding reduces support (new proposal).
* For owned provinces only, core freezes/development/disasters/fiscal/harvest/demographics occur. Unowned provinces retain their original starting stocks but cannot produce tax receipts, harvest, metal, or arms in this provisional neutral model. That is a deliberate no-government default, NOT a model of independent peasant activity.
* Only policy-tagged worlds select these values. The original untagged economy module and 308 regression tests remain unaffected. Old saves do not silently become 1000 scale.

Known remaining problems: money/food still accumulate, genuine market counterparty cash and finite mines absent, only one faction action per month, military AI too passive, specialist profiles repeat cultivation, population food shortages have only support consequence, original game release is incomplete. Do not promote balance PASS from one seed.
