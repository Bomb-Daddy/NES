#include "gk68_observed_rest.h"
/* Exact source: ZL-GK1-MESEN-GAP-0001, approved combined modified ROM,
 * Fonzarelli Spring 1175, two orders. The elapsed 8000-frame runner
 * timed out after snapshots, so provenance is CAPTURED_NEAR_END/TIMEOUT,
 * not process exit0; B cancellation comparison independently exited 0. */
static const Gk68RestCase spring={1175,170,101,124,185,439,208,51,0,2,GK68_REST_SPRING};
static const Gk68RestCase pending={1175,170,101,124,185,439,208,51,0,2,GK68_REST_CONFIRM};
static const Gk68RestCase summer={1175,829,97,115,183,439,208,51,1,3,GK68_REST_SUMMER};
static int equal(const Gk68RestCase *a,const Gk68RestCase *b){
 return a->year==b->year && a->bp==b->bp && a->rate==b->rate &&
  a->skill==b->skill && a->economy==b->economy && a->gold==b->gold &&
  a->food==b->food && a->troops==b->troops && a->season==b->season &&
  a->orders==b->orders && a->stage==b->stage;
}
int gk68_rest_valid(const Gk68RestCase *s){
 return s&&(equal(s,&spring)||equal(s,&pending)||equal(s,&summer));
}
Gk68RestResult gk68_rest_open(Gk68RestCase *out){
 if(!out) return GK68_REST_BAD;
 *out=spring;
 return GK68_REST_OK;
}
Gk68RestResult gk68_rest_press_a(Gk68RestCase *state){
 if(!gk68_rest_valid(state))return GK68_REST_BAD;
 if(state->stage==GK68_REST_SPRING){*state=pending;return GK68_REST_OK;}
 if(state->stage==GK68_REST_CONFIRM){*state=summer;return GK68_REST_OK;}
 return GK68_REST_WRONG_PHASE;
}
Gk68RestResult gk68_rest_press_b(Gk68RestCase *state){
 if(!gk68_rest_valid(state))return GK68_REST_BAD;
 if(state->stage!=GK68_REST_CONFIRM)return GK68_REST_WRONG_PHASE;
 *state=spring;return GK68_REST_OK;
}
