#ifndef GK133_BOOT_ORDER_H
#define GK133_BOOT_ORDER_H
#include <stdint.h>
/* Source-verified *order* of the stock startup's first F5 consumer and its
 * later F5 reset. A Mesen AllZeros run is an explicitly chosen emulator
 * power-on profile, never a universal hardware RAM default. No game unlock. */
typedef enum {
 GK133_BOOT_OK=0, GK133_BOOT_ARGUMENT=1, GK133_BOOT_ROM_REFUSED=2,
 GK133_BOOT_SOURCE_DIVERGED=3
} Gk133BootStatus;
typedef struct {
 uint16_t first_banked_pc;          /* executed source trace $8000 */
 uint16_t banked_jsr_target;       /* $8000 -> $8060 */
 uint16_t first_f5_read_pc;        /* $8060 */
 uint16_t later_f5_clear_pc;       /* $8040, after first read under zero RAM */
 uint16_t subsequent_ppu_gate_pc;  /* $C5BE under archived Mesen AllZeros */
 uint16_t subsequent_ppu_gate_register; /* $2002 */
 uint32_t unknown_ram_instructions,mesen_allzeros_instructions;
 uint8_t unseeded_f5_writes;       /* MUST be 0 */
 uint8_t mesen_allzeros_f5_writes; /* MUST be 1 */
 uint8_t power_on_ram_required;    /* MUST be 1 */
 uint8_t stock_first_spring_ready; /* MUST be 0 */
} Gk133BootOrder;
/* Input MUST be the exact original private ROM. On all non-OK results,
 * output is left byte-identical. Does not retain or expose ROM bytes. */
Gk133BootStatus gk133_verify_boot_f5_order(const char *rom,
                                           Gk133BootOrder *out);
#endif
