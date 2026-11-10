#ifndef GK170_SLOT_PREVIEW_H
#define GK170_SLOT_PREVIEW_H
#include "gk167_private_cold_save.h"
/* PORT-ONLY, owner-private checkpoint inventory for controller TITLE screen.
 * Preview uses the SAME source-attested, owner-private cold-load validator as
 * actual Y-restore; never trusts the header, does not expose country RAM,
 * and never changes the live game, turns, country fields, or saved files.
 * This is a saved ORIGINAL spring continuation, NOT a playable NES season.
 */
typedef enum {
 GK170_SLOT_INVALID=0, GK170_SLOT_UNAVAILABLE=1,
 GK170_SLOT_REFUSED=2, GK170_SLOT_VERIFIED=3
} Gk170SlotStatus;
typedef struct {
 Gk170SlotStatus status;
 uint8_t profile,country_count,player_index,orders_remaining,orders_ended;
 uint16_t year,rate;
} Gk170SlotPreview;
/* Requires canonical CSV and already-installed verified PRE_SETUP worlds.
 * One file per call, only on slot selection/title entry (not on every frame).
 * On argument failure output untouched. On file failure returns a neutral
 * status with zero numeric fields. */
Gk170SlotStatus gk170_preview_private_slot(const char *path,const char *csv,
 const Gk1App *live,Gk170SlotPreview *out);
#endif
