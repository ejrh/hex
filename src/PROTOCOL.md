Protocol Notes
==============

This file describes the protocol used for server-cliet communications.  It assumes that a 2-way
Message stream has been set up.  A Message is one of a distinct set of defined types, and will
contain a payload of data values depending on its type.  By default, Messages are rendered as
text.

A server can have zero or more clients connected to it.  All communication is between a single
client and the server.  In general, clients send requests to the server, and the server broadcasts
updates to all clients.  Clients and the server should thus maintain the same state.  Where a
client sends a request that is incompatible with the server's state, the server will ignore or
modify the request as appropriate, and then send an update.  Clients do not assume their requests
are fulfilled and do not update their own state from them; they update based on the server's
update messages.

Initial connection
------------------

    Server                  Client

----

    StreamOpen(version)                 StreamOpen(version)

(Server and client now know what version the other is.)

                                        StreamReplay(game_id, msg_id)

(msg_id is the id of the last message received.  If server's game id matches and it has all
Messages since msg_id, and msg_id is not greater than the highest known Message id, then play
from that point:)

    StreamState(game_id, msg_id)
    update(...)                         request(...)

(Otherwise, send a full state:)

    full_state(...)
    StreamState(game_id, msg_id)
    update(...)                         request(...)

If a client is disconnected, it attempts to reconnect and performs the same initial connection
operation.  It will receive a backlog of missed update messages, or a full state.
