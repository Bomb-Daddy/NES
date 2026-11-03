"""Atomic player-facing turn orchestrator. AI uses the same legal Campaign dispatcher.

Every opponent phase is staged in a clone, the month is advanced, then the
caller atomically persists that staged result before installing it. This is
not the final strategic AI design or final balance.
"""
from .ai import take_turn
from .historical_ai import take_turn as historical_turn, STYLES


def stage_month_with_opponents(campaign, player='A'):
    """Return (new_campaign, summary), leaving input unchanged including on error.

    No active battle may be skipped by this turn command. Opponent actions
    and the scheduler advance all run in the candidate clone.
    """
    from og.core.fixed import CoreError
    if campaign.state.get('battle'):
        raise CoreError('BATTLE_ACTIVE', 'Resolve the current battle first')
    if player not in campaign.state['factions']:
        raise CoreError('BAD_TARGET', 'Unknown player faction')
    staged = campaign.clone()
    old = staged.state['month']
    outcomes = []
    for opponent in sorted(staged.state['factions']):
        if opponent == player:
            continue
        style = 'opportunistic' if old % 3 == 0 else 'balanced'
        if staged.state.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
            own_style=STYLES[int(opponent[1:])%len(STYLES)]
            result=historical_turn(staged,opponent,own_style,old,prefix='pc-game-han-v3')
        else:
            result = take_turn(staged, opponent, style, prefix='pc-game', index=old)
        outcomes.append(result)
    if staged.state['battle']:
        raise CoreError('AI_BATTLE_ACTIVE', 'AI turn left an unresolved battle')
    result = staged.advance_month()
    return staged, {'month': result, 'opponents': outcomes}
