#include "common.h"

#include "hexutil/messaging/message.h"

#include "hex/chat/chat.h"
#include "hex/game/game_messages.h"


ChatUpdater::ChatUpdater(ChatWindow *chat_window): chat_window(chat_window) {
}

void ChatUpdater::receive(boost::shared_ptr<Message> update) {
    if (update->type == Chat) {
        auto chat_msg = boost::dynamic_pointer_cast<ChatMessage>(update);
        chat_window->add_to_history(chat_msg->data);
    }
}
