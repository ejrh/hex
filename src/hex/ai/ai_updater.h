#ifndef AI_UPDATER_H
#define AI_UPDATER_H

#include "hex/messaging/receiver.h"

class Ai;

class AiUpdater: public MessageReceiver {
public:
    AiUpdater(Ai *ai);
    virtual ~AiUpdater();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    void apply_update(boost::shared_ptr<Message> update);

private:
    Ai *ai;
};

#endif
