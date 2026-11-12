#ifndef GK1_APP_STATE_H
#define GK1_APP_STATE_H
#include "../execution_039/gk1_battle_session.h"
#include <stdint.h>
#include "../execution_045/gk1_observed_turn.h"
#include "../execution_047/gk1_divide_contract.h"
#include "gk61_market.h"
#include "gk64_manual_rules.h"
#include "gk65_battle_terminal.h"
#include "gk68_observed_rest.h"
#include "gk71_order_budget.h"
#include "gk72_battle_pass.h"
#include "gk75_spring_economy.h"
#include "gk110_spring_stage.h"
#include "gk111_stock_spring_audit.h"
#include "gk112_stock_setup_deltas.h"
#include "gk212_stock_raw_variation.h"
#include "gk213_stock_spring_boundary.h"
#include "gk214_spring_batch.h"
#include "gk222_enhanced_spring_lab.h"
#include "gk217_finance_preview.h"
#include "gk219_finance_compare.h"
#include "gk220_labor_history.h"
#include "gk216_fresh_cpu_pc.h"
#include "gk114_stock_spring_timeline.h"
#include "gk211_fresh_sequence.h"
#include "gk79_stock_checkpoint.h"
#include "gk116_enhanced_assign.h"
#include "gk187_classic_assign.h"
#include "gk158_enhanced_summer.h"
#include "gk159_enhanced_fall.h"
#include "gk160_enhanced_winter.h"
#include "gk161_enhanced_year2.h"
#include "gk176_enhanced_cycle.h"
#include "gk119_order_bridge.h"
#include "gk137_stock_rng.h"
#include "gk138_rng_reverse.h"
#include "gk139_mesen_state.h"
#include "gk140_rng_lineage.h"
#include "gk141_dispatch_scan.h"
#include "gk142_fixed_cfg.h"
#include "gk143_bank_cfg.h"
#include "gk144_archived_mapper.h"
#include "gk145_fresh_progress.h"
#include "gk146_assign_quantity.h"
#include "gk147_stock_goods.h"
#include "gk148_fresh_bank_trace.h"
#include "gk149_ppu_phase_scan.h"
#include "gk150_render_window.h"
#include "gk151_even_source.h"
#include "gk153_country_write_watch.h"
#include "gk172_rng_source_xref.h"
#include "gk178_source_rng.h"
#include "gk180_seed_writer_scan.h"
#include "gk181_bank_seed_trace.h"
#include "gk182_archived_source_bank.h"
#include "gk202_irq_rng_dispatch.h"
#include "gk203_rng_selector_writers.h"
#include "gk204_selector_store_inventory.h"
#include "gk152_spring_writer_map.h"
#include "../execution_049/gk1_scenario_factory.h"
/* A standalone, copyright-clean native-app vertical slice, NOT a shipped game.
 * Campaign buttons are navigation only: no campaigns are fabricated.
 * Approved combined modified-ROM one-cell checkpoint cases only.
 */
typedef enum {GK1_SCREEN_TITLE=0, GK1_SCREEN_CAMPAIGNS=1,
 GK1_SCREEN_RESEARCH_BATTLE=2, GK1_SCREEN_UNIMPLEMENTED=3, GK1_SCREEN_PATH_PICK=4, GK1_SCREEN_PATH_BATTLE=5, GK1_SCREEN_DIVIDE_PICK=6, GK1_SCREEN_DIVIDE_BATTLE=7, GK1_SCREEN_SCENARIO_COUNTRIES=8, GK1_SCREEN_SETUP_WIZARD=9, GK1_SCREEN_LABOR_PLAN=10,GK1_SCREEN_DEMOBILIZE=11, GK1_SCREEN_MARKET_EXPLORER=12,
 GK1_SCREEN_MANUAL_RULES=13, GK1_SCREEN_BATTLE_TIMELINE=14,GK1_SCREEN_REST_CASE=15,GK1_SCREEN_ORDER_BUDGET_RESEARCH=32,GK1_SCREEN_BATTLE_PASS_DRILL=16,GK1_SCREEN_SPRING_ARCHIVE=33,GK1_SCREEN_STOCK_CONTINUE=34,GK1_SCREEN_STOCK_COMMANDS=35,GK1_SCREEN_STOCK_TRADE_REFERENCE=36,GK1_SCREEN_ENHANCED_ASSIGN=37,GK1_SCREEN_ENHANCED_SUMMER=38,GK1_SCREEN_ENHANCED_FALL=39,GK1_SCREEN_ENHANCED_WINTER=40,GK1_SCREEN_ENHANCED_YEAR2=41, GK1_SCREEN_STOCK_STORAGE=42,GK1_SCREEN_ENHANCED_CONTINUATION=43,GK1_SCREEN_CLASSIC_ASSIGN=44,GK1_SCREEN_CLASSIC_TAX=45,GK1_SCREEN_SPRING_FINANCE_PREVIEW=46,GK1_SCREEN_ENHANCED_SPRING_LAB=47} Gk1Screen;
typedef enum {GK1_INPUT_UP=1, GK1_INPUT_DOWN=2, GK1_INPUT_LEFT=3,
 GK1_INPUT_RIGHT=4, GK1_INPUT_CONFIRM=5, GK1_INPUT_CANCEL=6, GK1_INPUT_SETUP=7, GK1_INPUT_DRAFT_UNDO_ONE=8,
 GK1_INPUT_DRAFT_UNDO_TEN=9, GK1_INPUT_LABOR_PLAN=10} Gk1Input;
typedef enum {GK53_SETUP_PLAYERS=0, GK53_SETUP_DIFFICULTY=1,
 GK53_SETUP_LORD=2, GK53_SETUP_ABILITIES=3, GK53_SETUP_AWAIT_SOURCE=4} Gk53SetupStage;
/* Source-verified NES manual: single Mongol player, 1-4 World players,
 * 1-5 difficulty, four World selectable lords, manual/auto abilities.
 * These are setup INTENT; original stat randomization and seasonal init OPEN.
 */
int gk53_setup_allowed_lord(uint8_t world,uint8_t country_index);
uint8_t gk53_setup_country_for_slot(uint8_t world,uint8_t slot);
typedef struct {
  Gk1Screen screen;
  uint8_t menu_index, case_index;
  uint8_t has_session;
  Gk1BattleSession session;
  Gk1MoveResult last_move;
  Gk45Turn path;
  uint8_t path_case;   /* 0 raw7, 1 controlled raw2, 2 controlled ally40 */
  uint8_t cursor_x,cursor_y;
  uint8_t path_events[4]; /* 1 step1, 2 step2, 3 B */
  uint8_t path_event_count;
  Gk47Divide divide;
  uint8_t divide_choice,divide_cursor_x,divide_cursor_y;
  uint8_t divide_events[8],divide_event_count;
  Gk1LoadedScenario worlds[2]; /* independent mutable PRE_SETUP copies */
  Gk1LoadedScenario finance_source_baseline[2]; /* immutable original CSV loaded for controller-only planning comparison */
  uint8_t world_ready[2],country_cursor[2],active_world;
  uint8_t setup_candidate[2]; /* 0xFF unset; INDEX intent ONLY, not NES player setup */
  uint8_t rules_profile; /* 0 stock Classic, 1 approved Enhanced; separate oracles */
  uint8_t setup_player_count[2]; /* 1 Mongol; 1..4 World */
  uint8_t setup_difficulty[2]; /* NES manual: 1..5 */
  uint8_t setup_stage[2]; /* stages do NOT imply original game reached Spring */
  uint8_t setup_player_slot[2]; /* current player selecting a lord */
  uint8_t setup_rulers[2][4]; /* country index, 255 means unset */
  uint8_t setup_ability_method[2]; /* 0 undecided, 1 automatic, 2 manual */
  uint8_t setup_method_cursor[2]; /* 0 automatic, 1 manual */
  /* EXEC0055: controller-entered proposed manual ability point distribution.
   * Manual p.8-9 confirms SIX abilities and LORD+FOUR PRINCES.
   * These are additional-point draft quantities ONLY, never base abilities or
   * NES-authoritative initialized stats. No season/ruler transform implied. */
  uint8_t manual_bonus_active[2];
  uint8_t manual_bonus_person[2];
  uint8_t manual_bonus_ability[2];
  uint8_t manual_bonus[2][5][6];
  uint8_t labor_from, labor_to; /* EXEC0057 PRE_SETUP planner cursor ONLY */
  Gk222Spring port_spring; /* Enhanced-only FINANCE PRACTICE; never Classic/original state */
  uint8_t port_spring_command; /* PASS or PORT-only worker move */
  Gk220History labor_history; /* EXEC0220 PORT-only reversible workforce edits; never NES orders */
  uint8_t finance_armed; /* first Classic START keeps prior blocked contract */
  uint8_t finance_cursor; /* EXEC0217 Classic source-bounded finance; PRE_SETUP only */
  uint8_t finance_compare; /* EXEC0218 A toggles original template vs PORT labor-planning delta */
  uint8_t market_merchant,market_item,market_sell;
  uint16_t example_rate; /* illustrative only, NOT original merchant Rate */
  uint8_t spring_stock_mode; /* EXEC0077 validated external STOCK read-only source profile */
  uint8_t spring_stock_count; /* 0 until exact hash-validated private record load */
  uint8_t spring_preview_mode; /* EXEC0075 original-source bounded what-if; never mutates country */
  uint8_t spring_archive_cursor; /* read-only 14-country archived capture; not Spring init */
  uint8_t manual_remaining_orders; /* manual battle window 1..3; NOT player turns */
  Gk65Terminal battle_timeline;
  Gk72BattlePass battle_pass; /* manual p31 Pass drill, not source unit/RNG */
  Gk68RestCase rest_case; /* isolated observed MOD checkpoint, not campaign */
  Gk79Checkpoint stock_checkpoint;
  Gk116EnhancedAssign enhanced_assign; /* distinct Enhanced mutable world */
  Gk187ClassicAssign classic_assign; /* manual-derived Classic, source separate */
  Gk158EnhancedSummer enhanced_summer; /* sequential Enhanced-only Summer; original source untouched */
  Gk159EnhancedFall enhanced_fall; /* sequential Enhanced-only Fall, NO original harvest */
  Gk160EnhancedWinter enhanced_winter; /* 4th ENHANCED labor-only season; no NES winter effects */
  Gk161EnhancedYear2 enhanced_year2; /* next Enhanced Spring; NOT original NES annual effects */
  Gk176EnhancedCycle enhanced_continuation; /* subsequent 20 Enhanced labor-only seasons; NO NES economy */
  uint8_t classic_tax_selection; /* manual-derived 1..100, no source ability cost parity */
  uint8_t assign_from,assign_to; /* exact source validated by gk77_load, not game startup */
  uint16_t assign_quantity; /* Enhanced-only source-attested requested labor amount */
  uint8_t stock_reconcile_view; /* VIEW cycles decoded / raw / finance / deltas / source goods */
  uint8_t stock_trade_merchant,stock_trade_item,stock_trade_sell; /* source-rate quote research only */
  uint8_t stock_command_cursor; /* authentic-stock 14-command controller menu (Pass/View only) */
  uint8_t stock_storage_cursor; /* PORT-ONLY command 13 OTHER: 0 save, 1 load, 2 return */
  uint8_t stock_storage_confirm; /* two A presses required, B cancels */
  uint8_t stock_storage_request; /* one-shot for platform shell: 1 save, 2 load */
  Gk71Budget order_budget_research; /* MANUAL-only, never live order in PRE_SETUP */
  /* EXEC0066: explicit manual what-if event selector, not ROM-derived gameplay. */
  uint8_t whatif_event,whatif_victim; /* event0..6, victim1=attacker 2=defender */
  uint16_t demob_quantity; /* EXEC0060 Enhanced-only PRE_SETUP preview, not season */
  char status[128];
} Gk1App;
void gk1_app_init(Gk1App *s);
Gk222Status gk1_app_enhanced_spring_lab_open(Gk1App *s);
/* Fresh ORIGINAL Mesen CPU sample PCs, full exact-stock and 14-stage SHA gated.
 * Read-only diagnostic; setup remains AWAIT_SOURCE, cannot award orders. */
Gk216Status gk1_app_original_fresh_cpu_pc(const Gk1App *app,
 const char *verified_csv,const char *stock_rom,
 const char *private_dir,Gk216CpuReport *out);
/* SOURCE-BOUNDED multination finance batch for an already installed world.
 * The native app never accepts this as setup evidence or grants a turn.
 * Original 14/27 PRE_SETUP templates remain unchanged on success/failure. */
Gk214Status gk1_app_spring_batch_projection(const Gk1App *app,
 uint8_t world_index,Gk214Batch *out);

/* Aggregate read-only comparison of two ORIGINAL, separately authenticated
 * Spring1175 histories. No source records embedded, no gameplay mutation. */
Gk212Status gk1_app_original_raw_variation(const Gk1App *app,
 const char *canonical_csv,const Gk79Checkpoint *fresh,
 const Gk79Checkpoint *archived,Gk212Variation *out);
/* Validates the real fresh original 14-stage owner-private evidence and
 * observes one pre-growth human first-Spring finance boundary in the EXISTING
 * application without mutating its countries or granting orders. Does NOT
 * derive the underlying setup/RNG, generalize other countries, or unlock
 * game phase. Original source records remain private. */
Gk213Status gk1_app_observed_first_spring_boundary(const Gk1App *,
 const char *canonical_csv,const char *private_fresh_dir,Gk213Boundary *out);
/* Exact-original BRK-vector jump-table RNG source proof, NOT actual setup. */
Gk202Status gk1_app_source_irq_rng_path(const Gk1App *app,
 const char *private_stock_rom,Gk202Proof *out);
/* Source-only static callsite inspection; no active campaign mutation. */
Gk203Status gk1_app_source_rng_selector_writers(const Gk1App *app,
 const char *private_stock_rom,Gk203Proof *out);
/* Exact archived Mesen source/SHA mapper+seed captured at a SAVED instant.
 * Only callable in pristine setup; no Spring/Classic orders generated. */
Gk182Status gk1_app_archived_source_bank(const Gk1App *app,
 const char *private_rom,const char *private_mss,Gk77Profile profile,
 Gk182SavedBankProof *out);
/* Canonical AWAIT_SOURCE pre-setup only. Runs one authentic fixed-bank RNG arithmetic primitive
 * with CALLER-EXPLICIT seed; never claims to generate Spring RNG history. */
Gk181Result gk1_app_bank_seed_trace(const Gk1App *app,
 const char *private_stock_rom,uint8_t hypothesized_bank,Gk181Map *out);
Gk180Status gk1_app_source_seed_writers(const Gk1App *app,
 const char *private_stock_rom,Gk180WriterMap *out);
Gk178Status gk1_app_source_rng_step(const Gk1App *app,
 const char *private_rom,const uint8_t seed[6],Gk178RngResult *out);
/* Original-cartridge setup/RNG static source inspection only, guarded to
 * canonical PRE_SETUP. Never promotes a real Spring or spends an order. */
Gk172Status gk1_app_source_rng_xref(const Gk1App *app,
 const char *private_rom,Gk172RngXref *out);
/* EXEC0204: exact-stock raw selector store candidates; does not execute NES. */
Gk204Status gk1_app_source_selector_store_inventory(const Gk1App *,
 const char *private_stock_rom,Gk204Inventory *out);
/* Source-instruction-level PRG-RAM country-store watch. Opt-in even-frame
 * hardware hypothesis only, not original Mesen and NOT gameplay. */
typedef enum { GK153_TRACE_BAD_STAGE=0, GK153_TRACE_BAD_SOURCE=1,
 GK153_TRACE_BOUNDED=2 } Gk153TraceResult;
/* Explicit PPU VBlank-edge counterfactual: no campaign mutation or unlock.
 * Only original SHA-authenticated private ROM and PRE_SETUP stage accepted. */
/* GK156 all-128 counterfactual audit is read-only, PRE_SETUP locked.
 * Even 128 equal variants cannot establish original parity without an
 * actual sign-positive read; see result.all_identical. */
Gk153TraceResult gk1_app_source_symbolic_vblank_exit(const Gk1App *app,
 const char *private_rom,uint32_t origin,uint8_t choice,Gk83Trace *out);
Gk153TraceResult gk1_app_source_sign_exit_matrix(const Gk1App *app,
 const char *rom,uint32_t origin,uint8_t choice,Gk156StatusMatrix *out);
Gk153TraceResult gk1_app_source_sign_only_poll(const Gk1App *app,
 const char *private_rom,uint32_t origin,uint8_t first_race,Gk83Trace *out);
Gk153TraceResult gk1_app_source_one_race_branch(const Gk1App *app,
 const char *private_rom,uint32_t origin,uint8_t branch_choice,Gk83Trace *out);
Gk153TraceResult gk1_app_source_country_writer_trace(const Gk1App *app,
 const char *private_rom,Gk83Trace *out);
/* Authentication required, never advances seasons or grants original orders. */
Gk152Status gk1_app_source_spring_writer_map(const Gk1App *app,
 const char *private_stock_rom,Gk152WriterMap *out);
/* GK151 source-only even parity hypothesis; never mutates native world. */
Gk151Status gk1_app_source_even_prerender(const Gk1App *app,
 const char *private_rom,Gk151Report *out);
/* Research-only progressive source bounded PPU render-window analysis.
 * Real original first Spring and all 3 original orders remain unimplemented. */
Gk150Status gk1_app_source_render_window(const Gk1App *app,
 const char *private_rom,Gk150Report *out);
/* Source-validated, phase-hypothesis read-only boot audit, not NES gameplay. */
Gk149Status gk1_app_source_ppu_phase_scan(const Gk1App *app,
 const char *private_stock_rom,Gk149PhaseScan *out);
/* Read-only analysis of independently authenticated original fresh reset event snapshots.
 * Does not initialize native Spring, populate RNG seeds or grant any orders. */
Gk145Status gk1_app_source_fresh_progress(const Gk1App *app,
 const Gk145FreshObservation *observations,size_t count,
 uint16_t original_year,uint16_t countries,Gk145FreshProgress *out);

/* Research-only original RATE RNG: caller provides explicit six-byte seed.
 * Only accepts a real setup intent at AWAIT_SOURCE; does NOT mutate app,
 * manufacture original seed/NMI timing, or mark first Spring playable. */
Gk137Result gk1_app_source_rate_proof(const Gk1App *app,
 const uint8_t explicit_nes_seed[6],Gk137StockRng *after,uint16_t *rate);
/* Explicit observed post-setup seed and RATE, NOT an inferred stock
 * RNG seed. Source-only backwards compatibility scan with unchanged app.
 * No mutation or Spring/Trade unlock on success or refusal. */
Gk138Result gk1_app_source_rate_history(const Gk1App *app,
 const uint8_t post_seed[6],uint16_t observed_rate,uint32_t max_draws_back,
 Gk138RateScan *out);
/* Research-only, read-only archived MesenCE named-state proof. The caller
 * must separately authenticate the originating private stock savestate,
 * original ROM and campaign branch. No snapshot grants a live turn. */
Gk139Status gk1_app_source_mesen_state_probe(const Gk1App *app,
 const uint8_t *uncompressed_named_state,size_t state_len,Gk139MesenProof *out);
/* Read-only, bounded source-PRNG lineage test between two owner-authenticated
 * Mesen saved-state observations; compatibility is NOT original draw count.
 * No setup advancement, turn grant, or mutation on success/refusal. */
Gk140Result gk1_app_source_rng_lineage(const Gk1App *app,
 const Gk139MesenProof *earlier,const Gk139MesenProof *later,
 uint32_t maximum_draws,Gk140Lineage *out);
/* Exactly source-bound, read-only ten specialty goods for the active original
 * 14/27-country checkpoint; this does not establish Trade or spawn Spring. */
Gk147GoodsStatus gk1_app_source_goods_view(const Gk1App *app,
 unsigned country_index,Gk147StockGoods *out);
/* Archived Mesen PRG-window observation, NEVER original boot-bank proof. */
Gk144Status gk1_app_source_archived_mapper(const Gk1App *app,
 const uint8_t *uncompressed_named_state,size_t bytes,Gk144ArchivedMapper *out);
/* Explicit, unverified PRG boot-bank hypothesis. No turns or state writes. */
Gk141Result gk1_app_source_bank_cfg(const Gk1App *app,
 const char *private_stock_rom_path,uint8_t assumed_bank,Gk143BankCfg *out);
/* Fixed-bank CFG is a read-only lower-bound on reachable ROM code,
 * not actual Mesen execution, dynamic setup or a Spring turn. */
Gk141Result gk1_app_source_fixed_cfg(const Gk1App *app,
 const char *private_stock_rom_path,Gk142FixedCfg *out);
Gk141Result gk1_app_source_dispatch_scan(const Gk1App *app,
 const char *private_stock_rom_path,Gk141DispatchMap *out);
/* Atomic install of both validated scenario templates: no auto-stat or Spring. */
int gk1_app_install_templates(Gk1App *s,const Gk1TemplateDataset *source);
const Gk1CountryTemplate *gk1_app_selected_country(const Gk1App *s);

/* All-country source-bounded partial Spring finance for the same native app.
 * No world mutation or setup/season unlock on either success or failure. */
Gk110Result gk1_app_source_spring_stage(const Gk1App *app,
 const char *verified_csv,Gk110SpringStage *out);
/* All-country read-only certified Spring mismatch audit from attached stock
 * source. Never changes setup, grants orders, or forecasts RNG. */
Gk111Result gk1_app_stock_spring_audit(const Gk1App *app,
 const char *verified_csv,Gk111StockSpringAudit *out);
/* AUTHENTIC original checkpoint full-field setup+season residuals; readonly. */
Gk112Status gk1_app_stock_setup_deltas(const Gk1App *app,
 const char *verified_csv,Gk112StockDeltas *out);
/* Six-stage ORIGINAL stock provenance, read-only. Not playable state. */
Gk148Result gk1_app_original_fresh_bank_trace(const Gk1App *app,
 const char *private_fresh_events,Gk148FreshTimeline *out);
Gk114Status gk1_app_original_fresh_timeline(const Gk1App *app,
 const char *verified_csv,const char *private_96bytes,Gk114Timeline *out);
/* Authenticated original fresh stock source timeline. Does not unlock gameplay. */
Gk211Status gk1_app_original_fresh_sequence(const Gk1App *app,
 const char *verified_csv,const char *private_dir,Gk211Report *out);
/* 1 indicates input handled; 0 rejects null/invalid. All paths controller reachable. */
int gk1_app_input(Gk1App *s,Gk1Input command);
const char *gk1_app_case_label(unsigned i);
/* Atomic state transition from a validated, replayable receipt.
 * Does not claim a complete NES campaign save. */
int gk1_app_restore_session(Gk1App *s,const Gk1BattleSession *loaded);
/* Restore from an exact validated source-event script, atomic to the app. */
int gk1_app_restore_path(Gk1App *s,uint8_t scenario,const uint8_t *events,uint8_t count);
/* Validated source-event replay of actual Divide controller phases, no campaigns. */
int gk1_app_restore_divide(Gk1App *s,const uint8_t *events,uint8_t count,
                           uint8_t selected,uint8_t cursor_x,uint8_t cursor_y);
/* G11 exact arithmetic is retained, but EXEC0115 rejects all app-level
 * transactions without an independently authenticated original merchant
 * event (none captured yet), even when a caller forges phase 2/offers. */
const char *gk87_stock_command_name(unsigned command);
Gk61Result gk1_app_trade(Gk1App *app,Gk61Actor *actor,
                         const Gk61Market *market,uint8_t country_one_based,
                         Gk61Merchant merchant,Gk61Action action,uint8_t item,
                         uint16_t quantity,int hire_morale_roll11);
#endif
