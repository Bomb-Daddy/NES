/* EXEC0242: archive-backend link-and-runtime test. Copyright-clean;
 * synthetic compressed bytes only, never original ROM/Mesen evidence. */
#include "gk241_archived_import_capability.h"
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifndef GK163_HAS_ZLIB
#error "EXEC0242 release-archive test was compiled without required ZLIB"
#endif
#define CHECK(cond) do { if (!(cond)) { \
 fprintf(stderr,"EXEC0242 assertion failed at %d: %s\n",__LINE__,#cond); \
 return 1; } ++checks; } while(0)
int main(void) {
 static const Bytef source[]="GK1-EXEC0242 synthetic-archive-backend 27 countries; not Mesen data";
 Bytef zipped[256], unzipped[sizeof(source)];
 uLongf zipped_size=sizeof zipped;
 uLongf output_size=sizeof unzipped;
 unsigned checks=0;
 CHECK(gk241_archived_import_backend()==GK241_ARCHIVE_BACKEND_ZLIB);
 CHECK(zlibVersion()!=NULL && zlibVersion()[0]!='\0');
 CHECK(compressBound(sizeof source)<=sizeof zipped);
 CHECK(compress2(zipped,&zipped_size,source,sizeof source,Z_BEST_COMPRESSION)==Z_OK);
 CHECK(zipped_size>8 && zipped_size<sizeof source+16);
 CHECK(uncompress(unzipped,&output_size,zipped,zipped_size)==Z_OK);
 CHECK(output_size==sizeof source);
 CHECK(memcmp(unzipped,source,sizeof source)==0);
 zipped[0]^=0x5au;
 output_size=sizeof unzipped;
 CHECK(uncompress(unzipped,&output_size,zipped,zipped_size)!=Z_OK);
#if defined(_WIN32)
 printf("EXEC0242 TESTED_PORT_WIN32 synthetic DEFLATE link/runtime and source backend %u assertions PASS; not physical Xbox or original NES\n",checks);
#else
 printf("EXEC0242 TESTED_PORT_LINUX synthetic DEFLATE link/runtime and source backend %u assertions PASS; Windows not tested by this process\n",checks);
#endif
 return 0;
}
