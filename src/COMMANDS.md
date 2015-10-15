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

A TransferUnit message is then issued to the updater, possibly preceded or succeeded by other
messages.

For simple movement, there are four scenarios:

 1. Move: the whole stack moves to an empty tile.  The target is the same stack.
 2. Split: the stack is split, with some units becoming a new stack.  A CreateStack message is
    issued first.  The target is the new stack.
 3. Merge: the whole stack is absorbed by another stack.  The target is the other stack.  A
    DeleteStack message is issued afterwards.
 4. Transfer: some units are transferred to an existing stack.  The target is the other stack.

If the full path is taken and the final position is occupied or owned by an enemy, an attack is
initiated (see below).

### View considerations

Updates to the game state are instantaneous.  Movement should be animated in the view, however.

The TransferUnit message is first handled by the view's pre-updater.  When it is received here,
the change has not yet been applied to the game.  The view will use the stack's existing state to
create a ghost object.  The target stack is locked so that it cannot be selected or altered.

The ghost is animated over the map, updating the visible/discovered
state of the view as it progresses.  When the ghost reaches its destination, the target stack
is unlocked and its view updated.

When the main view updater receives a TransferUnit message, the change has already been applied
to the game state.  If there is an active ghost for the target then the target's view is not
updated.


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
