#ifndef GAME_WRITER_H
#define GAME_WRITER_H

namespace hex {

class Game;
class MessageReceiver;
class Message;

class GameWriter {
public:
    GameWriter(MessageReceiver *receiver): receiver(receiver) { }

    void write(Game *game);

private:
    void write_unit_types(Game *game);
    void write_tile_types(Game *game);
    void write_feature_types(Game *game);
    void write_structure_types(Game *game);
    void write_factions(Game *game);
    void write_levels(Game *game);
    void write_unit_stacks(Game *game);
    void write_structures(Game *game);
    void write_turn(Game *game);

    void emit(const boost::shared_ptr<Message>& message);

private:
    MessageReceiver *receiver;
};

};

#endif
