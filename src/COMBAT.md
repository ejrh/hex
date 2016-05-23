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
opportunity to make an attack.  The battle proceeds in phases, starting in the Charge phase.

  - Charge: Units with the Charge ability may make an attack.

  - Ranged defence: Units with ranged abilities (Archery, etc.) may make an attack.

  - Melee: All units may attack.

The battle continues until one side is defeated or the attacking side has not damaged the
defending side in one complete turn.

The result of simulating a battle is a sequence of attacks.

Attacks
-------

An attack describes the unit making the attack, the target (if any), the ability used in the
attack, and the effect (e.g. damage).
