#include "gk331_app_offer_bridge.h"
#include "gk331_stock_offer_rng.h"
#include <stdio.h>
#include <string.h>
int main(void){
 unsigned checks=0;
 unsigned char seed[6]={1u,2u,3u,4u,5u,6u};
 Gk331Offers out;memset(&out,0x42,sizeof out);
 Gk331Offers before=out;
 if(gk331_offer_masks(NULL,seed,&out)!=GK331_BAD)return 2;
 checks++;
 if(memcmp(&out,&before,sizeof out)!=0)return 2;
 checks++;
 if(gk331_offer_masks("/gk331/no/existing/original/rom",seed,&out)!=GK331_UNVERIFIED_ROM)return 2;
 checks++;
 if(memcmp(&out,&before,sizeof out)!=0)return 2;
 checks++;
 if(gk331_app_stock_trade_offers(NULL,"/gk331/no/original",seed,&out)!=GK331_BAD)return 2;
 checks++;
 if(memcmp(&out,&before,sizeof out)!=0)return 2;
 checks++;
 /* No fabricated Spring from uninitialized actual native app. */
 Gk1App app;gk1_app_init(&app);
 if(gk331_app_stock_trade_offers(&app,"/gk331/no/original",seed,&out)!=GK331_STAGE)return 2;
 checks++;
 if(memcmp(&out,&before,sizeof out)!=0)return 2;
 checks++;
 printf("GK331 public native-app refusal/rollback checks=%u PASS; first Spring NOT GENERATED\n",checks);
 return 0;
}
