#ifndef GAME_ARBITER_H
#define GAME_ARBITER_H

class GameArbiter: public MessageReceiver {
public:
    GameArbiter(Game *game, Updater *publisher);
    virtual ~GameArbiter();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    Game *game;
    Updater *publisher;
};


#endif
