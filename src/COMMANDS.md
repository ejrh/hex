Command Notes
=============

Movement
--------

Movement is requested using the UnitMove message:

    UnitMove(stack_id, units, path, target_id)

This specifies the stack the units are coming from, the set of units that were selected to move,
the path the units will follow, and the stack (if any) in position at the end of the path.

There are a variety of scenarios:

 1. The full stack is moving to an empty tile: the full stack's position is adjusted.
 2. The full stack is moving to a tile occupied by a friendly stack: the full stack is absorbed
    into it.
 3. Part of the stack is moving to an empty tile: a new stack is created at that tile.
 4. Part of the stack is moving to a tile occupied by a friendly stack: units are transferred to
    that stack.
 5. If the target tile contains an enemy stack or a structure owned by an enemy, the move is an
    attack.  Moving into the final position depends on the outcome of the attack.

The game arbiter verifies the validity of the move and modifies the path as appropriate.  If the
stack at the end of the path has changed, the last step is removed.  If any other step is
blocked, the path is truncated at that point.

If the full path is taken and the final position is occupied by an enemy, an attack is
initiated (see below).

View considerations
```````````````````

Updates to the game state are instantaneous.  Movement should be animated in the view, however.

Units aborbed into a stack are always appended.  The view updater only receives the message after
the units have been absorbed into the target stack, but before any other updates are applied.  It
can determine which units were moved as the last N units in the target stack, where N is the size
of the units parameter.

The view will create a ghost object and animate it along the path, masking the target stack where
appropriate.  Selecting the target stack will also be disabled.  The ghost will also update the
visible/discovered state of the view as it progresses.  When the ghost reaches its destination,
the target stacks will return to normal.

Attacking
---------

TODO
