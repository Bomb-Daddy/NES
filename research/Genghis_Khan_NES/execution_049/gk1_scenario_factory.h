#ifndef GK1_SCENARIO_FACTORY_H
#define GK1_SCENARIO_FACTORY_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* Public numeric research data, not licensed NES ROM bytes. Every record is a
   LOADED TEMPLATE, before setup/difficulty/player allocation/season processing.
   This module MUST NOT be treated as a runnable first-turn scenario engine. */
#define GK1_COUNTRY_COLUMNS 45u
#define GK1_COUNTRIES_MONGOL 14u
#define GK1_COUNTRIES_WORLD 27u
#define GK1_COUNTRIES_ALL 41u
#define GK1_COUNTRY_NAME_BYTES 64u
#define GK1_LORD_NAME_BYTES 64u
#define GK1_GOODS 11u

typedef enum { GK1_MONGOL_CONQUEST=1, GK1_WORLD_CONQUEST=2 } Gk1ScenarioId;
typedef enum { GK1_PRE_SETUP_TEMPLATE=1 } Gk1ScenarioPhase;
typedef enum { GK1_LOAD_OK=0, GK1_LOAD_IO=1, GK1_LOAD_FORMAT=2,
               GK1_LOAD_VALUE=3, GK1_LOAD_ORDER=4, GK1_LOAD_UNSUPPORTED=5 } Gk1LoadStatus;
typedef struct {
    uint8_t scenario_id, country_id, index, specialty_index;
    char country_name[GK1_COUNTRY_NAME_BYTES];
    char lord_name[GK1_LORD_NAME_BYTES];
    char specialty_name[24];
    uint32_t country_sram_base, country_rom_offset;
    uint32_t specialty_sram_address, specialty_rom_offset, name_rom_offset;
    uint16_t goods[GK1_GOODS]; /* ten trade goods + Food */
    uint16_t arms,skill,troops,economy,town,defense,masons;
    uint16_t commodity_history,artisans,food_history,food_workers;
    uint16_t gold,morale,tax,extra_tax_used;
    uint16_t undrafted_reserve,eligibility_percent;
    uint16_t drafted_population,combined_population,eligible_draft_count;
} Gk1CountryTemplate;
typedef struct {
    Gk1CountryTemplate countries[GK1_COUNTRIES_ALL];
    uint8_t mongol_count,world_count;
} Gk1TemplateDataset;
typedef struct {
    Gk1ScenarioId scenario_id;
    Gk1ScenarioPhase phase;
    uint8_t country_count;
    Gk1CountryTemplate countries[GK1_COUNTRIES_WORLD];
} Gk1LoadedScenario;

/* All failures are atomic: 'out' retains its prior bytes. This loader accepts
   the exact v1.07 45-column layout, not arbitrary CSV dialects or source ROM. */
Gk1LoadStatus gk1_template_read(FILE *stream,Gk1TemplateDataset *out);
Gk1LoadStatus gk1_template_load(const char *path,Gk1TemplateDataset *out);
const Gk1CountryTemplate *gk1_template_country(const Gk1TemplateDataset *all,
                                                 Gk1ScenarioId scenario,uint8_t one_based_id);
/* Copies loaded pre-setup records into mutable independent scenario state;
   deliberately does not run the NES player's setup or seasonal processing. */
Gk1LoadStatus gk1_open_loaded_scenario(const Gk1TemplateDataset *all,
                                      Gk1ScenarioId scenario,Gk1LoadedScenario *out);
#endif
