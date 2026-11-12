#ifndef GK225_PRACTICE_RESUME_H
#define GK225_PRACTICE_RESUME_H
#include "gk1_app_state.h"
/* A genuine second-process ENHANCED PORT PRACTICE title resume.
 * No Classic NES game setup or seasonal order is initialized here. */
typedef enum {
 GK225_OK=0, GK225_WRONG_PHASE, GK225_MISSING,
 GK225_SOURCE, GK225_CORRUPT
} Gk225Status;
/* Reads an EXEC0224 receipt from a source-verified user-private slot.
 * The record independently selects 14/27-country model by source digest.
 * On any failure, the entire Gk1App is byte-identical to the input. */
Gk225Status gk225_resume_title(Gk1App *app,const char *csv,const char *path);
#endif
