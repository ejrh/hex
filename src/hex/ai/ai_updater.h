#ifndef AI_UPDATER_H
#define AI_UPDATER_H

#include "hexutil/messaging/receiver.h"

class Ai;
class GameUpdater;

class AiUpdater: public MessageReceiver {
public:
    AiUpdater(Ai *ai);
    virtual ~AiUpdater();

    virtual void receive(Message *update);

private:
    void apply_update(Message *update);

private:
    Ai *ai;
    GameUpdater *game_updater;
};

#endif
