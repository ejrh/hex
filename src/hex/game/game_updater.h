#ifndef GAME_UPDATER_H
#define GAME_UPDATER_H

class GameUpdater: public MessageReceiver {
public:
    GameUpdater(Game *game);
    virtual ~GameUpdater();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    void apply_update(boost::shared_ptr<Message> update);

private:
    Game *game;
};


#endif
