#ifndef AI_UPDATER_H
#define AI_UPDATER_H

#include "hexutil/messaging/receiver.h"

class Ai;
class GameUpdater;

class AiUpdater: public MessageReceiver {
public:
    AiUpdater(Ai *ai);
    virtual ~AiUpdater();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    void apply_update(boost::shared_ptr<Message> update);

private:
    Ai *ai;
    GameUpdater *game_updater;
};

#endif
