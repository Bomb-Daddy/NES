"""EXEC0033 -- exact scoped source-profile movement decisions.

The four decisions are specifically from controlled approved Shared BP/Rest NES
checkpoint experiments. This is a fail-closed Windows-port acceptance oracle,
not a general collision resolver, multi-step pathfinder, merge mechanic,
occupant ownership classifier, or compiled Windows component.
"""
from dataclasses import dataclass
from enum import Enum

ROM_SHA256 = '4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5'
CHECKPOINT_SHA256 = 'e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b'

class Occupancy(str, Enum):
    EMPTY = 'empty'
    FRIENDLY_SLOT1 = 'friendly_slot1'
    ENEMY_SLOT10 = 'enemy_slot10'

class Disposition(str, Enum):
    REJECT = 'rejected_at_destination'
    COMMIT_FINISHED = 'committed_and_finished'
    FRIENDLY_COMBINE_OBSERVED = 'friendly_combine_observed'

class UnverifiedMoveState(ValueError):
    pass

@dataclass(frozen=True)
class MoveObservation:
    case: str
    disposition: Disposition
    soldiers0: int
    xy0: tuple[int,int]
    mobility0: int
    soldiers1: int
    xy1: tuple[int,int]
    completion_word: int
    rng_hex: str
    original_phase: str = 'after_idle'

# Each key is complete deliberately: caller may NOT extrapolate to other
# soldier strengths, unit types, tile IDs, enemy ownership or coordinates.
_EXACT = {
  ('insufficient_mobility',40,3,2,Occupancy.EMPTY): MoveObservation(
      'insufficient_mobility',Disposition.REJECT,40,(7,7),2,40,(6,9),0,'c7c3f28f2363'),
  ('exact_mobility',40,3,4,Occupancy.EMPTY): MoveObservation(
      'exact_mobility',Disposition.COMMIT_FINISHED,34,(8,7),10,40,(6,9),1,'a31e4ecd8fd1'),
  ('friendly_occupied',40,3,10,Occupancy.FRIENDLY_SLOT1): MoveObservation(
      'friendly_occupied',Disposition.FRIENDLY_COMBINE_OBSERVED,74,(8,7),10,0,(8,7),2,'a31e4ecd8fd1'),
  ('enemy_occupied',40,3,10,Occupancy.ENEMY_SLOT10): MoveObservation(
      'enemy_occupied',Disposition.REJECT,40,(7,7),10,40,(6,9),0,'c7c3f28f2363'),
}

def known_checkpoint_observation(case: str, soldiers0: int, dest_terrain: int,
                                 mobility0: int, occupancy: Occupancy) -> MoveObservation:
    """Return exact bounded outcome; explicitly refuse every unproven state.

    Four native inputs were controller-tested. Friendly destination is slot1
    originally 40 soldiers, enemy destination slot10 originally 70.
    """
    if type(soldiers0) is not int or type(dest_terrain) is not int or type(mobility0) is not int:
        raise UnverifiedMoveState('typed integer state required, not bool/float')
    if not isinstance(occupancy, Occupancy):
        raise UnverifiedMoveState('explicit occupant identity required')
    key = (case,soldiers0,dest_terrain,mobility0,occupancy)
    if key not in _EXACT:
        raise UnverifiedMoveState('no native completed-command evidence for this exact source-profile state')
    return _EXACT[key]


def verify_observed_phase(observation: MoveObservation, recorded: dict) -> None:
    """Fail when a recorded post-idle source state contradicts the exact oracle."""
    fields = {
        'case':observation.case,'phase':observation.original_phase,
        'rom_sha256':ROM_SHA256,'checkpoint_sha256':CHECKPOINT_SHA256,
        'unit0_soldiers':observation.soldiers0,'unit0_xy':list(observation.xy0),
        'unit0_mobility':observation.mobility0,'unit1_soldiers':observation.soldiers1,
        'unit1_xy':list(observation.xy1),'completion_word':observation.completion_word,
        'rng':observation.rng_hex,'gold':403,'food':198,'target_terrain_byte':3,
    }
    for k,v in fields.items():
        if recorded.get(k)!=v:
            raise AssertionError(f'{observation.case}: native phase field {k} expected {v!r}, got {recorded.get(k)!r}')
