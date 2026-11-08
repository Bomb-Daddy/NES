#include "gk69_mesen_ram_map.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
/* SHA256 of exact 8192-byte live_map_capture/battle.workram, not of a ROM.
 * Private source proof: Drive GK1_MESEN_GAP_0001_Private_Native_Proof.
 * The program never bundles, modifies or publishes the copyrighted capture.
 */
static const uint8_t verified_ram_sha256[32]={
 0xdc,0xe6,0xb9,0xfb,0x05,0xd4,0x7d,0xf2,
 0xb6,0x22,0x27,0x89,0xae,0x2a,0x86,0x3d,
 0x7e,0x78,0x99,0x50,0xe1,0x6c,0x94,0x62,
 0x2e,0x70,0x0f,0x5d,0xd4,0xff,0x07,0xb1};
const char *gk69_map_label(uint8_t raw){
 switch(raw){
 case 2:return "PLAINS";
 case 3:return "FOREST";
 case 4:return "MOUNTAIN";
 default:return "UNCLASSIFIED";
 }
}
Gk69MapStatus gk69_map_from_ram(const uint8_t *ram,size_t length,Gk69MesenMap *out){
 if(!ram||!out||length!=GK69_RAM_LENGTH)return GK69_MAP_BAD_INPUT;
 uint8_t sha[32];gk1_sha256_bytes(ram,length,sha);
 if(memcmp(sha,verified_ram_sha256,sizeof sha)!=0)return GK69_MAP_NOT_EXACT_CAPTURE;
 Gk69MesenMap local;memset(&local,0,sizeof local);
 for(size_t i=0;i<GK69_MAP_CELLS;i++){
  uint8_t b=ram[GK69_MAP_OFFSET+i];local.raw[i]=b;
  if(b==2)local.source_plains++;
  else if(b==3)local.source_forest++;
  else if(b==4)local.source_mountain++;
  else local.unknown++;
 }
 /* Source facts from authenticated live WorkRAM, not reclassified high-bit flags. */
 if(local.source_plains!=75||local.source_forest!=13||
    local.source_mountain!=55||local.unknown!=17||
    local.raw[7*16+8]!=3||local.raw[7*16+9]!=7)
  return GK69_MAP_NOT_EXACT_CAPTURE;
 *out=local;return GK69_MAP_OK;
}
Gk69MapStatus gk69_map_from_file(const char *path,Gk69MesenMap *out){
 if(!path||!*path||!out)return GK69_MAP_BAD_INPUT;
 FILE *f=fopen(path,"rb");if(!f)return GK69_MAP_IO;
 uint8_t ram[GK69_RAM_LENGTH];
 size_t n=fread(ram,1,sizeof ram,f);
 int extra=fgetc(f);int failed=ferror(f);
 int closed=fclose(f);
 if(n!=sizeof ram||extra!=EOF||failed||closed!=0)return GK69_MAP_BAD_INPUT;
 return gk69_map_from_ram(ram,sizeof ram,out);
}
int gk69_map_move(Gk69MesenMap *map,int dx,int dy){
 if(!map||dx < -1||dx >1||dy < -1||dy>1||
    (dx!=0&&dy!=0)||map->x>=GK69_MAP_WIDTH||map->y>=GK69_MAP_HEIGHT)return 0;
 int x=(int)map->x+dx,y=(int)map->y+dy;
 if(x<0||x>= (int)GK69_MAP_WIDTH||y<0||y>= (int)GK69_MAP_HEIGHT)return 0;
 map->x=(uint8_t)x;map->y=(uint8_t)y;return 1;
}
