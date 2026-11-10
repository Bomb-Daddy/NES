#ifndef GK146_ASSIGN_QUANTITY_H
#define GK146_ASSIGN_QUANTITY_H
#include <stdint.h>
/* Enhanced-only controller quantity dial. This changes a proposed amount,
 * never original stock RAM, campaign orders or original NES Assign rules.
 * Valid choices are bounded to representable working-party transfers. */
typedef enum {GK146_QUANTITY_OK=0,GK146_QUANTITY_INVALID=1} Gk146QuantityResult;
Gk146QuantityResult gk146_quantity_step(uint16_t before,int direction,uint16_t *next);
int gk146_quantity_is_valid(uint16_t quantity);
#endif
