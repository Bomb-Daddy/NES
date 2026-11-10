#ifndef GK103_NTSC_PPU_H
#define GK103_NTSC_PPU_H
#include <stdint.h>
/* SOURCE-RESEARCH ONLY: NTSC unrendered PPU event-clock model, NOT Mesen.
 * The frame origin and CPU/PPU phase are EXPLICIT assumptions. Any rendered
 * frame, read at the vblank race, or peripheral not represented MUST gate.
 * All temporal comparisons are in integer PPU dots; no floats or RNG. */
typedef struct {
 uint64_t last_dot,frames,vblank_events,nmi_edges,status_reads;
 uint8_t vblank,nmi_enable,nmi_pending,ppumask,phase;
 uint32_t frame_origin_dots; /* EXPLICIT hypothesis, not authentic Mesen phase */
 uint64_t render_window_limit_dot; /* first pre-render dot not modeled */
 uint8_t render_window_active,render_window_gate; /* GK150 opt-in ONLY */
 uint8_t origin_initial_status_unknown;
 uint8_t warmup_ignored_ctrl,rendering_unmodeled,race_gate;
} Gk103Ppu;
#define GK103_FRAME_DOTS 89342ULL
#define GK103_VBLANK_START 82182ULL
#define GK103_VBLANK_END 89002ULL
#define GK103_CTRL_WARMUP_CPU 29658ULL
/* Phase is 0..2 PPU dots relative to CPU cycle 0. Frame origin is assumed
 * scanline0 dot0. No claim this matches the original Mesen powerup phase. */
int gk103_ppu_reset(Gk103Ppu *p,uint8_t phase);
/* EXEC0104: full 89342-dot source-research frame-origin hypotheses, never parity.
 * Reject power-on frame origins within unresolved active vblank instead of
 * inventing a power-on PPUSTATUS value. Does NOT change legacy 0..2 API. */
int gk104_ppu_reset_origin(Gk103Ppu *p,uint32_t origin_dots);
/* EXEC0136: constant-time event counting for arbitrarily large valid spans.
 * Preserve PPUSTATUS-cleared vblank until a new start event. Refuse counter
 * overflow, clock regression or unmodeled rendering without state mutation. */
int gk103_ppu_advance(Gk103Ppu *p,uint64_t cpu_cycle);
/* Read only the high vblank bit, with lower source value caller-supplied.
 * Explicitly rejects ±2 PPU-dot race window; it never guesses the NMI edge. */
/* EXEC0135 refusal guarantee: no timed frame/vblank/NMI changes, output
 * mutation, or status-read consumption when a PPUSTATUS timing race fails.
 * Only diagnostic race_gate is set. This is a bounded research model. */
int gk103_ppu_status(Gk103Ppu *p,uint64_t cpu_cycle,uint8_t lower_bits,uint8_t *value);
/* Accepted after PPU powerup warmup only. */
int gk103_ppu_ctrl(Gk103Ppu *p,uint64_t cpu_cycle,uint8_t value);
/* Rendering requires odd-frame skip, sprite effects and PPUMASK -> GATE. */
/* Rejected rendering write is terminal diagnostic: only
 * rendering_unmodeled changes, never ppumask/clock/vblank/NMI counts. */
int gk103_ppu_mask(Gk103Ppu *p,uint64_t cpu_cycle,uint8_t value);
/* GK150 experimental source trace only. Rendering is NOT simulated.
 * Accept a PPUMASK render enable within one pre-render-safe timing window,
 * then fail closed BEFORE an odd-frame timing change or status ambiguity. */
int gk150_ppu_mask_bounded_window(Gk103Ppu *p,uint64_t cycle,uint8_t value);
int gk103_ppu_take_nmi(Gk103Ppu *p);
#endif
