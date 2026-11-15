
/* EXEC0304: strict register-value checking atop source-bounded original NES
 * chronological replay in the inherited application, never a phase unlock. */
Gk304Status gk1_app_original_store_values(const Gk1App *app,
 const char *rom,const uint8_t initial[6],
 const Gk302SeedEvent *events,size_t count,
 const Gk304Registers *registers,size_t nr,Gk304Report *out){
 Gk302SeedResult gate={0};
 Gk302Status st=gk1_app_original_seed_timeline(app,rom,initial,
                                                events,count,&gate);
 if(st!=GK302_OK)return st==GK302_STAGE?GK304_STAGE:
                       st==GK302_ROM?GK304_STOCK_ROM:GK304_TIMELINE;
 return gk304_verify_store_values(rom,initial,events,count,registers,nr,out);
}
