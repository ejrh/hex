Serialisation
=============

Hex uses a custom text serialisation format.  This is used for:

  * The initial or current game state, including terrain info, parties, units, etc.
  * Network communcations of request and update messages.
  * Descriptions of view objects, such as how to render tiles or units.

A serialisation stream is a sequence of text characters, such as read from or written to a file
or received or sent over a network connection.  A `Serialiser` can be used to write `Message`
objects to the stream, and correspondingly a `Deserialiser` is used to read `Message` objects from
a stream.

Each message object is written in the following form:

    type(data, ...)

Where `type` is a label describing the message type, and `data` are zero or more pieces of data
separated by commas.  A fixed set of message types are defined in the game.  Each is accompanied
by a specification of what data types belong to it.

For example, the message type to create a structure on the map is defined like this in `src/hex/game/message_types.h`:

    MSG_TYPE(CreateStructure, WM3(Point, std::string, int))

An example message as serialised would look something like:

    CreateStructure((7,3), 'wizard_tower', 7)

Note that the data arguments are defined only by type and do not have names.  Their
interpretation is a matter for the code that receives a CreateStructure message.

It is an error for a stream to contain an unrecognised message type.  It is also an error for a
message input to contain serialised data that does not match the expected types.

White space is generally ignored, however distinct messages should be separated with a new line
or `;` character.  Additionally, comments can be appear in the stream beginning with a `#`
character and continuing to the end of the line.

When serialised messages are communicated over a network, an individual message cannot be broken
over multiple lines.  When reading from a text file, this restriction does not apply.  (This
inconsistency may be fixed in future.)  In resource description files it is convenient to break
long messages over multiple lines, e.g.

    PaintScript("pad1_ps", [
        SetLibrary("PAD1.ILB"),
        TerrainMatch("grass", PaintFrame(1)),
        TerrainMatch("desert", PaintFrame(2)),
        TerrainMatch("snow", PaintFrame(3)),
        TerrainMatch("steppe", PaintFrame(4)),
        TerrainMatch("wasteland", PaintFrame(5))
    ])
