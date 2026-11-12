#include "gk225_practice_resume.h"
#include "gk224_practice_save.h"
#include <stdlib.h>
#include <string.h>

Gk225Status gk225_resume_title(Gk1App *app,const char *csv,const char *path){
 if(!app||app->screen!=GK1_SCREEN_TITLE||app->rules_profile!=1u||
    app->port_spring.active||app->classic_assign.active||
    app->enhanced_assign.active||!app->world_ready[0]||!app->world_ready[1]||
    app->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return GK225_WRONG_PHASE;
 if(!path||!*path||!csv||!*csv)return GK225_SOURCE;
 /* The full native app state is committed only after source-authenticated
  * disk read, 80-byte SHA, both original CSV identities and receipt replay.
  * One candidate at a time avoids destructive partial transition on fail. */
 Gk222Spring *candidate=calloc(1,sizeof *candidate);
 if(!candidate)return GK225_SOURCE;
 Gk225Status result=GK225_SOURCE;
 for(unsigned w=0;w<2;w++){
  uint8_t command=0;
  Gk224Status r=gk224_load(path,csv,&app->worlds[w],candidate,&command);
  if(r==GK224_OK){
   if(candidate->scenario!=(uint8_t)app->worlds[w].scenario_id ||
      candidate->player_index>=app->worlds[w].country_count){
    result=GK225_CORRUPT;break;
   }
   app->port_spring=*candidate;
   app->port_spring_command=command;
   app->active_world=(uint8_t)w;
   app->country_cursor[w]=candidate->player_index;
   app->screen=GK1_SCREEN_ENHANCED_SPRING_LAB;
   result=GK225_OK;break;
  }
  if(r==GK224_UNAVAILABLE){result=GK225_MISSING;break;}
  if(r==GK224_CORRUPT||r==GK224_STATE){result=GK225_CORRUPT;break;}
  /* A valid World receipt will fail the Mongol source digest. The second
   * attempt must independently pass the full same-source replay. */
 }
 free(candidate);
 return result;
}
