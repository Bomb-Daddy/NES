#include "gk81_stock_attach.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static const char *file_for(Gk77Profile p){
 switch(p){
  case GK77_FRESH_MONGOL_WORKRAM:return "stock_fresh_mongol_1175.records";
  case GK77_ARCHIVED_MONGOL_SAVERAM:return "stock_archived_mongol_1175.records";
  case GK77_ARCHIVED_WORLD_SAVERAM:return "stock_archived_world_1206.records";
  default:return NULL;
 }
}
const char *gk81_stock_filename(Gk77Profile p){return file_for(p);}
static int root_ok(const char *root){
 if(!root||!*root)return 0;
 /* Absolute Linux or Windows drive/network share; never fall back to CWD. */
 if(root[0]!='/'){
  if(strlen(root)<3)return 0;
  if(!((root[0]>='A'&&root[0]<='Z')||
       (root[0]>='a'&&root[0]<='z'))||root[1]!=':'||
       (root[2]!='\\'&&root[2]!='/')){
   if(!(root[0]=='\\'&&root[1]=='\\'))return 0;
  }
 }
 if(strstr(root,"/../")||strstr(root,"\\..\\")||
    (strlen(root)>=3&&strcmp(root+strlen(root)-3,"/..") ==0)||
    (strlen(root)>=3&&strcmp(root+strlen(root)-3,"\\..") ==0))return 0;
 return 1;
}
int gk81_stock_path(const char *root,Gk77Profile p,char *out,size_t cap){
 const char *name=file_for(p);
 if(!name||!root_ok(root)||!out||cap<4)return 0;
 const size_t n=strlen(root),m=strlen(name);
 if(n>2048||n+m+sizeof("/GK1NativeResearch/")>=cap)return 0;
 char sep=strchr(root,'\\')?'\\':'/';
 int ended=(root[n-1]=='/'||root[n-1]=='\\');
 /* Must not alter output on failed validation. */
 int wrote=snprintf(out,cap,"%s%sGK1NativeResearch%c%s",root,ended?"":(sep=='/'?"/":"\\"),sep,name);
 return wrote>=0&&(size_t)wrote<cap;
}
Gk81AttachResult gk81_stock_attach(const char *root,
 const char *explicit_path,Gk77Profile explicit_profile,
 Gk77Profile requested,Gk77StockSpring *source,char *selected,size_t cap){
 if(!source||!selected||!cap||!file_for(requested))return GK81_ATTACH_INVALID;
 char path[2300];const char *file=NULL;
 if(explicit_path&&*explicit_path){
  if(requested!=explicit_profile)return GK81_ATTACH_INVALID;
  if(strlen(explicit_path)>=sizeof path)return GK81_ATTACH_INVALID;
  file=explicit_path;
 }else{
  if(!gk81_stock_path(root,requested,path,sizeof path))return GK81_ATTACH_INVALID;
  file=path;
 }
 const size_t len=strlen(file);
 if(len+1>cap)return GK81_ATTACH_INVALID;
 Gk77StockSpring *candidate=malloc(sizeof *candidate);
 if(!candidate)return GK81_ATTACH_INVALID;
 Gk77Result g=gk77_load(file,requested,candidate);
 if(g==GK77_OK){*source=*candidate;memcpy(selected,file,len+1u);}
 free(candidate);
 return g==GK77_OK?GK81_ATTACH_OK:g==GK77_IO?GK81_ATTACH_NOT_FOUND:GK81_ATTACH_BAD_SOURCE;
}
