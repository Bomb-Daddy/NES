#!/usr/bin/env python3
"""Compile ACTUAL C11 predicate extracted from public inherited source.

Negative control must be observed: prior fresh-title bug accepted a corrupted
wizard. Patched predicate rejects it and retains known fresh chooser route.
No NES ROM or private checkpoint is consumed.
"""
import pathlib,subprocess,sys,tempfile,hashlib
ROOT=pathlib.Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT))
import stage310 as s
SOURCE=pathlib.Path(sys.argv[1]) if len(sys.argv)>1 else pathlib.Path('/mnt/data/GK1_EXEC0276_gk162_original_entry.c.txt')
c=SOURCE.read_text()
# The predicate is identical by intended unchanged anchor from public 0276
# through post-0309; a CI staging mismatch must FAIL CLOSED.
start=c.index('static int pristine_title(')
end=c.index('Gk162EntryStatus gk162_enter_original_checkpoint(',start)
predicate=c[start:end]
assert predicate.count(s.BEFORE)+predicate.count(s.AFTER)==1
header=r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK53_SETUP_PLAYERS 0u
#define GK53_SETUP_AWAIT_SOURCE 4u
#define GK77_FRESH_MONGOL_WORKRAM 1u
#define GK77_ARCHIVED_MONGOL_SAVERAM 2u
#define GK77_ARCHIVED_WORLD_SAVERAM 3u
typedef unsigned Gk77Profile;
typedef struct {
 uint8_t setup_stage[2],setup_candidate[2],setup_player_count[2];
 uint8_t setup_difficulty[2],setup_player_slot[2],setup_ability_method[2];
 uint8_t setup_method_cursor[2],manual_bonus_active[2],manual_bonus_person[2];
 uint8_t manual_bonus_ability[2],setup_rulers[2][4],manual_bonus[2][5][6];
 uint8_t active_world;
} Gk1App;
'''
body=r'''
static void init(Gk1App *a){
 memset(a,0,sizeof *a);
 for(unsigned w=0;w<2;w++){
   a->setup_candidate[w]=255;a->setup_player_count[w]=1;
   a->setup_difficulty[w]=1;
   for(unsigned i=0;i<4;i++)a->setup_rulers[w][i]=255;
 }
}
#define CK(x) do{if(!(x)){fprintf(stderr,"fail C11 line %d: %s\\n",__LINE__,#x);return 2;}}while(0)
int main(void){
 Gk1App a;init(&a);
 CK(gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 CK(gk162_original_setup_compatible(&a,GK77_ARCHIVED_MONGOL_SAVERAM));
 CK(gk162_original_setup_compatible(&a,GK77_ARCHIVED_WORLD_SAVERAM));
 init(&a);a.setup_candidate[0]=0;
 CK(!gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 init(&a);a.setup_rulers[0][0]=0;
 CK(!gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 init(&a);a.setup_difficulty[0]=2;
 CK(!gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 init(&a);a.manual_bonus[0][0][0]=1;
 CK(!gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 init(&a);a.setup_player_count[0]=2;
 CK(!gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 init(&a);a.active_world=0;a.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 a.setup_player_slot[0]=1;a.setup_candidate[0]=0;a.setup_rulers[0][0]=0;
 a.setup_ability_method[0]=1;
 CK(gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 a.setup_difficulty[0]=2;
 CK(!gk162_original_setup_compatible(&a,GK77_FRESH_MONGOL_WORKRAM));
 puts("C11 compiled original-stock setup identity controls PASS");return 0;
}
'''
def execute(code,tag):
 with tempfile.TemporaryDirectory() as td:
  td=pathlib.Path(td);src=td/'check.c';exe=td/'check'
  src.write_text(header+code+body)
  for opts in (['-O2'],['-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer']):
   cmd=['cc','-std=c11','-Wall','-Wextra','-Werror',*opts,str(src),'-o',str(exe)]
   compile=subprocess.run(cmd,capture_output=True,text=True)
   if compile.returncode:
    print(compile.stderr);return False
   result=subprocess.run([str(exe)],capture_output=True,text=True)
   print(tag,opts, 'exit='+str(result.returncode),result.stdout.strip(),result.stderr.strip()[:180])
   if tag=='old':
    if result.returncode==0:return False
   else:
    if result.returncode!=0:return False
 return True

old=predicate.replace(s.AFTER,s.BEFORE)
new=predicate.replace(s.BEFORE,s.AFTER)
if not execute(old,'old'):raise SystemExit('negative control failed')
if not execute(new,'patched'):raise SystemExit('patched control failed')
print('EXEC0310 isolated exact C11 source negative-control + patch PASS')
