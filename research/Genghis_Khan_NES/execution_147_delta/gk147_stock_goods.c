#include "gk147_stock_goods.h"
#include <string.h>
static const char *const good_names[10]={
 "FUR","SILK","JEWELS","METALS","MEDICINES",
 "WOODWORKS","CERAMICS","GLASS","TEXTILES","PERFUME"
};
const char *gk147_goods_name(unsigned i){return i<10u?good_names[i]:NULL;}
static uint16_t le16(const uint8_t *s){return (uint16_t)((unsigned)s[0]|((unsigned)s[1]<<8u));}
Gk147GoodsStatus gk147_source_goods(const Gk79Checkpoint *cp,
 const Gk116EnhancedAssign *enh,uint8_t profile,unsigned index,Gk147StockGoods *out){
 if(!cp||!enh||!out||profile>1u||index>=GK77_MAX_COUNTRIES)
   return GK147_GOODS_INVALID;
 if(cp->valid!=1u||cp->source_verified!=1u||
    cp->world.country_count!=(cp->profile==GK77_ARCHIVED_WORLD_SAVERAM?27u:14u)||
    (cp->profile!=GK77_FRESH_MONGOL_WORKRAM&&
     cp->profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     cp->profile!=GK77_ARCHIVED_WORLD_SAVERAM)||
    index>=cp->world.country_count||!gk85_raw_source_intact(cp)||
    !gk88_stock_projection_intact(cp))return GK147_GOODS_UNVERIFIED;
 if(enh->active){
  if(profile!=1u||!gk116_attestation_intact(enh,cp))
    return GK147_GOODS_UNVERIFIED;
 }
 Gk147StockGoods result={0};
 const Gk1CountryTemplate *country=&cp->world.countries[index];
 const uint8_t *source=cp->stock_raw[index];
 for(unsigned i=0;i<10u;i++){
  result.goods[i]=le16(source+0x22u+2u*i);
  if(result.goods[i]!=country->goods[i])return GK147_GOODS_MISMATCH;
 }
 result.food=le16(source+0x36u);
 if(result.food!=country->goods[10])return GK147_GOODS_MISMATCH;
 result.country_index=(uint8_t)index;
 result.country_count=cp->world.country_count;
 result.profile=cp->profile;
 result.year=cp->year;
 *out=result;
 return GK147_GOODS_OK;
}
