"""Source-shaped six-command battle navigation, separate from campaign economics.

Pure navigation state. The action backend remains the real Enhanced Campaign;
this layer adds no combat formulas, damage rules or RNG draws.
UI parity is only PARTIAL until exact prompts/phases are source-replayed.
"""
from dataclasses import dataclass

ROOT = ('Move', 'Attack', 'Flee', 'View', 'Tactics', 'Wait')
SUBMENUS = {
    'Move': ('Normal', 'Move enemy'),
    'Attack': ('Simult.', 'Normal', 'Fire', 'Charge'),
    'Flee': ('Officer', 'Whole army'),
    'View': ('Officer', 'Battle summary', 'Prisoner fate'),
    'Tactics': ('Reinforce', 'Switch sides', 'Duel'),
    'Wait': ('Wait officer', 'End day'),  # end-day compatibility until source-timed auto advance
}

@dataclass
class NESBattleMenu:
    branch: str | None = None
    index: int = 0

    @property
    def labels(self):
        return SUBMENUS[self.branch] if self.branch else ROOT

    def select(self, direction):
        if direction not in (-1, 1):
            raise ValueError('Menu movement must be -1 or +1')
        self.index = (self.index + direction) % len(self.labels)
        return self.labels[self.index]

    def enter(self):
        option = self.labels[self.index]
        if self.branch is None:
            self.branch = option
            self.index = 0
            return None
        return self.branch, option

    def back(self):
        if self.branch is None:
            return False
        old_branch = self.branch
        self.branch = None
        self.index = ROOT.index(old_branch)
        return True

    def reset(self):
        self.branch = None
        self.index = 0
