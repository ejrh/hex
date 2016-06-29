Combat Notes
============

Battles
-------

A battle is initiated when a stack attacks a neighbouring tile.  The attacked tile and its
surrounding tiles form the field of battle.  All stacks within the field are eligible to be
involved in the battle.

  - A stack allied with the attacker is on the attacking side.

  - A stack allied with the owner of the attacked tile is on the defending side.

A stack allied with both sides, or neither, will not be involved in the battle.

The battle is simulated as a series of turns.  In each turn, each involved unit may have the
opportunity to make an attack.  In a unit's turn, it considers all other participants (including
friendly ones and itself) and all available move types.  The best move and target is chosen.

The battle continues until one side is defeated or the attacking side has not damaged the
defending side for several turns.

The result of simulating a battle is a sequence of attacks.  When a battle is committed, its
results are saved back to the game state.

Move Types
----------

The available move types are registered in the combat model.  Each move type has a number of
methods:

  - `is_viable` - Is it possible to make this move?
  - `expected_value` - What is the expected value of making this move?  (in approximately units of
     damage to the target)
  - `repeats` - How many times does the move get made?
  - `generate` - Generate a move using this move type
  - `apply` - Apply a move generating using this move type

The first three are purely functional but the return values of `expected_value` and `generate`
assume that is_viable returned true.

`apply` will update the battle units' state when called with a move.

Some move types:

  - Strike - damages a target using a standard damage roll
  - Charge - can be used only in first turn, adds +2 to attack
  - Healing - heals up to 5 health of a friendly target

Damage roll
-----------

A damage roll generates a random damage value using the unit's attack and damage stats, and the
target's defence stat.

    attack_value = random(0 to attack)
    defence_value = random(0 to defence)
    if (attack_value > defence_value)
        damage_value = random(0 to damage)
    else
        damage_value = 0

Riposte
-------

After moves a Riposte move may be generated.  This move is made by the target in reponse to a
move.  A riposte is viable if the last move was of melee type (such as Strike), and the target
has the Strike ability.  The target and participant of the last move are switched for the
following Riposte.

Riposte is similar to Strike in terms of effect, except that the attack and damage scores are
reduced by 1.

Moves
-----

A move describes the unit making the attack, the target (if any), the move type used in the
attack, and the effect (e.g. damage).  Moves are serialised as simple tuples using participant ids
and move type names.
