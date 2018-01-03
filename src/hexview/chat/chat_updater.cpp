#include "common.h"

#include "hexutil/messaging/message.h"

#include "hexgame/game/game_messages.h"

#include "hexview/chat/chat.h"


namespace hex {

ChatUpdater::ChatUpdater(ChatWindow *chat_window): chat_window(chat_window) {
}

void ChatUpdater::receive(Message *update) {
    if (update->type == Chat) {
        auto chat_msg = dynamic_cast<ChatMessage *>(update);
        chat_window->add_to_history(chat_msg->data);
    }
}

};
