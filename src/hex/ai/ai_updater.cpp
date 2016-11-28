#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"

#include "hex/ai/ai.h"
#include "hex/ai/ai_updater.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"


AiUpdater::AiUpdater(Ai *ai): ai(ai) {
    game_updater = new GameUpdater(&ai->game);
}

AiUpdater::~AiUpdater() {
    delete game_updater;
}

void AiUpdater::receive(Message *update) {
    game_updater->receive(update);
    apply_update(update);
}

void AiUpdater::apply_update(Message *update) {
    switch (update->type) {
        case CreateFaction: {
            auto upd = dynamic_cast<CreateFactionMessage *>(update);
            if (upd->data2 == ai->faction_type) {
                ai->faction = ai->game.factions.get(upd->data1);
            }
        } break;

        default:
            break;
    }
}
