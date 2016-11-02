#include "common.h"

#include "hex/ai/ai.h"
#include "hex/ai/ai_updater.h"
#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"


AiUpdater::AiUpdater(Ai *ai): ai(ai) {
    game_updater = new GameUpdater(&ai->game);
}

AiUpdater::~AiUpdater() {
    delete game_updater;
}

void AiUpdater::receive(boost::shared_ptr<Message> update) {
    game_updater->receive(update);
    apply_update(update);
}

void AiUpdater::apply_update(boost::shared_ptr<Message> update) {
    switch (update->type) {
        case CreateFaction: {
            auto upd = boost::dynamic_pointer_cast<CreateFactionMessage>(update);
            if (upd->data2 == ai->faction_type) {
                ai->faction = ai->game.factions.get(upd->data1);
            }
        } break;

        default:
            break;
    }
}
