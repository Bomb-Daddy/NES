# Original KOEI third branch — Director economy correction v1.12

Historical Scenario 1 only. Source fork of previously tested v1.11. Direct Oct 30 Director corrections supersede older hypothetical numerical recommendations. All unchanged old save policy IDs retain original semantics.

- **New saves:** `OG_1000_DIRECTOR_V2` — cultivation, wealth, flood 0–1000 with slower progress. Legacy V1 policy and untagged files remain loader-compatible.
- **Government food:** No civilian monthly food removal. Existing army ration costs (troop monthly + movement) retained; no separate civilian shortage support penalty.
- **Metal:** permanent nondepleting **only if** a per-province source flag is True. Unknown/False flag yields no mining. 41-province metal site mapping is **BLOCKED_SOURCE**, all 41 default unknown. No invented ore sites or depletion.
- **Markets:** unlimited external counterparty buying/selling, subject to player's gold/stocks, size/BP/quoting, seasonal and scarcity pricing. Existing `market_stocks` index is only a price-pressure indicator; negative stock is prohibited.
- **Costs:** do not add new public-government recurrent costs. Retain previous March troop pay and explicit army rations; extra experimental recurring 10 cents/troop wage NOT applied under V2.
- **Occupation:** a player-visible `occupy_vacant` order and AI use real officer and its entire assigned stationed army to move to a genuine adjacent `None` province. No enemy army, no imaginary battle. Source troop count goes down, target goes up, original province remains owned, target is governed next month. Full preview, revision, duplicate-request, atomic save and cold-load requirements.
- **AI:** profile-weighted ROI/incentive based on food reserve vs actual military demand. Vacant settlements scored with profile-specific weights; main PC historical campaign uses weighted chooser only on new policy.

**NOT VERIFIED:** native Windows build/PC controller, historical NES fidelity of this original game, real metal deposit geography. Test numbers are balance candidates, not final Director acceptance. One 120-month seed does not demonstrate cross-seed balance.
