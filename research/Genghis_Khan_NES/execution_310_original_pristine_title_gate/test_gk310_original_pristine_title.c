/* EXEC0310: authentic FIRST SPRING admission cannot silently accept an
 * uncommitted but contaminated fresh title setup. This is a C11 regression
 * inside the same complete inherited native Gk1App, not a new game engine. */
#include "gk162_original_entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned assertions=0;
#define CK(expr) do {assertions++; if(!(expr)){fprintf(stderr,"GK310 FAIL line %d: %s\n",__LINE__,#expr);exit(2);}}while(0)

static void reset_title(Gk1App *app,const Gk1TemplateDataset *data) {
    gk1_app_init(app);
    CK(gk1_app_install_templates(app,data));
    app->rules_profile=0u;
}
static void refuse(const Gk1App *state,const char *csv) {
    Gk1App *app=malloc(sizeof *app),*before=malloc(sizeof *before);
    Gk79Checkpoint *fake=calloc(1,sizeof *fake);
    CK(app&&before&&fake);
    *app=*state;
    CK(!gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
    *before=*app;
    CK(gk162_enter_original_checkpoint(app,csv,"/private-not-provided",
       GK77_FRESH_MONGOL_WORKRAM,0u)==GK162_ENTRY_TEMPLATE);
    CK(!memcmp(app,before,sizeof *app));
    fake->valid=1u;fake->source_verified=1u;
    fake->profile=GK77_FRESH_MONGOL_WORKRAM;fake->player_index=0u;
    fake->world.scenario_id=app->worlds[0].scenario_id;
    fake->world.country_count=app->worlds[0].country_count;
    CK(gk162_commit_verified_checkpoint(app,csv,fake)==GK162_ENTRY_TEMPLATE);
    CK(!memcmp(app,before,sizeof *app));
    free(fake);free(before);free(app);
}
int main(int argc,char **argv) {
    CK(argc==2);
    Gk1TemplateDataset *data=malloc(sizeof *data);
    Gk1App *app=calloc(1,sizeof *app);
    CK(data&&app);
    CK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
    reset_title(app,data);
    CK(gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
    CK(gk162_original_setup_compatible(app,GK77_ARCHIVED_MONGOL_SAVERAM));
    CK(gk162_original_setup_compatible(app,GK77_ARCHIVED_WORLD_SAVERAM));

    /* Old EXEC0309 wrongly ACCEPTED all these as fresh-title compatible
     * solely because setup_stage remained GK53_SETUP_PLAYERS. */
    reset_title(app,data);app->setup_candidate[0]=0u;refuse(app,argv[1]);
    reset_title(app,data);app->setup_rulers[0][0]=0u;refuse(app,argv[1]);
    reset_title(app,data);app->setup_player_count[0]=2u;refuse(app,argv[1]);
    reset_title(app,data);app->setup_difficulty[0]=3u;refuse(app,argv[1]);
    reset_title(app,data);app->setup_player_slot[0]=1u;refuse(app,argv[1]);
    reset_title(app,data);app->setup_ability_method[0]=1u;refuse(app,argv[1]);
    reset_title(app,data);app->setup_method_cursor[0]=1u;refuse(app,argv[1]);
    reset_title(app,data);app->manual_bonus_active[0]=1u;refuse(app,argv[1]);
    reset_title(app,data);app->manual_bonus[0][0][0]=1u;refuse(app,argv[1]);
    reset_title(app,data);app->manual_bonus_person[0]=1u;refuse(app,argv[1]);
    reset_title(app,data);app->manual_bonus_ability[0]=1u;refuse(app,argv[1]);

    /* All archived-restored sources remain tied to truly pristine titles. */
    reset_title(app,data);app->setup_candidate[0]=0u;
    CK(!gk162_original_setup_compatible(app,GK77_ARCHIVED_MONGOL_SAVERAM));
    reset_title(app,data);app->setup_candidate[1]=6u;
    CK(!gk162_original_setup_compatible(app,GK77_ARCHIVED_WORLD_SAVERAM));

    /* Existing separately observed known fresh chooser route stays intact,
     * never claims ROM source-independent first-Spring generation. */
    reset_title(app,data);
    app->active_world=0u;
    app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
    app->setup_player_count[0]=1u;
    app->setup_player_slot[0]=1u;
    app->setup_candidate[0]=0u;
    app->setup_rulers[0][0]=0u;
    app->setup_difficulty[0]=1u;
    app->setup_ability_method[0]=1u;
    CK(gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
    app->setup_difficulty[0]=2u;
    CK(!gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
    app->setup_difficulty[0]=1u;
    app->manual_bonus[0][4][5]=1u;
    CK(!gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
    free(app);free(data);
    printf("GK310 %u C11 source-setup predicates/rollback controls PASS; no new NES Spring generated\n",assertions);
    return 0;
}
