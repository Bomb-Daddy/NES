#ifndef GK58_SLOTS_H
#define GK58_SLOTS_H
#include <stddef.h>
#include <stdint.h>
/* Port-only slot namespace for immutable, exclusive-created pre-setup
 * snapshots. Slot1 is exactly the inherited filename (backwards compatible).
 * Other slots append suffixes and never overwrite or rename old saves.
 * This is not a completed playable-campaign save system. */
#define GK58_SLOT_MAX 3u
uint8_t gk58_slot_cycle(uint8_t slot,int direction);
int gk58_slot_path(const char *base,uint8_t slot,char *output,size_t length);
#endif
