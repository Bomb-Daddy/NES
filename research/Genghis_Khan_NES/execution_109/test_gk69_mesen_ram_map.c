#include "gk69_mesen_ram_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAILED line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 uint8_t ram[GK69_RAM_LENGTH];memset(ram,2,sizeof ram);
 Gk69MesenMap map,prior;memset(&map,0xa5,sizeof map);prior=map;
 CHECK(gk69_map_from_ram(NULL,sizeof ram,&map)==GK69_MAP_BAD_INPUT);
 CHECK(gk69_map_from_ram(ram,sizeof ram-1,&map)==GK69_MAP_BAD_INPUT);
 CHECK(gk69_map_from_ram(ram,sizeof ram,&map)==GK69_MAP_NOT_EXACT_CAPTURE);
 CHECK(memcmp(&map,&prior,sizeof map)==0);
 CHECK(gk69_map_from_file(NULL,&map)==GK69_MAP_BAD_INPUT);
 CHECK(gk69_map_from_file("not-found-gk69",&map)==GK69_MAP_IO);
 CHECK(memcmp(&map,&prior,sizeof map)==0);
 CHECK(!strcmp(gk69_map_label(2),"PLAINS"));
 CHECK(!strcmp(gk69_map_label(3),"FOREST"));
 CHECK(!strcmp(gk69_map_label(4),"MOUNTAIN"));
 for(int j=0;j<256;j++)if(j!=2&&j!=3&&j!=4)CHECK(!strcmp(gk69_map_label((uint8_t)j),"UNCLASSIFIED"));
 CHECK(gk69_map_move(NULL,1,0)==0);
 CHECK(gk69_map_move(&map,2,0)==0);
 CHECK(gk69_map_move(&map,1,1)==0);
 map.x=0;map.y=0;
 CHECK(gk69_map_move(&map,-1,0)==0);
 CHECK(gk69_map_move(&map,0,-1)==0);
 for(int x=0;x<15;x++)CHECK(gk69_map_move(&map,1,0));
 CHECK(gk69_map_move(&map,1,0)==0);
 for(int y=0;y<9;y++)CHECK(gk69_map_move(&map,0,1));
 CHECK(gk69_map_move(&map,0,1)==0);
 CHECK(map.x==15&&map.y==9);
 CHECK(gk69_map_move(&map,-1,0)&&map.x==14);
 if(argc==2){
   CHECK(gk69_map_from_file(argv[1],&map)==GK69_MAP_OK);
   CHECK(map.x==0&&map.y==0);
   CHECK(map.source_plains==75&&map.source_forest==13&&map.source_mountain==55&&map.unknown==17);
   CHECK(map.raw[7*16+8]==3&&map.raw[7*16+9]==7);
   CHECK(map.raw[0]==7&&map.raw[15]==0x82);
   FILE *f=fopen(argv[1],"rb");CHECK(f!=NULL);
   CHECK(fread(ram,1,sizeof ram,f)==sizeof ram);CHECK(fclose(f)==0);
   /* Hostile mutation must reject and preserve the proven prior map fully. */
   Gk69MesenMap exact=map;
   ram[GK69_MAP_OFFSET+7*16+8]=0;
   CHECK(gk69_map_from_ram(ram,sizeof ram,&map)==GK69_MAP_NOT_EXACT_CAPTURE);
   CHECK(memcmp(&map,&exact,sizeof map)==0);
 }
 printf("GK69 RAM MAP %u/%u assertions PASS; private_source=%s\n",checks,checks,argc==2?"MATCHED LIVE MODIFIED NES CAPTURE":"NOT PROVIDED");
 return 0;
}
