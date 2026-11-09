#ifndef GK137_STOCK_RNG_H
#define GK137_STOCK_RNG_H
#include <stdint.h>
/* Source-derived from authentic NES economic mapping v1.07, bank C289.
 * Deterministic 48-bit CPU-side generator ONLY. NMI/frame evolution of the
 * original seed is NOT modeled. No candidate seed implies real NES setup.
 */
typedef struct { uint64_t seed48; uint64_t draws; } Gk137StockRng;
typedef enum { GK137_OK=0, GK137_BAD_INPUT=1 } Gk137Result;
/* NES memory $84..$89 stores this unsigned seed in BIG-ENDIAN byte order. */
Gk137Result gk137_from_nes_bytes(Gk137StockRng *out,const uint8_t bytes[6]);
void gk137_to_nes_bytes(const Gk137StockRng *rng,uint8_t bytes[6]);
/* One unconditional C289 source draw: returns 15-bit result. */
Gk137Result gk137_draw15(Gk137StockRng *rng,uint16_t *out);
/* rand(n); n==0 returns 0 WITHOUT advancing the generator. */
Gk137Result gk137_bounded(Gk137StockRng *rng,uint16_t n,uint16_t *out);
/* Source-verified RATE expression only; does not run the rest of Spring. */
Gk137Result gk137_spring_rate(Gk137StockRng *rng,uint8_t difficulty,
                              uint16_t *out_rate);
/* Summer/Autumn/Winter source decline expression, excluding the separate
 * original direction draw and every intervening AI/NMI RNG call. This is a
 * conditional *one-draw* formula, NOT a live seasonal transition.
 */
Gk137Result gk137_rate_decline_one_draw(Gk137StockRng *rng,uint16_t current,
                                        uint16_t *out_rate,uint8_t *percent);
#endif
