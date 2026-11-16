#ifndef GK326_CPU_COUNTRY_STORE_H
#define GK326_CPU_COUNTRY_STORE_H
#include <stddef.h>
#include <stdint.h>
/* Executable bounded NMOS-6502 original NES country-write address decoding.
 * This does NOT prove a sampled PC executed, supply original stock RNG, grant
 * orders or generate first Spring. No copyright-bearing ROM is embedded. */
#define GK326_ORIGINAL_ROM_SIZE 262160u
#define GK326_COUNTRY_BASE 0x6dcdU
#define GK326_COUNTRY_BYTES 83u
#define GK326_COUNTRY_WORK_RAM_OFFSET 0x0dcdU
#define GK326_RAM_BYTES 8192u

typedef enum {GK326_FRESH_WORKRAM=1,GK326_ARCHIVED_SAVERAM=2} Gk326Condition;
typedef enum {
 GK326_OK=0,GK326_BAD_ARGUMENT=1,GK326_BAD_ROM=2,
 GK326_WRONG_BANK=3,GK326_UNSUPPORTED_OPCODE=4,
 GK326_NOT_COUNTRY=5,GK326_BAD_CONTEXT=6
} Gk326Status;
typedef struct {
 uint16_t pc;       /* actual CPU PC from an independently observed event */
 uint8_t bank;      /* actual Mesen PRG bank (0-14 switch; 15 fixed) */
 uint8_t a,x,y;     /* observed CPU register values at this instruction */
 uint8_t zp[256];   /* observed CPU zero page; no pointer guessing */
 Gk326Condition condition; /* fresh WorkRAM, archived SaveRAM */
 uint8_t country_count;    /* 14 Mongol, 27 World only */
} Gk326CpuSource;
typedef struct {
 uint16_t cpu_address;
 uint16_t physical_offset;
 uint8_t country_one_based,offset_in_country;
 uint8_t opcode,old_value,new_value;
 Gk326Condition backing;
 uint8_t source_address_decoded;
 uint8_t original_rom_hash_verified;
 uint8_t first_spring_generated; /* must remain 0 */
 uint8_t player_orders_granted;  /* must remain 0 */
} Gk326StoreReceipt;
/* sourceRom is original authenticated ROM privately supplied at test time.
 * sourceCpu must be a separately authenticated actual 6502 snapshot; this
 * function only decodes its instruction and applies ONE original CPU store
 * to a COPY of the source's 8192-byte RAM bank, never to Gk1App or a save.
 * On failure the entire RAM and receipt are unchanged. */
/* Strict production route: same decoder + exact private original stock SHA256. */
Gk326Status gk326_apply_stock_country_store(const uint8_t *source_rom,size_t rom_size,
 const Gk326CpuSource *source_cpu,uint8_t source_ram[GK326_RAM_BYTES],
 Gk326StoreReceipt *receipt);
/* Structural address-decoder fixture route: NOT authenticated stock. */
Gk326Status gk326_apply_country_store(const uint8_t *source_rom,size_t rom_size,
 const Gk326CpuSource *source_cpu,uint8_t source_ram[GK326_RAM_BYTES],
 Gk326StoreReceipt *receipt);
#endif
