Command Notes
=============

Movement
--------

Movement is requested using the UnitMove message:

    UnitMove(stack_id, units, path, target_id)

This specifies the stack the units are coming from, the set of units that were selected to move,
the path the units will follow, and the stack (if any) in position at the end of the path.

The game arbiter verifies the validity of the move and modifies the path as appropriate.  If the
target at the end of the path has changed, the last step is removed.  If any other step is
blocked, the path is truncated at that point.

A sequence of `MoveUnits` is then issued, one for each step in the path.  This is followed by a
`TransferUnits` message, possibly preceded or succeeded by other messages.

For simple movement, there are four scenarios:

 1. Move: the whole stack moves to an empty tile.  The target is the same stack.
 2. Split: the stack is split, with some units becoming a new stack.  A `CreateStack` message is
    issued before the trasfer.  The target is the new stack.
 3. Merge: the whole stack is absorbed by another stack.  The target is the other stack.  A
    `DeleteStack` message is issued after the transfer.
 4. Transfer: some units are transferred to an existing stack.  The target is the other stack.

If the full path is taken and the final position is occupied or owned by an enemy, an attack is
initiated (see below).

### View considerations

Updates to the game state are instantaneous.  Movement should be animated in the view, however.
The sequence of `MoveUnits` messages facilitates this.

The first `MoveUnits` message results in a ghost being created, associated with that stack id.
The stack is marked as moving, which means it is not rendered (the ghost will represent it on the
screen).  At the same time, the stack id is locked in the throttle; this means that incoming game
messages are blocked for the time being.

The ghost animates along the move step.  When it reaches the target position, the stack id is
unlocked; his lets the next the `MoveUnits` message through to be processed, triggering the ghost
to start moving to the next position.

When the `MoveUnits` messages are all processed, the stack id no longer locked and the
`TransferUnits` message is received.  This causes the ghost to be retired and the unit stacks
involved to return to normal.

Game effects that occur on given step on the path are applied by the game updater as it processes
the corresponding `MoveUnits` messags.  This includes updating the discovered map for the units'
owner, and applying any terrain or unit properties.


Attacking
---------

TODO

Turn end
--------

A faction indicates it is ready for turn end with a message to the arbiter:

    FactionReady(faction_id, ready)

Factions indicate their readiness by setting the boolean ready flag and can change it as many
times as they like before turn end is announced.  The arbiter broadcasts FactionReady messages
as updates.

When the arbiter receives a FactionReady message that marks all factions as ready, a TurnEnd
update is sent.  During turn end, most commands are ignored.

    TurnEnd()

The arbiter computes and sends all turn end updates, such as recovered health
for units, and income from structures.  Finally a TurnBegin message is sent.

    TurnBegin(turn_number)
