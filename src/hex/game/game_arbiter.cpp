#include "common.h"

#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"

#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_arbiter.h"


GameArbiter::GameArbiter(Game *game, Updater *publisher): game(game), publisher(publisher) {
}

GameArbiter::~GameArbiter() {
}

void GameArbiter::receive(boost::shared_ptr<Message> command) {

    switch (command->type) {
        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> cmd = boost::dynamic_pointer_cast<UnitMoveMessage>(command);

            // TODO check that move is allowed, refuse if not

            emit(boost::make_shared<UnitMoveMessage>(cmd->unit, cmd->path));
        } break;

        case Chat: {
            boost::shared_ptr<WrapperMessage<std::string> > chat_msg = boost::dynamic_pointer_cast<WrapperMessage<std::string> >(command);
            emit(boost::make_shared<WrapperMessage<std::string> >(Chat, chat_msg->data));
        } break;

        default:
            break;
    }

    publisher->flush();
}

void GameArbiter::emit(boost::shared_ptr<Message> update) {
    publisher->receive(update);
}
