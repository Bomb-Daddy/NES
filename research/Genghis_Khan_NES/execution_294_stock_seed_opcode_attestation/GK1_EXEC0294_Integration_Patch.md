--- a/execution_199/CMakeLists.txt
+++ b/execution_199/CMakeLists.txt
@@ -16,7 +16,7 @@
 if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
   add_compile_options(-Wall -Wextra -Wpedantic -Werror)
 endif()
-add_library(gk1_app_state STATIC gk247_receipt_file_guard.c gk241_archived_import_capability.c gk237_owner_fresh_win.c gk61_market.c gk64_manual_rules.c gk65_battle_terminal.c gk68_observed_rest.c gk74_spring_archive.c gk75_spring_economy.c gk110_spring_stage.c gk111_stock_spring_audit.c gk112_stock_setup_deltas.c gk212_stock_raw_variation.c gk213_stock_spring_boundary.c gk214_spring_batch.c gk222_enhanced_spring_lab.c gk230_win32_smoke_args.c gk224_practice_save.c gk225_practice_resume.c gk226_practice_checkpoint.c gk227_practice_title_preview.c gk217_finance_preview.c gk219_finance_compare.c gk220_labor_history.c gk215_original_spring_gap.c gk278_original_spring_coverage.c gk280_original_rate_bridge.c gk288_original_rng_stream.c gk282_stock_rng_trace.c gk216_fresh_cpu_pc.c gk218_stock_write_trace.c gk114_stock_spring_timeline.c gk115_market_gate.c gk116_enhanced_assign.c gk158_enhanced_summer.c gk159_enhanced_fall.c gk160_enhanced_winter.c gk161_enhanced_year2.c gk176_enhanced_cycle.c gk177_enhanced_receipt.c gk178_source_rng.c gk180_seed_writer_scan.c gk181_bank_seed_trace.c gk182_archived_source_bank.c gk183_checkpoint_isolation.c gk185_stock_hotkey.c gk187_classic_assign.c gk188_classic_receipt.c gk189_classic_cold_save.c gk162_original_entry.c gk164_mss_entry.c gk165_fresh_json.c gk166_fresh_entry.c gk193_auto_source_entry.c gk194_classic_storage_gate.c gk195_classic_view.c gk196_classic_trade_status.c gk198_tax_forecast.c gk199_classic_tax_undo.c gk202_irq_rng_dispatch.c gk203_rng_selector_writers.c gk204_selector_store_inventory.c gk205_selector_runtime_watch.c gk206_rng_source_parity.c gk207_seed_write_watch.c gk208_archived_rng.c gk209_archived_rate_ambiguity.c gk211_fresh_sequence.c gk167_private_cold_save.c gk170_slot_preview.c gk171_enhanced_title_resume.c gk172_rng_source_xref.c gk173_rng_control.c gk174_setup_boundary.c gk119_order_bridge.c gk117_stock_pristine_gate.c gk77_stock_spring.c gk78_spring_world.c gk79_stock_checkpoint.c gk80_full_checkpoint.c gk81_stock_attach.c gk82_stock_cartridge.c gk83_reset_trace.c gk105_openbus.c gk106_ppu_bus.c gk107_ppu_ports.c gk108_cpu_rmw_bus.c gk109_rmw_families.c gk121_bit_flags.c gk122_cpu_logic.c gk123_cpu_loadstore.c gk124_cpu_compare.c gk125_cpu_arith.c gk126_io_barrier.c gk127_io_transaction.c gk128_oam.c gk129_dma_source.c gk130_dma_bus.c gk131_boot_gate.c gk132_f5_setup.c gk133_boot_order.c gk134_stock_command_route.c gk137_stock_rng.c gk138_rng_reverse.c gk139_mesen_state.c gk140_rng_lineage.c gk141_dispatch_scan.c gk142_fixed_cfg.c gk143_bank_cfg.c gk144_archived_mapper.c gk145_fresh_progress.c gk146_assign_quantity.c gk147_stock_goods.c gk148_fresh_bank_trace.c gk149_ppu_phase_scan.c gk150_render_window.c gk151_even_source.c gk152_spring_writer_map.c gk153_country_write_watch.c gk94_prgram.c gk98_controller_port.c gk98_cpu_rotate.c gk100_cpu_cycles.c gk101_mmc1_cycles.c gk103_ntsc_ppu.c gk95_stock_trade_reference.c gk96_ppu_warmup.c gk91_reset_zero.c gk92_ppustatus_hypothesis.c gk89_spring_reconcile.c gk90_verified_finance.c gk71_order_budget.c gk72_battle_pass.c ../execution_049/gk1_scenario_factory.c ../execution_045/gk1_observed_turn.c ../execution_047/gk1_divide_contract.c gk1_app_state.c gk1_source_identity.c ../execution_039/gk1_battle_session.c ../execution_034/gk1_tactical_slice.c)
+add_library(gk1_app_state STATIC gk247_receipt_file_guard.c gk241_archived_import_capability.c gk237_owner_fresh_win.c gk61_market.c gk64_manual_rules.c gk65_battle_terminal.c gk68_observed_rest.c gk74_spring_archive.c gk75_spring_economy.c gk110_spring_stage.c gk111_stock_spring_audit.c gk112_stock_setup_deltas.c gk212_stock_raw_variation.c gk213_stock_spring_boundary.c gk214_spring_batch.c gk222_enhanced_spring_lab.c gk230_win32_smoke_args.c gk224_practice_save.c gk225_practice_resume.c gk226_practice_checkpoint.c gk227_practice_title_preview.c gk217_finance_preview.c gk219_finance_compare.c gk220_labor_history.c gk215_original_spring_gap.c gk278_original_spring_coverage.c gk280_original_rate_bridge.c gk288_original_rng_stream.c gk294_seed_writer_attest.c gk282_stock_rng_trace.c gk216_fresh_cpu_pc.c gk218_stock_write_trace.c gk114_stock_spring_timeline.c gk115_market_gate.c gk116_enhanced_assign.c gk158_enhanced_summer.c gk159_enhanced_fall.c gk160_enhanced_winter.c gk161_enhanced_year2.c gk176_enhanced_cycle.c gk177_enhanced_receipt.c gk178_source_rng.c gk180_seed_writer_scan.c gk181_bank_seed_trace.c gk182_archived_source_bank.c gk183_checkpoint_isolation.c gk185_stock_hotkey.c gk187_classic_assign.c gk188_classic_receipt.c gk189_classic_cold_save.c gk162_original_entry.c gk164_mss_entry.c gk165_fresh_json.c gk166_fresh_entry.c gk193_auto_source_entry.c gk194_classic_storage_gate.c gk195_classic_view.c gk196_classic_trade_status.c gk198_tax_forecast.c gk199_classic_tax_undo.c gk202_irq_rng_dispatch.c gk203_rng_selector_writers.c gk204_selector_store_inventory.c gk205_selector_runtime_watch.c gk206_rng_source_parity.c gk207_seed_write_watch.c gk208_archived_rng.c gk209_archived_rate_ambiguity.c gk211_fresh_sequence.c gk167_private_cold_save.c gk170_slot_preview.c gk171_enhanced_title_resume.c gk172_rng_source_xref.c gk173_rng_control.c gk174_setup_boundary.c gk119_order_bridge.c gk117_stock_pristine_gate.c gk77_stock_spring.c gk78_spring_world.c gk79_stock_checkpoint.c gk80_full_checkpoint.c gk81_stock_attach.c gk82_stock_cartridge.c gk83_reset_trace.c gk105_openbus.c gk106_ppu_bus.c gk107_ppu_ports.c gk108_cpu_rmw_bus.c gk109_rmw_families.c gk121_bit_flags.c gk122_cpu_logic.c gk123_cpu_loadstore.c gk124_cpu_compare.c gk125_cpu_arith.c gk126_io_barrier.c gk127_io_transaction.c gk128_oam.c gk129_dma_source.c gk130_dma_bus.c gk131_boot_gate.c gk132_f5_setup.c gk133_boot_order.c gk134_stock_command_route.c gk137_stock_rng.c gk138_rng_reverse.c gk139_mesen_state.c gk140_rng_lineage.c gk141_dispatch_scan.c gk142_fixed_cfg.c gk143_bank_cfg.c gk144_archived_mapper.c gk145_fresh_progress.c gk146_assign_quantity.c gk147_stock_goods.c gk148_fresh_bank_trace.c gk149_ppu_phase_scan.c gk150_render_window.c gk151_even_source.c gk152_spring_writer_map.c gk153_country_write_watch.c gk94_prgram.c gk98_controller_port.c gk98_cpu_rotate.c gk100_cpu_cycles.c gk101_mmc1_cycles.c gk103_ntsc_ppu.c gk95_stock_trade_reference.c gk96_ppu_warmup.c gk91_reset_zero.c gk92_ppustatus_hypothesis.c gk89_spring_reconcile.c gk90_verified_finance.c gk71_order_budget.c gk72_battle_pass.c ../execution_049/gk1_scenario_factory.c ../execution_045/gk1_observed_turn.c ../execution_047/gk1_divide_contract.c gk1_app_state.c gk1_source_identity.c ../execution_039/gk1_battle_session.c ../execution_034/gk1_tactical_slice.c)
 target_include_directories(gk1_app_state PUBLIC . ../execution_039 ../execution_034 ../execution_045 ../execution_047 ../execution_049)
 add_library(gk1_pre_setup_store STATIC gk1_pre_setup_store.c)
 add_library(gk1_setup_draft_store STATIC gk1_setup_draft_store.c)
@@ -1531,3 +1531,10 @@
 add_test(NAME GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback
  COMMAND test_gk288_original_rng_stream
  ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
+
+# EXEC0294: verified ROM-store candidate cross-check — not an NES runtime run.
+add_executable(test_gk294_seed_writer_attest test_gk294_seed_writer_attest.c)
+target_link_libraries(test_gk294_seed_writer_attest PRIVATE gk1_app_state)
+add_test(NAME GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals
+ COMMAND test_gk294_seed_writer_attest
+ ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
--- a/execution_199/gk1_app_state.c
+++ b/execution_199/gk1_app_state.c
@@ -2265,3 +2265,28 @@
  return gk288_calculate_original_stream(rom,seed,
    app->setup_difficulty[app->active_world],draws,out);
 }
+
+/* EXEC0294 — authenticated raw STORE source candidates, stage-gated using
+ * the SAME non-playing boundary as EXEC0288. Separate from real Mesen CPU
+ * execution, Spring economy initialization, and controller turn admission. */
+Gk294Status gk1_app_attest_seed_writer_source(const Gk1App *app,
+ const char *private_rom,const Gk294Write *claimed,size_t count,
+ Gk294SourceProof *out){
+ if(!app||app->active_world>1u||app->rules_profile!=0u||
+    !app->world_ready[app->active_world]||
+    app->worlds[app->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||
+    app->worlds[app->active_world].scenario_id!=(app->active_world?
+        GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
+    app->worlds[app->active_world].country_count!=(app->active_world?27u:14u)||
+    memcmp(&app->worlds[app->active_world],
+      &app->finance_source_baseline[app->active_world],
+      sizeof app->worlds[app->active_world])!=0||
+    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
+    app->setup_difficulty[app->active_world]<1u||
+    app->setup_difficulty[app->active_world]>5u||
+    app->stock_checkpoint.valid||app->enhanced_assign.active||
+    app->enhanced_summer.active||app->enhanced_fall.active||
+    app->enhanced_winter.active||app->enhanced_year2.active||
+    app->enhanced_continuation.active)return GK294_STAGE;
+ return gk294_attest_seed_writers(private_rom,claimed,count,out);
+}
--- a/execution_199/gk1_app_state.h
+++ b/execution_199/gk1_app_state.h
@@ -21,6 +21,7 @@
 #include "gk280_original_rate_bridge.h"
 #include "gk282_stock_rng_trace.h"
 #include "gk288_original_rng_stream.h"
+#include "gk294_seed_writer_attest.h"
 #include "gk222_enhanced_spring_lab.h"
 #include "gk217_finance_preview.h"
 #include "gk219_finance_compare.h"
@@ -157,6 +158,12 @@
 } Gk1App;
 /* Source-verified contiguous C289 hypothetical draws from explicit CPU seed.
  * Partial original RNG generator, NOT first Spring setup or a turn unlock. */
+/* EXEC0294: authentic-ROM STORE-byte attestation at pristine Classic setup.
+ * This is SOURCE_STATIC candidate evidence, not an executed Mesen trace.
+ * Never alters app or awards Spring orders. */
+Gk294Status gk1_app_attest_seed_writer_source(const Gk1App *app,
+ const char *private_rom,const Gk294Write *claimed,size_t count,
+ Gk294SourceProof *out);
 Gk288Status gk1_app_original_rng_stream(const Gk1App *app,
  const char *private_rom,const uint8_t observed_seed[6],
  unsigned draws,Gk288Stream *out);
--- /dev/null
+++ b/execution_199/gk294_seed_writer_attest.c
@@ -0,0 +1,78 @@
+#include "gk294_seed_writer_attest.h"
+#include "gk1_source_identity.h"
+#include <stdio.h>
+#include <stdlib.h>
+#include <string.h>
+#define GK294_STOCK_LEN 262160u
+#define GK294_PRG_BYTES 16384u
+/* Fingerprint is public metadata. No copyrighted program data embedded. */
+static const uint8_t expected_sha[32]={
+ 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
+ 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
+ 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
+ 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
+};
+static Gk294Status read_authenticated_rom(const char *path,uint8_t **out){
+ FILE *f=fopen(path,"rb");if(!f)return GK294_ROM_IO;
+ uint8_t *bytes=malloc(GK294_STOCK_LEN+1u);
+ if(!bytes){fclose(f);return GK294_ROM_IO;}
+ size_t n=fread(bytes,1,GK294_STOCK_LEN+1u,f);
+ int err=ferror(f),closed=fclose(f);
+ if(err||closed){free(bytes);return GK294_ROM_IO;}
+ if(n!=GK294_STOCK_LEN){free(bytes);return GK294_ROM_IDENTITY;}
+ uint8_t sha[32];gk1_sha256_bytes(bytes,n,sha);
+ if(memcmp(sha,expected_sha,sizeof sha)!=0){free(bytes);return GK294_ROM_IDENTITY;}
+ *out=bytes;return GK294_OK;
+}
+Gk294Status gk294_attest_seed_writers(const char *rom,
+ const Gk294Write *writes,size_t count,Gk294SourceProof *out){
+ if(!rom||!*rom||!writes||!out||count==0||count>GK294_MAX_WRITES)
+  return GK294_ARGUMENT;
+ for(size_t k=0;k<count;k++){
+  const Gk294Write *w=&writes[k];
+  if(w->destination_zp<0x84u||w->destination_zp>0x89u||
+     w->mapped_prg_bank>15u||
+     (w->source_pc>=0xc000u ? w->mapped_prg_bank!=15u :
+       (w->source_pc<0x8000u||w->mapped_prg_bank==15u)))
+    return GK294_BANK_MISMATCH;
+  if(k&&w->cpu_cycle<=writes[k-1].cpu_cycle)return GK294_CHRONOLOGY;
+ }
+ uint8_t *source=NULL;
+ Gk294Status r=read_authenticated_rom(rom,&source);
+ if(r!=GK294_OK)return r;
+ uint8_t mask=0u;
+ for(size_t k=0;k<count;k++){
+  const Gk294Write *w=&writes[k];
+  const size_t pos=16u+((size_t)w->mapped_prg_bank*GK294_PRG_BYTES)+
+    (w->source_pc>=0xc000u?(size_t)(w->source_pc-0xc000u):
+                             (size_t)(w->source_pc-0x8000u));
+  const size_t relative=pos-(16u+((size_t)w->mapped_prg_bank*GK294_PRG_BYTES));
+  const uint8_t opcode=source[pos];
+  unsigned expected_size=0,operand=0;
+  if(opcode==0x84u||opcode==0x85u||opcode==0x86u)expected_size=2;
+  else if(opcode==0x8cu||opcode==0x8du||opcode==0x8eu)expected_size=3;
+  /* Never read an operand past the end of the 16 KiB mapped PRG bank. */
+  if(!expected_size||relative+expected_size>GK294_PRG_BYTES){
+   free(source);return GK294_SOURCE_MISMATCH;
+  }
+  operand=source[pos+1u];
+  if(expected_size==3)operand|=((unsigned)source[pos+2u]<<8);
+  if(!expected_size||relative+expected_size>GK294_PRG_BYTES||
+     operand!=(unsigned)w->destination_zp){
+   free(source);return GK294_SOURCE_MISMATCH;
+  }
+  mask|=(uint8_t)(1u<<(w->destination_zp-0x84u));
+ }
+ free(source);
+ if(mask!=0x3fu)return GK294_INCOMPLETE;
+ Gk294SourceProof next={0};
+ next.opcode=0u; /* Multiple opcodes: no single 'canonical writer' exists. */
+ next.target=0u;
+ next.mapped_prg_bank=0xffu; /* Claimed banks may differ per record. */
+ next.count=(uint8_t)count;
+ next.unique_target_mask=mask;
+ next.source_candidates_only=1u;
+ next.original_rom_sha256_verified=1u;
+ *out=next;
+ return GK294_OK;
+}
--- /dev/null
+++ b/execution_199/gk294_seed_writer_attest.h
@@ -0,0 +1,38 @@
+#ifndef GK294_SEED_WRITER_ATTEST_H
+#define GK294_SEED_WRITER_ATTEST_H
+#include <stdint.h>
+#include <stddef.h>
+/* EXEC0294 — Copyright-clean, authenticated 6502 STORE-opcode cross-check.
+ * Raw source PC/bank claims are UNTRUSTED: a matching store in ROM does NOT
+ * establish that Mesen actually executed it, nor prove the value written.
+ * No original ROM bytes or gameplay data are distributed with this module.
+ */
+#define GK294_MAX_WRITES 64u
+typedef enum {
+ GK294_OK=0, GK294_ARGUMENT=1, GK294_ROM_IO=2, GK294_ROM_IDENTITY=3,
+ GK294_SOURCE_MISMATCH=4, GK294_BANK_MISMATCH=5, GK294_CHRONOLOGY=6,
+ GK294_INCOMPLETE=7, GK294_STAGE=8
+} Gk294Status;
+typedef struct {
+ uint64_t cpu_cycle;          /* chronological CPU-cycle claim, not proof */
+ uint16_t source_pc;          /* alleged executing STORE opcode address */
+ uint8_t mapped_prg_bank;     /* 0..15 in 16 KiB MMC1 bank mapping */
+ uint8_t destination_zp;     /* must be $84..$89 */
+} Gk294Write;
+typedef struct {
+ uint8_t opcode;             /* authenticated ROM opcode, no PRG embedded */
+ uint8_t target;             /* $84..$89 */
+ uint8_t mapped_prg_bank;
+ uint8_t count;
+ uint8_t unique_target_mask; /* 0x3F means all six seed bytes observed */
+ uint8_t source_candidates_only; /* ALWAYS 1: no live CPU proof */
+ uint8_t original_rom_sha256_verified; /* source bytes verified once */
+} Gk294SourceProof;
+/* Reject all indirect, indexed, stack, or unclear write modes instead of
+ * mistaking a nearby byte sequence for observed source. Direct zero-page
+ * STA/STX/STY and absolute stores to zero-page addresses only.
+ * Failure leaves *out unchanged; successful result cannot unlock gameplay.
+ */
+Gk294Status gk294_attest_seed_writers(const char *private_rom,
+ const Gk294Write *claimed, size_t count, Gk294SourceProof *out);
+#endif
--- /dev/null
+++ b/execution_199/test_gk294_seed_writer_attest.c
@@ -0,0 +1,130 @@
+/* EXEC0294: hostile source-claim attestation, no NES ROM shipped.
+ * Optional argv[2] PRIVATE original stock ROM enables the authenticated
+ * positive branch; without ROM run only stage/rollback/invalid-input QA.
+ * Six accepted raw code-pattern locations are STATIC candidates, emphatically
+ * NOT actual observed writes or proof of 6502 reachability.
+ */
+#include "gk1_app_state.h"
+#include "gk1_source_identity.h"
+#include <stdio.h>
+#include <stdlib.h>
+#include <string.h>
+#define CK(x) do{if(!(x)){fprintf(stderr,"GK294 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
+static const Gk294Write candidates[6]={
+ {1000u,0x9dfdu,9u,0x84u},
+ {2000u,0x8919u,8u,0x85u},
+ {3000u,0x8676u,5u,0x86u},
+ {4000u,0x8896u,1u,0x87u},
+ {5000u,0x882fu,3u,0x88u},
+ {6000u,0x855bu,0u,0x89u}
+};
+int main(int argc,char **argv){
+ unsigned checks=0;
+ CK(argc==2||argc==3);
+ Gk294SourceProof out;memset(&out,0xa5,sizeof out);
+ Gk294SourceProof orig=out;
+ CK(gk294_attest_seed_writers(NULL,candidates,6,&out)==GK294_ARGUMENT);
+ CK(gk294_attest_seed_writers("",candidates,6,&out)==GK294_ARGUMENT);
+ CK(gk294_attest_seed_writers("/missing/rom.nes",NULL,6,&out)==GK294_ARGUMENT);
+ CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,0,&out)==GK294_ARGUMENT);
+ CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,65,&out)==GK294_ARGUMENT);
+ CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,6,NULL)==GK294_ARGUMENT);
+ CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,6,&out)==GK294_ROM_IO);
+ CK(memcmp(&out,&orig,sizeof out)==0);
+ Gk294Write trial[6];memcpy(trial,candidates,sizeof trial);
+ trial[0].source_pc=0x7fffu;
+ CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_BANK_MISMATCH);
+ memcpy(trial,candidates,sizeof trial);trial[0].mapped_prg_bank=15;
+ CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_BANK_MISMATCH);
+ memcpy(trial,candidates,sizeof trial);trial[0].destination_zp=0x83;
+ CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_BANK_MISMATCH);
+ memcpy(trial,candidates,sizeof trial);trial[0].cpu_cycle=2000;
+ CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_CHRONOLOGY);
+ CK(memcmp(&out,&orig,sizeof out)==0);
+ Gk1App *app=calloc(1,sizeof *app),*before=malloc(sizeof *before);
+ Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
+ CK(app&&before&&dataset);
+ gk1_app_init(app);
+ CK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
+ CK(gk1_app_install_templates(app,dataset));
+ app->active_world=0;app->rules_profile=0;
+ app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
+ app->setup_difficulty[0]=1;
+ memcpy(before,app,sizeof *before);
+ CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_ROM_IO);
+ CK(!memcmp(before,app,sizeof *app));
+ CK(!memcmp(&out,&orig,sizeof out));
+ app->rules_profile=1;
+ CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_STAGE);
+ app->rules_profile=0;
+ app->stock_checkpoint.valid=1;
+ CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_STAGE);
+ app->stock_checkpoint.valid=0;
+ app->worlds[0].countries[0].gold++;
+ CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_STAGE);
+ app->worlds[0].countries[0].gold--;
+ CK(!memcmp(before,app,sizeof *app));
+ CK(!memcmp(&out,&orig,sizeof out));
+ if(argc==3){
+  const char *rom=argv[2];
+  CK(gk1_verified_original_csv(argv[1]));
+  for(unsigned world=0;world<=1;world++){
+   app->active_world=(uint8_t)world;
+   app->setup_stage[world]=GK53_SETUP_AWAIT_SOURCE;
+   for(unsigned diff=1;diff<=5;diff++){
+    app->setup_difficulty[world]=(uint8_t)diff;
+    memcpy(before,app,sizeof *before);
+    CK(gk1_app_attest_seed_writer_source(app,rom,candidates,6,&out)==GK294_OK);
+    CK(!memcmp(before,app,sizeof *app));
+    CK(out.count==6&&out.unique_target_mask==0x3fu&&
+       out.source_candidates_only==1&&out.original_rom_sha256_verified==1);
+    Gk294SourceProof replay={0};
+    CK(gk1_app_attest_seed_writer_source(app,rom,candidates,6,&replay)==GK294_OK);
+    CK(!memcmp(&out,&replay,sizeof out));
+   }
+  }
+  orig=out;
+  memcpy(trial,candidates,sizeof trial);trial[0].source_pc=0xffffu;
+  trial[0].mapped_prg_bank=15u;
+  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
+  memcpy(trial,candidates,sizeof trial);trial[0].source_pc=0xbfffu;
+  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
+  memcpy(trial,candidates,sizeof trial);trial[3].source_pc++;
+  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
+  CK(!memcmp(&out,&orig,sizeof out));
+  memcpy(trial,candidates,sizeof trial);trial[3].mapped_prg_bank=2;
+  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
+  memcpy(trial,candidates,sizeof trial);trial[3].destination_zp=0x88;
+  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
+  memcpy(trial,candidates,sizeof trial);trial[0].destination_zp=0x85;
+  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
+  CK(gk294_attest_seed_writers(rom,candidates,5,&out)==GK294_INCOMPLETE);
+  CK(!memcmp(&out,&orig,sizeof out));
+  CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,6,&out)==GK294_ROM_IO);
+  CK(!memcmp(&out,&orig,sizeof out));
+  /* Corrupt authenticated source WITHOUT publishing it. Refuse exact-size
+   * mutation, truncated and extended input, always preserving out. */
+  const char *badpath="GK1_EXEC0294_transient_BAD_ROM_DO_NOT_PUBLISH.bin";
+  unsigned char *bytes=malloc(262161u);CK(bytes);
+  FILE *src=fopen(rom,"rb");CK(src);
+  CK(fread(bytes,1,262161u,src)==262160u);
+  CK(fclose(src)==0);
+  for(unsigned which=0;which<3;which++){
+   FILE *bad=fopen(badpath,"wb");CK(bad);
+   size_t length=262160u;
+   if(which==0)bytes[100u]^=1u;
+   if(which==1)length--;
+   if(which==2){length++;bytes[262160u]=0u;}
+   CK(fwrite(bytes,1,length,bad)==length);
+   CK(fclose(bad)==0);
+   CK(gk294_attest_seed_writers(badpath,candidates,6,&out)==GK294_ROM_IDENTITY);
+   CK(!memcmp(&out,&orig,sizeof out));
+   CK(remove(badpath)==0);
+   if(which==0)bytes[100u]^=1u;
+  }
+  free(bytes);
+ }
+ printf("GK294 %u assertions PASS — source candidates %s; NO actual Mesen seed writes or first Spring\n",
+   checks,argc==3?"ROM-AUTHENTICATED":"NOT RUN WITHOUT PRIVATE ROM");
+ free(app);free(before);free(dataset);return 0;
+}
