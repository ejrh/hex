#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/chat/chat.h"


ChatUpdater::ChatUpdater(ChatWindow *chat_window): chat_window(chat_window) {
}

void ChatUpdater::receive(boost::shared_ptr<Message> update) {
    if (update->type == Chat) {
        boost::shared_ptr<WrapperMessage<std::string> > chat_msg = boost::dynamic_pointer_cast<WrapperMessage<std::string> >(update);
        chat_window->add_to_history(chat_msg->data);
    }
}
