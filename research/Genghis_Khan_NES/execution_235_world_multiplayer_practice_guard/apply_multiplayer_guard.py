#!/usr/bin/env python3
"""EXEC0235: prevent a four-human World setup from silently becoming single-ruler Enhanced practice.
Applies only to the exact inherited EXEC0234 native C11 game; no stock ROM or original data.
"""
from pathlib import Path
import sys
BASE='execution_199'
OLD1=''' uint8_t w=s->active_world;
 uint8_t player=s->setup_candidate[w];
 if(player>=s->worlds[w].country_count)return GK222_BAD_PLAYER;'''
NEW1=''' uint8_t w=s->active_world;
 /* EXEC0235: the Enhanced finance practice ledger has exactly ONE active
  * ruler and ONE three-order budget. World setup may legitimately nominate
  * 2-4 human rulers, but treating player one as everybody's campaign would
  * silently discard the other players. Refuse before mutating any game state.
  * This is NOT an original-stock Spring implementation. */
 if(s->setup_player_count[w]!=1u)return GK222_BAD_SOURCE;
 uint8_t player=s->setup_candidate[w];
 if(player>=s->worlds[w].country_count)return GK222_BAD_PLAYER;'''
OLD2='''     if(result!=GK222_OK)
       strcpy(s->status,"ENHANCED FINANCE PRACTICE REFUSED; SOURCE IS INVALID");'''
NEW2='''     if(result!=GK222_OK)
       strcpy(s->status,(w==1u && s->setup_player_count[w]>1u)?
         "WORLD MULTI-HUMAN SPRING NOT IMPLEMENTED: ALL RULERS PRESERVED":
         "ENHANCED FINANCE PRACTICE REFUSED; SOURCE IS INVALID");'''
APPEND='''
# EXEC0235: fail-closed multi-human World setup to Enhanced-only practice guard.
add_executable(test_gk235_world_multi_guard test_gk235_world_multi_guard.c)
target_link_libraries(test_gk235_world_multi_guard PRIVATE gk1_app_state gk1_pad_router)
add_test(NAME GK1_235_WorldMultiplayerPracticeGuard
 COMMAND test_gk235_world_multi_guard ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''
def transform(src,cm):
 if src.count(OLD1)!=1 or src.count(OLD2)!=1 or 'EXEC0235:' in src or 'GK1_235_' in cm:
  raise ValueError('missing, changed or previously applied anchor')
 return src.replace(OLD1,NEW1,1).replace(OLD2,NEW2,1),cm+APPEND
def install(root,donor):
 base=root/BASE; src=base/'gk1_app_state.c';cm=base/'CMakeLists.txt';out=base/'test_gk235_world_multi_guard.c'
 if out.exists():raise ValueError('test already exists')
 old=src.read_text();build=cm.read_text();a,b=transform(old,build)
 payload=donor.read_bytes()
 if b'EXEC0235' not in payload or len(payload)<2000:raise ValueError('test donor incomplete')
 src.write_text(a,encoding='utf-8',newline='');cm.write_text(b,encoding='utf-8',newline='');out.write_bytes(payload)
if __name__=='__main__':
 try:
  if len(sys.argv)!=2:raise ValueError('usage: apply_multiplayer_guard.py HYDRATED_ROOT')
  install(Path(sys.argv[1]),Path(__file__).with_name('test_gk235_world_multi_guard.c'))
  print('EXEC0235 installed exact-source native World multi-human practice guard')
 except (ValueError,OSError) as e:
  print('EXEC0235 REFUSED '+str(e),file=sys.stderr);raise SystemExit(1)
