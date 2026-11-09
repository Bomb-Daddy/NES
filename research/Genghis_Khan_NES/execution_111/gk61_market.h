#ifndef GK61_MARKET_H
#define GK61_MARKET_H
/* Exact original NES price/transaction arithmetic, consume only after original
   player-setup establishes a playable strategic turn. This does not manufacture
   first-Spring setup, merchant appearance, or action-gating scheduler rules. */
#include <stdint.h>
#include "../execution_049/gk1_scenario_factory.h"
#define GK61_MERCHANT_COUNT 3u
#define GK61_ITEM_COUNT 17u
#define GK61_SELLABLE_COUNT 11u
#define GK61_ABILITY_COUNT 6u

typedef enum {GK61_CHINA=0,GK61_ISLAM=1,GK61_UIGHUR=2} Gk61Merchant;
typedef enum {GK61_CLASSIC=0,GK61_SHARED_BP=1} Gk61Profile;
typedef enum {GK61_BUY=0,GK61_SELL=1,GK61_HIRE=2} Gk61Action;
typedef enum {GK61_OK=0,GK61_BAD_INPUT=1,GK61_NOT_PLAYABLE=2,GK61_UNAVAILABLE=3,
              GK61_NO_RESOURCE=4,GK61_NO_ORDERS=5,GK61_NO_ABILITY=6,GK61_NO_CAPACITY=7} Gk61Result;

typedef struct {
 uint16_t ability[GK61_ABILITY_COUNT]; /* Leadership, Judgment, Planning, Charm, Body, Battle */
 uint16_t orders;
} Gk61Actor;
typedef struct {
 /* Caller sets playable=1 ONLY after source-verified original game setup. */
 uint8_t playable;
 Gk61Profile profile;
 uint16_t rate;
 uint8_t merchant_present[GK61_MERCHANT_COUNT];
 uint8_t buy_offer[GK61_MERCHANT_COUNT][GK61_ITEM_COUNT];
 uint8_t sell_offer[GK61_MERCHANT_COUNT][GK61_SELLABLE_COUNT];
} Gk61Market;
typedef struct {
 uint16_t buy_tenths, sell_tenths, hire_tenths;
} Gk61Quote;
/* These functions only calculate source numeric rules; real market presence,
   seasonal rate and offer draws must be supplied by the original scheduler. */
Gk61Result gk61_quote(uint16_t rate,Gk61Merchant merchant,uint8_t item,Gk61Quote *out);
Gk61Result gk61_max_quantity(const Gk61Market *m,const Gk1CountryTemplate *c,
                             Gk61Merchant merchant,Gk61Action action,uint8_t item,uint16_t *out);
/* For Hire, morale_roll11 is the already-produced original rand(11) [0,10].
   No RNG draw is performed for a refusal or a non-Hire action. */
Gk61Result gk61_commit(const Gk61Market *m,Gk1CountryTemplate *c,Gk61Actor *actor,
                       Gk61Merchant merchant,Gk61Action action,uint8_t item,
                       uint16_t quantity,int morale_roll11);
#endif
