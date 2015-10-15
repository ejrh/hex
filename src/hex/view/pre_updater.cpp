#include "common.h"

#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"
#include "hex/view/pre_updater.h"
#include "hex/view/view.h"


PreUpdater::PreUpdater(Game *game, GameView *game_view): game(game), game_view(game_view) {
}

PreUpdater::~PreUpdater() {
}

void PreUpdater::receive(boost::shared_ptr<Message> update) {
    apply_update(update);
}

void PreUpdater::apply_update(boost::shared_ptr<Message> update) {
    switch (update->type) {
        case TransferUnits: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);
            game_view->transfer_units(upd->data1, upd->data2, upd->data3, upd->data4);
        } break;

        default:
            break;
    }
}
