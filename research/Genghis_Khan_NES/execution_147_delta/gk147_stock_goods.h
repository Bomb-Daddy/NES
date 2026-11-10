#ifndef GK147_STOCK_GOODS_H
#define GK147_STOCK_GOODS_H
#include "gk119_order_bridge.h"
#include <stdint.h>
/* Original-stock spring ten specialty goods, FROM the original 83-byte
 * country record at offsets +$22,+$24,...,+$34 (Food lives at +$36).
 * Strictly READ-ONLY; never a merchant availability oracle, transaction,
 * order, current seasonal generator or controller command execution.
 * This diagnostic page is permitted ONLY after a private SHA-authenticated
 * original stock checkpoint has entered the native C11 game.
 */
typedef enum {GK147_GOODS_OK=0,GK147_GOODS_INVALID=1,
              GK147_GOODS_UNVERIFIED=2,GK147_GOODS_MISMATCH=3} Gk147GoodsStatus;
typedef struct {
 uint16_t goods[10];
 uint16_t food;
 uint8_t country_index,country_count,profile;
 uint16_t year;
} Gk147StockGoods;
const char *gk147_goods_name(unsigned index);
/* Failure is atomic: out and the entire original/enhanced app remain untouched.
 * If Enhanced has an active labor receipt, require its replay to be valid too.
 * Re-decode every good from the OWNER-PRIVATE original raw record; do not
 * substitute PRE_SETUP templates, synthetic data, or arbitrary prices.
 */
Gk147GoodsStatus gk147_source_goods(const Gk79Checkpoint *original,
 const Gk116EnhancedAssign *enhanced,uint8_t rules_profile,
 unsigned country_index,Gk147StockGoods *out);
#endif
