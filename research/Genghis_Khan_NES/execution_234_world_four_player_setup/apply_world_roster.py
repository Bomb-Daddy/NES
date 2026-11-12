#!/usr/bin/env python3
"""EXEC0234 bounded overlay, on top of COMPLETE inherited EXEC0233 native app.
The base 0230 source and chronological 0231/0232/0233 overlays are prerequisites.
No original ROM, checkpoints, graphics or audio are copied or published.
"""
from pathlib import Path
import sys

BEFORE = '''   label(113,337,"ALLOWED WORLD: GENGHIS / RICHARD / ALEXIOUS / YORITOMO",EDGE);
  }else if(stage==GK53_SETUP_ABILITIES){'''
AFTER = '''   label(113,337,"ALLOWED WORLD: GENGHIS / RICHARD / ALEXIOUS / YORITOMO",EDGE);
   /* EXEC0234: real 1-4 player setup audit on the gameplay screen.
    * Show the committed human lords only. Unchosen slots remain blank;
    * never imply an initialized ruler, original ROM RNG, or Spring turn. */
   if(w==1u){
    char picked[4][8];
    for(unsigned j=0;j<4u;j++){
     uint8_t ix=app.setup_rulers[1][j];
     if(j>=app.setup_player_count[1]||ix==255u)
      snprintf(picked[j],sizeof picked[j],"--");
     else snprintf(picked[j],sizeof picked[j],"%u",(unsigned)ix+1u);
    }
    snprintf(line,sizeof line,"COMMITTED WORLD LORD COUNTRIES: P1 %s / P2 %s / P3 %s / P4 %s",
     picked[0],picked[1],picked[2],picked[3]);
    label(113,372,line,EDGE);
   }
  }else if(stage==GK53_SETUP_ABILITIES){'''
APPEND = '''
# EXEC0234: the same native controller/app 27-country, four-ruler World flow.
# This is a genuine compiled game-state regression, not original Spring parity.
add_executable(test_gk234_world_four_players test_gk234_world_four_players.c)
target_link_libraries(test_gk234_world_four_players PRIVATE gk1_manual_setup_store gk1_pad_router)
add_test(NAME GK1_234_NativeWorldFourPlayerSetupAndRollback
 COMMAND test_gk234_world_four_players ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def apply(root: Path, donor: Path):
    base=root/'execution_199'
    shell=base/'gk1_native_shell.c'
    cmake=base/'CMakeLists.txt'
    target=base/'test_gk234_world_four_players.c'
    # Fail closed BEFORE any file modification. Replays fail on duplicated or
    # already-applied markers, changed ancestor source or existing test file.
    source=shell.read_text(encoding='utf-8')
    build=cmake.read_text(encoding='utf-8')
    tests=donor.read_bytes()
    if (source.count(BEFORE)!=1 or 'EXEC0234:' in source or
        build.count('GK1_234_NativeWorldFourPlayerSetupAndRollback')!=0 or
        target.exists() or len(tests)<2000 or b'EXEC0234 World four-player' not in tests):
        raise ValueError('missing, changed, duplicate or preinstalled source; refused')
    shell.write_text(source.replace(BEFORE,AFTER,1),encoding='utf-8',newline='')
    cmake.write_text(build+APPEND,encoding='utf-8',newline='')
    target.write_bytes(tests)

if __name__ == '__main__':
    try:
        if len(sys.argv)!=2: raise ValueError('usage: apply_world_roster.py SOURCE_ROOT')
        apply(Path(sys.argv[1]),Path(__file__).with_name('test_gk234_world_four_players.c'))
        print('EXEC0234 installed original World four-player roster HUD and native C11 controller hostile QA')
    except (OSError,ValueError) as e:
        print('EXEC0234 REFUSED: '+str(e),file=sys.stderr)
        sys.exit(1)
