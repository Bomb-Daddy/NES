"""First-time-player contextual explanations for real source commands.

The catalog is deliberately non-executable. Read only; opening Help never
previews, commits, advances RNG, consumes Body Points or mutates campaign state.
"""
from __future__ import annotations

ORDER_HELP = {
    'Allocate Workers': ('Move a specific number of workers from one job to another within this province. Choose the source workforce, destination job, and number.','Only available workers may be reassigned. Preview first; no workers move until you confirm.'),
    'Distribute Food': ('Give government-held food to local residents to support the province. Enter the number of units.','This is an explicit order, not an automatic monthly subtraction from the government store. The preview shows whether the order is allowed.'),
    'Distribute Goods': ('Distribute government-held specialty goods to local residents. Choose the amount.','You cannot distribute more than you actually own. The preview reports the effect and applicable costs.'),
    'Recruit': ('Recruit soldiers in the selected province. Enter how many you want to recruit and choose an eligible officer.','Your province needs sufficient resources and eligible command capacity. Preview shows the exact legal result.'),
    'Recruit Officer': ('Attempt to bring an available historical officer into your service. Choose the officer ID shown in the roster.','Only officers who are actually available can be recruited; not every historical officer is present in every starting province.'),
    'Assign Troops': ('Move existing soldiers between eligible local officer commands. Pick a target officer and troop count.','This does not create soldiers. Both the source and destination officer must remain legal after the transfer.'),
    'Disband Soldiers': ('Release a specified number of soldiers from your active army. This reduces your military force.','Review troop and population changes in the preview. This action is not an attack or movement order.'),
    'Move Army': ('Move a legal army and its commander into an adjacent friendly OR vacant province. Vacant entry uses the same Move Army order.','Troops and commanders travel from a real source location; the move cannot teleport people across the map.'),
    'Market': ('Buy or sell food, specialty goods, Metal, Arms or other available commodities with the merchant. Set commodity, buy/sell and units.','Quotes vary with season, supply, quantity and negotiation where supported. You must pay for purchases and own what you sell; inspect the quote.'),
    'Train Officer': ('Spend a legal action improving the chosen officer’s selected attribute, such as War or Intelligence. Set effort.','Only the approved five attributes apply; Body Points cannot exceed 200. Preview determines the actual cost and result.'),
    'Train Troops': ('Drill the soldiers of all generals in the selected province; trained units improve their Skill without changing officer War.','This uses the observed NES training gain: sum of floor(2 × selected trainer War / 13), capped at 100. The current single-trainer UI uses the acting general; other trainers may be selected by the engine. Cross-port battle parity remains unverified.'),
    'Equip Arms': ('Issue actual provincial Arms stock to a named local general’s unit, increasing its equipped-soldier coverage.','This is the provisional Enhanced bridge between unified marketplace/manufacturing and original ROTK II per-general Arms. A stock batch represents 100 equipped soldiers; no weapon is free or assigned twice.'),
    'Transport': ('Send a chosen resource or troop quantity to an allowed destination province. Select destination, commodity and units.','The origin must actually own the goods or soldiers and the route must be legal; transports do not duplicate supplies.'),
    'Reward Governor': ('Show every appointed governor, their province and current loyalty; the ruler may reward one directly.','Gold or a horse comes from the ruler’s own province and uses the ruler’s Body Points. This experimental command never changes provincial control or grants a governor any special authority.'),
    'Reward Officer': ('Reward a local officer with gold, a horse or writings. Gold and horses increase officer loyalty; writings improve Intelligence if an appointed advisor qualifies.','Only the ruler or local governor may order rewards. Gold must be in the province treasury; horses must have been purchased; writing rewards need an appointed higher-Intelligence advisor. Effects are experimental, not proven NES formulas.'),
    'Buy Horse': ('Purchase horses to use as officer rewards.','Each horse costs 100 gold, matching the ROTK II manual. Temporary specialty purchase until the unified marketplace exposes horses as a standard commodity.'),
    'Appoint Advisor': ('Appoint a qualified local officer as advisor.','Requires ruler and an officer with Intelligence above 80. Writing rewards require a local appointed advisor.'),
    'Swear Brotherhood': ('Bind a local officer to your ruler as a sworn sibling, Bandit Kings style.','Requires loyalty at least 95 and the ruler present. Loyalty becomes 100. The game already grants direct appointment and direct provincial command independently of brotherhood; no province-control gate is planned.'),
    'Feast': ('Spend local treasury gold on a feast for all local officers.','Experimental effect: 10 gold per attendee, 5 Body Points recovered and a small loyalty gain for nonsworn officers. Original Bandit Kings compatibility effects remain to be verified.'),
    'Rest': ('Recover Body Points for local officers without spending gold.','Experimental effect: 10 Body Points per local officer, up to 200. Uses a strategic action.'),
    'Appoint Governor': ('Assign an eligible officer as the province governor and choose a practical management focus.','A governor must belong to the faction and be legally present. The new experimental Personnel focus uses actual treasury gold and governor Body Points to improve the lowest-loyalty local officer each month when affordable.'),
    'Plan Attack': ('Set a military objective against a valid neighboring enemy. This is preparation and does not by itself win a battle.','You still need real troops, supplies and a legal Attack order. A plan may be abandoned when conditions change.'),
    'Attack': ('Order eligible soldiers to assault a neighboring hostile province. Choose the target and force size.','This enters the tactical battle system. Defending armies and occupied castle tiles must be resolved by actual battle rules.'),
    'Ally': ('Propose an alliance to another active ruler faction. Diplomatic acceptance is not guaranteed.','An ordinary alliance lasts twelve months and renews only if both factions independently agree.'),
    'Joint': ('Request a joint military operation with an ally. Choose your partner, target and promised troops.','The required alliance and promises must be valid; planning does not magically transfer or create soldiers.'),
    'Marry': ('Propose a marriage alliance if the historical family and eligibility conditions actually permit it.','A ruler without a documented eligible relation cannot use the order. The game will tell you why a request is rejected.'),
    'Give': ('Offer a chosen amount of treasury gold to another faction as diplomacy.','The gold must exist in your treasury. Check the quoted result before you authorize the transfer.'),
    'Cancel Alliance': ('End an existing agreement with the selected faction. This may remove protection against attacks.','You may deliberately end a treaty early. A human should see a clear warning before a hostile breach is committed.'),
    'Threat': ('Attempt to pressure another ruler faction through diplomacy. Select the target faction.','A threat can be refused and may affect relations under current game rules. Preview the legal effects instead of assuming a guaranteed result.'),
}
STRATEGIC_HELP = {
    'Map': ('View current China province ownership while other rulers act or when manually requested.',
            'D-pad or click highlights a province; A (or clicking it again) opens that province. Your own province opens with its commands; any other province opens read-only with a single Exit command. B or Select returns to your province.'),
    'Province target selection': ('Select a target province for the order that opened this map.',
                                  'Highlight the province with D-pad or click, press A to return to the pending order. No game order is committed on the map.'),
}
PROVINCE_HELP = {
    'Domestic': ('Assign civilian workers, distribute food and other goods.',
                 'Commands preview and commit real ledger changes. Direct Cultivate is not an approved command.'),
    'Military': ('Recruit, train and equip soldiers; move armies, transport supplies and begin attacks.',
                 'General-commanded armies observe the 100-group cap and defend source provinces.'),
    'Troop Assignments': ('Assign real soldiers among actual stationed generals.',
                          'A transfer must be legal and conserve troops and military arms.'),
    'Personnel': ('Recruit and train officers and manage appointments, rewards and relationships.',
                  'Only eligible officers and current rulers may issue restricted commands.'),
    'Market': ('Buy and sell actual commodities including weapons through the unified market.',
               'Quotes, gold, inventories and equipped weapons must be conserved.'),
    'Foreign Affairs': ('Negotiate with rulers through real diplomacy commands.',
                        'The target picker chooses a province and resolves its ruler; no order is executed on the China map.'),
    'Information': ('Read the actual saved campaign journal and province state.',
                    'Information screens never issue or commit orders.'),
    'Advisor': ('Read genuine monthly advice and six-month saved-state changes.',
                'Advice is information and is not automatically executed.'),
    'Save / Load': ('Save the campaign to a slot or cold-load an existing game.',
                    'Never replace a valid save until the new data is safely written.'),
    'Options / Help': ('Read contextual controls and help for the current screen.',
                       'Actual configured Xbox controller buttons must be accessible without a mouse.'),
    'End Month': ('Finish player orders and watch opponents process their monthly turns.',
                  'China map appears only during the real calculation and returns to the same province.'),
    'Back to Strategic Map': ('Open the China map to look at, or open, any province.',
                              'Highlight a province and press A to open it. Other rulers\' provinces are read-only with an Exit command; B or Select returns to your province.'),
}

TROOP_HELP = {
    'Transfer Between Generals': ('Move soldiers already assigned to one stationed general to another local general.',
                                  'No new soldiers are created. Select both generals and a legal count; preview before committing.'),
    'Disband to Population': ('Return soldiers from the selected general to the civilian population.',
                             'Choose a legal positive count and confirm. The command must conserve soldiers and population.'),
    'Action': ('Choose Transfer Between Generals or Disband to Population.',
               'Recruit and Move Army are separate Military commands, not duplicate Troop Assignment controls.'),
    'General': ('Select the stationed friendly general whose soldiers will be transferred or disbanded.',
                'The selected general must command enough actual troops.'),
    'To General': ('Choose the destination general for a troop transfer.',
                   'The receiving general must be present and must remain under the 100-group limit.'),
    'Troop Count': ('Choose how many existing soldiers to transfer or disband.',
                    'Left/Right changes the count; Select changes the adjustment step. A previews and confirms.'),
    'Preview / Commit': ('Preview the current troop order and confirm it with a second A press.',
                         'B cancels without changing soldiers, Body Points or the saved world.'),
}

BATTLE_HELP = {
    'Move': ('Move your selected troop unit to a legal tactical hex.','Choose a destination with the D-pad and press A; occupied tiles are not freely enterable.'),
    'Attack': ('Attack an eligible enemy unit from a legal battle position.','Combat strength and surviving defenders determine the actual result.'),
    'End Turn': ('Finish your current tactical phase and let the battle advance.','Positions, losses and morale persist in the same campaign.'),
    'Auto Resolve': ('Let the existing battle rules resolve further fighting automatically.','Auto must obey the same unit collision and occupied-castle rules as manual orders.'),
    'Close': ('Close the battle command window.','You can return to this saved battle until it has actually ended.'),
    'Next Officer': ('Select another eligible officer commanding troops on your side.','The selection changes your tactical actor; it does not spend a turn or move soldiers.'),
    'Next Target': ('Select a surviving enemy officer unit as the target for an attack, bribe, or tactical action.','Only currently eligible opposing units appear; selection alone does not commit an action.'),
    'Rest': ('Have the selected officer wait during the current tactical turn.','This is a battle Wait order, not the province Personnel Rest command.'),
    'Move Enemy': ('Attempt the original tactical Move Enemy action against a selected opposing officer.','Choose the enemy first, then a target hex with the D-pad and A; the engine checks legality before committing.'),
    'Bribe': ('Offer gold to a selected opposing officer during battle.','Select an enemy with Next Target; Left/Right adjusts gold and Select cycles offers. The engine may reject the bribe.'),
    'Duel': ('Challenge an enemy officer to a duel or answer an opening enemy challenge.','On an incoming challenge, Select toggles Accept/Refuse and A confirms; the engine resolves the duel outcome.'),
    'Reinforce': ('Bring an eligible friendly officer from an adjacent owned province into the battle.','Select cycles eligible stationed officers; Left/Right adjusts rice. Troops and supplies must exist at the source.'),
    'Withdraw': ('Retreat the selected officer or whole army to an eligible adjacent province.','Select cycles legal destinations and Left/Right toggles whole-army withdrawal. The move must preserve real troop stations.'),
    'Prisoner Fate': ('Resolve a captured officer after battle using an available disposition.','Select cycles prisoners; Left/Right selects the available decision; A commits. This is separate from battlefield combat.'),
    'Normal': ('Make a normal attack with the selected officer against a legal enemy.','The battle engine calculates damage, terrain, casualties and action eligibility.'),
    'Charge': ('Use the Charge attack mode with the selected officer.','The battle engine checks the mode, target, remaining action and applicable combat rules.'),
    'Simult.': ('Use the simultaneous attack mode when permitted.','Select an eligible enemy first; the engine determines whether both sides can attack and how losses settle.'),
    'Fire': ('Attempt a fire attack against a legal target.','Weather and terrain can prohibit or extinguish fire. No resources or turns are consumed on rejected orders.'),
}
DEFAULT_HELP = ('Read what the selected option does before confirming. Nothing changes while Help is open.', 'Use A to select or B to cancel. Save and load will preserve committed campaign changes.')

def help_for(topic, surface='strategic', field=None):
    catalog={'strategic':STRATEGIC_HELP,'orders':ORDER_HELP,'province':PROVINCE_HELP,'battle':BATTLE_HELP,'troops':TROOP_HELP}.get(surface,{})
    first,second=catalog.get(topic, DEFAULT_HELP)
    if field:
        second += f' Current field: {str(field).replace("_", " ")}. Adjust this value before previewing.'
    return str(topic), first+'\n\n'+second

def check_help_coverage():
    from .orders_ui import SPECS
    from .province_stage import NAV
    assert set(SPECS)==set(ORDER_HELP)
    assert set(NAV)==set(PROVINCE_HELP)
    return True
MENU_HELP = {
    'Resume': ('Close the pause menu and continue the current game.','No orders or months advance until you explicitly confirm a game action.'),
    'Save Game': ('Choose one of five numbered slots and save the current historical campaign.','Saving must retain your chosen human ruler and current month. Existing slots ask before being overwritten.'),
    'Load Game': ('Choose a numbered save slot to return to that recorded campaign.','Your selected human ruler, provinces, officers and calendar are restored from the same save.'),
    'Continue Last Save': ('Reload the most recent active campaign file.','If this file is damaged the load is rejected without changing the current game.'),
    'Return to Main Menu': ('Leave the current game and return to the opening controls.','Confirm the choice; save first if you want to preserve recent changes.'),
    'Exit Game': ('Close the program.','Save before exiting if you want to continue this campaign later.'),
    'Audio Status': ('Check whether music and sound effects are available.','A playing track in software is not proof of audible sound on a real device.'),
    'Soundtrack Library (15 tracks)': ('Browse the available private music tracks.','These recordings are for this private prototype and have no public redistribution clearance.'),
    'Controller Bindings': ('Reassign supported Xbox controller buttons to game actions.','B and Menu stay reserved for Back and Pause so you can always recover. Press A on a row, then the new controller button.'),
    'Keyboard Bindings': ('Reassign keyboard keys to game actions.','Escape and F10 stay reserved for Back and Pause. Press A on an action, then your new keyboard key.'),
    'Restore Defaults': ('Reset the bindings for this device to the original safe controls.','This does not reset campaign saves, officers, or game settings.'),
    'Back': ('Return to the previous menu.','B on the Xbox controller or Escape on the keyboard also goes back.'),
}
_old_help_for=help_for
def help_for(topic, surface='strategic', field=None):
    if surface=='menu':
        if str(topic).startswith('Slot '):
            entry=('Choose a numbered campaign save slot.','Saving, loading and overwriting follow the choice shown in the menu.')
        elif str(topic).startswith(('Music:','Sound Effects:')):
            entry=('Change the current game audio setting.','The change affects playback, not campaign action points or resources.')
        elif str(topic).startswith(('Up:','Down:','Left:','Right:','A:','B:','Start:','Select:','Help:')):
            entry=('This line displays the currently assigned input for a game action.','Press A to capture a new controller button or keyboard key. Duplicate assignments swap safely; B/Menu and Escape/F10 remain reserved.')
        else:entry=MENU_HELP.get(topic,DEFAULT_HELP)
        return str(topic),entry[0]+'\n\n'+entry[1]
    return _old_help_for(topic,surface,field)
