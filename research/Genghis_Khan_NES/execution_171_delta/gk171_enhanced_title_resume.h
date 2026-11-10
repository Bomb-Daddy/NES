#ifndef GK171_ENHANCED_TITLE_RESUME_H
#define GK171_ENHANCED_TITLE_RESUME_H
#include "gk1_app_state.h"
/* Port-only research cold resume from an owner-private attested original NES
 * Spring GK17 source plus an ENHANCED action receipt. No stock initialization,
 * country RAM generation, original command or seasonal economy is implied. */
typedef enum {
 GK171_OK=0, GK171_BAD_INPUT=1, GK171_NO_RECEIPT=2,
 GK171_SOURCE_REFUSED=3, GK171_RECEIPT_REFUSED=4, GK171_MEMORY=5
} Gk171Result;
/* Existing Enhanced receipts in monotonically increasing research order:
 * GA16, GS18, GF19, GW20, GY21. First existing (highest stage) is
 * authoritative; malformed higher stage MUST NOT silently roll back. */
#define GK171_STAGES 5u
typedef struct { const char *source_gk17; const char *canonical_csv;
 const char *receipt[GK171_STAGES]; } Gk171Paths;
Gk171Result gk171_resume_stage(const Gk171Paths *paths,unsigned stage,
 Gk1App *live);
Gk171Result gk171_resume_latest(const Gk171Paths *paths,Gk1App *live,
 unsigned *restored_stage);
#endif
