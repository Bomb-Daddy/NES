/* EXEC0240: build-contract test for optional Mesen inflater.
 * No copyrighted source bytes, no owner paths, no artificial successful import.
 * It deliberately differentiates no-zlib from a zlib-enabled build.
 */
#include "gk182_archived_source_bank.h"
#include "gk163_mesen_import.h"
#include <stdio.h>
#include <string.h>
int main(void){
 Gk182SavedBankProof proof;
 memset(&proof,0x5a,sizeof proof);
 if(gk182_original_saved_bank(NULL,"/missing/private.mss",
    GK77_ARCHIVED_WORLD_SAVERAM,&proof)!=GK182_ARGUMENT)return 1;
 if(gk182_original_saved_bank("/missing/private.nes",NULL,
    GK77_ARCHIVED_WORLD_SAVERAM,&proof)!=GK182_ARGUMENT)return 2;
 if(gk182_original_saved_bank("/missing/private.nes","/missing/private.mss",
    (Gk77Profile)99,&proof)!=GK182_ARGUMENT)return 3;
 Gk182Status status=gk182_original_saved_bank("/missing/private.nes",
    "/missing/private.mss",GK77_ARCHIVED_WORLD_SAVERAM,&proof);
#ifdef GK163_HAS_ZLIB
 if(status!=GK182_SOURCE)return 4;
 puts("EXEC0240 PASS: zlib build refuses missing original source; no private input used");
#else
 if(status!=GK182_UNAVAILABLE)return 5;
 puts("EXEC0240 PASS: no-zlib build explicitly refuses original importer");
#endif
 /* Both entry points reject absent private source and do not mutate output. */
 unsigned char records[GK163_MAX_RECORDS];
 memset(records,0xc3,sizeof records);
 size_t count=17u;
 Gk139MesenProof cpu;
 memset(&cpu,0xa1,sizeof cpu);
 Gk163Status unpack=gk163_mesen_extract_with_proof("/missing/private.mss",
    GK77_ARCHIVED_WORLD_SAVERAM,records,&count,&cpu);
#ifdef GK163_HAS_ZLIB
 if(unpack!=GK163_IO)return 7;
#else
 if(unpack!=GK163_UNAVAILABLE)return 8;
#endif
 if(count!=17u)return 9;
 for(size_t i=0;i<sizeof records;i++)if(records[i]!=0xc3)return 10;
 const unsigned char *cp=(const unsigned char *)&cpu;
 for(size_t i=0;i<sizeof cpu;i++)if(cp[i]!=0xa1)return 11;
 /* Failure must not hand back fabricated source proof. */
 const unsigned char *b=(const unsigned char *)&proof;
 for(size_t i=0;i<sizeof proof;i++)if(b[i]!=0x5a)return 6;
 return 0;
}
