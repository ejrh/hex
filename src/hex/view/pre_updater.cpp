#include "common.h"

#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/combat/combat.h"
#include "hex/messaging/message.h"
#include "hex/view/pre_updater.h"
#include "hex/view/view.h"
#include "hex/view/combat/battle_viewer.h"


PreUpdater::PreUpdater(Game *game, GameView *game_view): game(game), game_view(game_view), battle_viewer(NULL) {
}

PreUpdater::~PreUpdater() {
}

void PreUpdater::receive(boost::shared_ptr<Message> update) {
    try {
        apply_update(update);
    } catch (const DataError& err) {
        BOOST_LOG_TRIVIAL(error) << "Invalid update received; " << err.what();
    }
}

void PreUpdater::apply_update(boost::shared_ptr<Message> update) {
    switch (update->type) {
        case TransferUnits: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);
            game_view->transfer_units(upd->data1, upd->data2, upd->data3, upd->data4);
        } break;

        case DoBattle: {
            boost::shared_ptr<DoBattleMessage> upd = boost::dynamic_pointer_cast<DoBattleMessage>(update);
            int attacker_id = upd->data1;
            Point attacking_point = game->stacks.get(attacker_id)->position;
            Point attacked_point = upd->data2;
            std::vector<Move>& moves = upd->data3;
            Battle battle(game, attacked_point, attacking_point, moves);
            if (battle_viewer)
                battle_viewer->show_battle(&battle);
        } break;

        default:
            break;
    }
}
