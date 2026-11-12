#include "gk209_archived_rate_ambiguity.h"
#include <string.h>
Gk209Status gk209_archived_rate_ambiguity(const char *rom,const char *mss,
 Gk77Profile profile,uint32_t window,Gk209RateAmbiguity *out){
 if(!rom||!*rom||!mss||!*mss||!out||window==0u||window>65536u||
    (profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK209_ARGUMENT;
 Gk208ArchiveProof source={0};
 Gk208Status status=gk208_archived_source_rng(rom,mss,profile,&source);
 if(status!=GK208_OK)
  return (status==GK208_ARCHIVE||status==GK208_ARCHIVE_IDENTITY)?
      GK209_ARCHIVE_REFUSED:GK209_SOURCE_REFUSED;
 /* Source arithmetic produces odd output seed; even Mesen state could be
  * mid-calculation or externally altered. Do not pretend it is an output. */
 if(!(source.archived_seed48&UINT64_C(1)))return GK209_NOT_A_DRAW_BOUNDARY;
 Gk209RateAmbiguity candidate={0};
 candidate.profile=profile;
 candidate.archived_pc=source.archived_pc;
 candidate.window=window;
 candidate.observed_spring_rate=(profile==GK77_ARCHIVED_WORLD_SAVERAM)?114u:101u;
 for(unsigned d=1;d<=5u;d++){
  Gk138RateScan scan={0};
  if(gk138_scan_spring_rate(source.archived_seed48,(uint8_t)d,
       candidate.observed_spring_rate,window,&scan)!=GK138_OK)
   return GK209_NOT_A_DRAW_BOUNDARY;
  candidate.by_difficulty[d-1u]=scan;
  if(scan.matches>0u)candidate.candidate_difficulties++;
  if(scan.matches==1u)candidate.uniquely_located_difficulties++;
 }
 /* Neither a single candidate nor an apparent unique match establishes
  * an authentic 6502 call chronology. Always mark both gates false. */
 *out=candidate;
 return GK209_OK;
}
