# ZL-GK1-EXEC-0018 — Normal-melee rules, executable kernel and new native captures

**Version:** 1.00 · **Date:** November 5, 2026 · **Author:** Zhuge Liang  
**To:** Director; Nobunaga (Windows integration); Cao Cao (new-component QA)  
**Guide:** `GK1_WINDOWS_MASTER_GUIDE.md`, G03 normal attacks; G09 event reuse.

## Result and scope

The first missing normal-attack damage contract is now implemented in `normal_melee.py`. It models normal-melee strength, unit matchup, terrain/role multipliers, castle fortification reduction, percentage share, two ordered casualty draws, rounding, word overflow and saturated losses. Nine new controller-driven modified-ROM attack captures match the numeric kernel, with a canceled target-selection control. Forty-four additional bounded source-instruction vectors check the new arithmetic and attribute boundaries. This is not a complete tactical engine, a fresh stock-ROM comparison, or a Windows executable.

The existing EXEC0015 in-battle checkpoint was reused. No completed economic, audio-sequencer, invasion/retreat or save/load investigation was repeated. All native work uses approved Shared-BP/Rest ROM SHA-256 `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`. The original and modified versions remain their respective behavioral references.

## 1. Actual new native work

Two isolated Linux headless Mesen processes ran: initial navigation/movement exploration, then instrumented attack discovery. Both are stopped. Exact emulator loader SHA-256 `e777af28bc6cc49d2e61ca0c1a82fbccf13811355332a53ea00749fc37a92761`, corrected core `0668f6c09bc89a1c8199df6cbf15aeebba243169b30f8116df97127dbfd3d7af`. Runtime/ROM were recovered from the existing private developer package, not installed on the Director's PC.

**Declared input fixtures:** from the accepted first-unit-order checkpoint, enemy unit11 was positioned adjacent to commander unit0 by writing only its x/y bytes to `(8,7)` at `$60DB`. Each comparison restores that same checkpoint and adds only its named input change. This avoids replaying invasion and does not manufacture damage. Unit types, input attributes, logical terrain or initial target count were varied; no game code or result field was patched. Logical terrain changes are arithmetic fixtures, not proof of a naturally generated castle or its redrawn graphics.

| Case | Changed input beyond adjacency | Attacker share | Attacker soldiers | Defender soldiers |
|---|---|---:|---:|---:|
| Baseline | None; cavalry vs cavalry, defender forest |39|40→24|70→60|
| Infantry attacker | Attacker type1 |44|40→26|70→57|
| Infantry defender | Defender type1 |37|40→24|70→61|
| Archer attacker | Attacker type2 |44|40→26|70→57|
| Higher attacker Skill | Skill102→500 |41|40→25|70→59|
| Higher attacker Arms | Arms124→500 |43|40→26|70→58|
| Plain defender | Logical terrain3→2 |34|40→23|70→63|
| Castle defender | Logical terrain3→6 |24|40→20|70→67|
| Last defender soldier | Target70→1 |98|40→39|1→0|

Castle country Defense changed **91→83 before casualty calculation**. Normal casualty draws were14 then3 out of24 in the noncastle captures. The castle case used3 then4; its earlier draw4=2 and draw12=3 were independently obtained by executing the original numeric prelude from the captured inputs, and match the native fortification/share results. The casualty draw values themselves are recorded in native VM context at `$8138`; do not confuse source-prelude execution with an additional native RNG trace.

Canceling at target selection kept all20 unit records, BP/orders, strategic resources and RNG unchanged. It emitted no watched cue request. Eight instrumented attack variants each requested cue41 five times; the first baseline attack preceded creation of that cue watch.

A separate initial movement observation moved unit0 `(7,7)→(8,7)` into forest:40→34 soldiers and Mobility10→6. This is retained as a bounded movement observation, **not** a completed movement-loss formula. Early incorrect menu navigation and all attempts remain in raw evidence.

## 2. Source-to-Windows data contract

Unit count pointer is `$6060 + 11*unitIndex`,20 slots. Relative to that pointer: count word+0; x byte+2; y byte+3; type byte+4; mobility byte+5. The preceding byte is a unit flag; complete removal/visibility meanings remain outside this kernel.

Side records start `$6001 + 27*side`: leader pointer word+0, role byte+2, country byte+3. Side Skill is word `$6037+2*side`; Arms is `$603B+2*side`. Character fields used here are Leadership+0E, Judgment+10, Body+16 and Battle+18. Charm and Planning are not direct inputs to this particular strength helper. This does not say they are irrelevant elsewhere.

Terrain lookup uses logical16×10 coordinates and `$615C + 16*y + x`, masked with`7F`. Numeric labels established here are type0 cavalry,1 infantry,2 archers; terrain2 plain,3 forest,6 castle. Do not assign names to the other terrain columns from guesswork.

The receiving Windows modules are `NormalMelee`, `BattleUnit`, `CharacterView`, `BattleOutcome` and `AudioEventBus`. Use typed game state, not hardcoded ROM addresses, outside a source-data adapter.

## 3. Exact normal-melee arithmetic

All ordinary word arithmetic follows unsigned16 wrap. `wide(a,b,d)` multiplies word inputs in32bits, divides, returns the low word; divisor0 returns`FFFF`. `pct(a,b)=wide(a,b,100)`. Preserve the operation order; do not simplify away wrap or floor.

### Combat strength — bank5 `$8061`

```
skillBucket = floor(Skill/10)+1
armsBucket  = floor(Arms/5)+1
trained = u16(pct(N,100+skillBucket)+pct(N,100+armsBucket)) >> 1
leaderFactor = 1090                         if role==102
             = u16(1000+Battle+(L>>1)+(Body>>2)+(J>>1)) otherwise
strength = u16(trained + wide(N,leaderFactor,1000)) >> 1
```

Role102 is a literal source branch, not an invented named officer class. Role bonus is2 for100,1 for101,0 otherwise. The baseline strength values are44 and84.

### Matchup and defensive multipliers

Attacker type rows / defender type columns0..2:

```
3 5 5
4 3 3
4 4 3
```

Add the attacker's role bonus. The source table is bank5 `$BBE0`.

For defender terrain0..5, use the defender-type row of bank5 `$BBCE`, then add defender role bonus:

```
1 1 3 2 1 5
1 2 3 4 5 5
1 2 3 4 5 5
```

For terrain6, first saturating-subtract from the **original defending side's country Defense**:

```
5 + rand(4) + floor(pct(attackerN,100+attackerArmsBucket)/30)
```

Then defensive multiplier is`3+rand(floor(updatedDefense/7)+1)+defenderRoleBonus`. Do not read pre-damage fortification or automatically choose whichever country's unit is currently receiving the attack.

### Share and casualties — bank5 `$80D4`, `$811E`, `$815F`, `$81E9`

```
A = u16(attackerStrength * attackMultiplier)
D = u16(defenderStrength * defenseMultiplier)
shareA = 50 if A==0 and D==0 else clamp(wide(A,100,u16(A+D)),1,99)
shareD = 100-shareA
```

For each unit, attacker first, take a separate`r=rand(24)`:

```
other = 100-ownShare
scaled = pct(N,87+r)
ratio = wide(scaled,other*other,ownShare*ownShare+other*other)
requestedLoss = u16((ratio>>1)+floor(other/ownShare)+1)
actualLoss = min(N,requestedLoss)
remaining = N-actualLoss
```

The squared terms are **the opposing percentage shares**, not raw troop Skill squared. This formula concerns live normal melee; it does not establish the separate off-screen battle engine. Both casualty calculations use the already computed shares; they are not recomputed from the first reduced headcount.

**Orchestration limit:** the original command calls unit-removal handling at zero counts between stages. The kernel returns numeric zero flags; it does not implement leader capture, battle-end unwinding, ownership changes or survivor recruitment. Do not blindly apply a full two-side state transition when a commander-zero path may invoke unresolved terminal handling. The captured destroyed target was noncommander unit11. G04 remains open.

## 4. Use the executable component

```
from normal_melee import Fighter, normal_melee
result = normal_melee(attacker, defender, game_rng.rand,
                      defending_country_defense=defending_side_country.defense)
```

For noncastle attacks the optional Defense argument may be omitted. The RNG callback must return an integer in`0..n-1`. The module records consumed bounds/values, uses immutable inputs, rejects invalid types/ranges, and returns strength/share/loss/count/fortification information for the caller to commit at the source-defined stage.

Run authored component checks, not a new NES campaign:

```
python -m unittest -v test_normal_melee
python verify_capture_bundle.py /private/unpacked/EXEC0018
```

`native_melee_cases.json` contains9 portable arithmetic fixtures; `source_arithmetic_vectors.json` contains44 additional source-executed vectors. The latter includes synthetic zero/word-limit inputs and role/attribute branches; it is not44 played battles. These checks passed on Linux Python3.13.5. No Windows binary or physical controller run occurred.

## 5. Audio finding collected alongside combat

**Cue41, pointer `$96AC`, is requested during the observed normal-attack animation.** Each of8 watched attacks requested it5 times at22-bridge-frame intervals, before casualties; cancellation requested none. This establishes a new gameplay association, not a new isolated-cue playback sweep. Both Music and Sound were On. Its specific option gate is not newly established here; do not invent one or claim an Off-control capture.

There are now10 entries with gameplay associations, plus structural cue0;41 other cue entries still need caller classification. Known entries may have additional uses/gates. The implemented sequencer is reused unchanged.

## 6. Verification and retained failures

The raw-memory verifier binds **32 SRAM/zero-page snapshot pairs** to456 complete native request/response pairs: initial session108/10 snapshots, attack session348/22 snapshots. The latter records18 declared direct input-memory writes; the initial navigation session had none. No unmatched request or unbound snapshot remains. New component tests reject an incorrect matchup coefficient. Capture checks reject both a changed troop byte with a recomputed metadata hash and a deleted raw response.

The kernel's tests were written and observed failing before the module existed, then passed after implementation. Initial source-probe attempts encountered unsupported CPU operations in an older limited harness; its raw failures are preserved. The existing fuller seasonal-source CPU interpreter was then reused unchanged, with its input guard explicitly pinned to this exact modified ROM. It executes numeric instructions in synthetic memory, not the console. Presentation is excluded from those bounded calls. An initial verification script read the wrong VM local offset; inspection showed A holds the share at`$81BE`, and the corrected checks passed. No earlier failed log was deleted.

New source-helper arithmetic matched all44 vectors; nine native action deltas independently match the module. These are separate checks. Screenshots establish actual menu routes and resulting display, not a claim of newly completed visual/animation fidelity. Castle/type input mutations are disclosed in each fixture.

## 7. Completion register and executable next work

| Item | Status | Next consumer / remaining limit |
|---|---|---|
| Normal-melee numerical kernel | PASS VERIFIED, bounded source + native mod cases | Nobunaga imports into tactical rules |
| Source type/terrain/role and strength formulas | PASS VERIFIED within decoded helpers | Preserve word arithmetic; extend new branches only on mismatch |
| Castle prelude arithmetic | PASS VERIFIED for decoded helper and controlled native case | Real castle scene/terminal outcomes remain separate |
| Attack cancel and noncommander zero target | PASS VERIFIED for captured routes | Do not infer commander-death outcome |
| Cue41 gameplay association | PARTIAL | Association observed; option gate/additional uses still to classify |
| General movement, arrows, duel, ambush, other battle commands | PARTIAL / NOT RUN as individually tracked | Next G03: legal movement and ranged/special command handlers |
| Victory/defeat/capture/auto-resolve/combat AI | NOT CLOSED | G04; no repeat early-retreat/save gate |
| Full Windows tactical integration | NOT RUN | Integrate this component with actual target selection, renderer and outcomes |

**Nobunaga:** implement this exact normal-melee component and event hook; consume known economy/approved mods in parallel as the master directs. **Cao Cao:** compare the new Windows function to these numeric fixtures; challenge stage ordering and terminal integration, not settled old NES research. **Zhuge Liang:** continue unresolved legal movement/ranged/special attacks and G04 outcomes, collecting new audiovisual triggers as they occur. All remaining G01–G12 work stays in the single master guide.

Publication receipt and verified Drive/GitHub links are recorded separately; this report does not claim recipient acknowledgment or entire-game completion.

**End of EXEC0018.**
