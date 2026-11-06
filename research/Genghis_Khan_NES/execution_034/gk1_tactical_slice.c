#include "gk1_tactical_slice.h"
#include <stddef.h>
#include <string.h>

static const uint8_t move_costs[7]={8,6,3,4,5,4,5};
static const uint8_t hazard_pcts[7]={7,5,255,2,5,255,255};

int gk1_mobility_cost(unsigned t) { return t<7 ? move_costs[t] : -1; }
int gk1_hazard_percent(unsigned t) { return t<7 ? hazard_pcts[t] : -1; }

int gk1_casualties(uint16_t soldiers, unsigned terrain, uint64_t seed48,
                   Gk1CasualtyResult *out)
{
    if(out==NULL || terrain>=7 || seed48>UINT64_C(0xffffffffffff)) return -1;
    Gk1CasualtyResult r={0};
    int p=gk1_hazard_percent(terrain);
    r.percent=(uint8_t)p;
    if(p==255) {
        r.after_percent=soldiers;
        r.after_random=soldiers;
        r.next_seed=seed48;
    } else {
        r.after_percent=(uint16_t)(((uint32_t)soldiers*(100u-(unsigned)p))/100u);
        r.next_seed=(UINT64_C(0)-((seed48|1u)*UINT64_C(0xE0900E0187C5))) & UINT64_C(0xffffffffffff);
        const uint16_t native_rand=(uint16_t)((r.next_seed>>32)&0x7fffu);
        r.has_random=1;
        r.rng_remainder=(uint8_t)(native_rand%4u);
        const unsigned loss=3u+r.rng_remainder;
        r.after_random=(uint16_t)((unsigned)r.after_percent<loss ? 0u : (unsigned)r.after_percent-loss);
    }
    *out=r;
    return 0;
}

Gk1BattleState gk1_checkpoint_fixture(uint8_t mobility, uint8_t terrain,
                                       Gk1Occupancy occupant)
{
    Gk1BattleState s={0};
    s.mover=(Gk1Unit){40,7,7,mobility};
    s.friendly_slot1=(Gk1Unit){40,6,9,10};
    s.enemy_slot10=(Gk1Unit){70,255,255,0}; /* Unknown original position is deliberately not invented. */
    if(occupant==GK1_OCC_FRIEND_SLOT1){s.friendly_slot1.x=8;s.friendly_slot1.y=7;}
    if(occupant==GK1_OCC_ENEMY_SLOT10){s.enemy_slot10.x=8;s.enemy_slot10.y=7;}
    s.rng48=GK1_SEED_INPUT;
    s.gold=403;s.food=198;
    s.tile_at_destination=terrain;
    s.phase=GK1_PHASE_MOVE_WHERE;
    s.combined_mod_profile=1;
    s.exact_checkpoint_unit_profile=1;
    s.other_units_guard=UINT32_C(0x8e67ec03); /* Sentinel, NOT a sourced hash. */
    return s;
}

static int source_base_is_exact(const Gk1BattleState *s)
{
    if(s==NULL || s->combined_mod_profile!=1 || s->exact_checkpoint_unit_profile!=1 || s->phase!=GK1_PHASE_MOVE_WHERE ||
       s->has_pending_step!=0 || s->completion_word!=0 ||
       s->mover.soldiers!=40 || s->mover.x!=7 || s->mover.y!=7 ||
       s->rng48!=GK1_SEED_INPUT || s->gold!=403 || s->food!=198 ||
       s->friendly_slot1.soldiers!=40 || s->enemy_slot10.soldiers!=70) return 0;
    return 1;
}

static void mark_finished(Gk1BattleState *s, uint16_t how_many)
{
    s->mover.mobility=10;
    s->completion_word=(uint16_t)(s->completion_word+how_many);
    s->phase=GK1_PHASE_ORDERS;
    s->has_pending_step=0;
}

Gk1MoveResult gk1_apply_observed_step(const Gk1BattleState *before,
                                      uint8_t dest_x, uint8_t dest_y,
                                      Gk1Occupancy occupant,
                                      Gk1BattleState *after)
{
    if(after==NULL || !source_base_is_exact(before) || dest_x!=8 || dest_y!=7)
        return GK1_UNVERIFIED;
    const uint8_t terrain=before->tile_at_destination;
    const uint8_t mobility=before->mover.mobility;
    if(occupant!=GK1_OCC_EMPTY && occupant!=GK1_OCC_FRIEND_SLOT1 &&
       occupant!=GK1_OCC_ENEMY_SLOT10) return GK1_UNVERIFIED;
    /* Validated checkpoint unit placement. Unknown ownership, variants or unit
     * strength cannot borrow one of these bounded behavior fixtures. */
    if(occupant==GK1_OCC_EMPTY) {
        if(before->friendly_slot1.x!=6 || before->friendly_slot1.y!=9 ||
           (before->enemy_slot10.x==8 && before->enemy_slot10.y==7)) return GK1_UNVERIFIED;
    } else if(occupant==GK1_OCC_FRIEND_SLOT1) {
        if(terrain!=3 || mobility!=10 || before->friendly_slot1.x!=8 ||
           before->friendly_slot1.y!=7 ||
           (before->enemy_slot10.x==8 && before->enemy_slot10.y==7)) return GK1_UNVERIFIED;
    } else {
        if(terrain!=3 || mobility!=10 || before->enemy_slot10.x!=8 ||
           before->enemy_slot10.y!=7 || before->friendly_slot1.x!=6 ||
           before->friendly_slot1.y!=9) return GK1_UNVERIFIED;
    }
    const int known = (terrain==3 && (mobility==2 || mobility==4 || mobility==10)) ||
                      (terrain==0 && mobility==10 && occupant==GK1_OCC_EMPTY);
    if(!known || (occupant!=GK1_OCC_EMPTY && mobility!=10)) return GK1_UNVERIFIED;
    Gk1BattleState updated=*before;
    if(occupant==GK1_OCC_ENEMY_SLOT10 || (terrain==3 && mobility==2)) {
        *after=updated; /* Native movement command rejects before consuming RNG. */
        return GK1_REJECTED;
    }
    const int cost=gk1_mobility_cost(terrain);
    if(cost<0 || (int)mobility<cost) return GK1_UNVERIFIED;
    Gk1CasualtyResult casualties;
    if(gk1_casualties(before->mover.soldiers,terrain,before->rng48,&casualties)!=0)
        return GK1_UNVERIFIED;
    updated.mover.x=dest_x;updated.mover.y=dest_y;
    updated.mover.mobility=(uint8_t)(mobility-(unsigned)cost);
    updated.mover.soldiers=casualties.after_random;
    updated.rng48=casualties.next_seed;
    if(occupant==GK1_OCC_FRIEND_SLOT1) {
        /* Verified outcome for EXACT 40 + 40 controlled experiment ONLY.
         * Native source join order/conditions, overflow and general type
         * restrictions remain unresolved. This branch must not generalize. */
        updated.mover.soldiers=74;
        updated.friendly_slot1.soldiers=0;
        mark_finished(&updated,2);
        *after=updated;
        return GK1_FINISHED;
    }
    if(mobility==(uint8_t)cost || terrain==0) {
        mark_finished(&updated,1);
        *after=updated;
        return GK1_FINISHED;
    }
    updated.has_pending_step=1;
    updated.phase=GK1_PHASE_MOVE_WHERE;
    *after=updated;
    return GK1_STEP_PENDING;
}

Gk1MoveResult gk1_finish_observed_path(const Gk1BattleState *before,
                                       Gk1BattleState *after)
{
    /* Only the actual one-step forest (10->6) with subsequent B is traced. */
    if(before==NULL || after==NULL || before->combined_mod_profile!=1 ||
       before->has_pending_step!=1 || before->exact_checkpoint_unit_profile!=1 || before->phase!=GK1_PHASE_MOVE_WHERE ||
       before->mover.soldiers!=34 || before->mover.x!=8 || before->mover.y!=7 ||
       before->mover.mobility!=6 || before->tile_at_destination!=3 ||
       before->rng48!=GK1_SEED_AFTER_HAZARD || before->completion_word!=0 ||
       before->gold!=403 || before->food!=198 ||
       before->friendly_slot1.soldiers!=40 || before->friendly_slot1.x!=6 ||
       before->friendly_slot1.y!=9) return GK1_UNVERIFIED;
    Gk1BattleState updated=*before;
    mark_finished(&updated,1);
    *after=updated;
    return GK1_FINISHED;
}
