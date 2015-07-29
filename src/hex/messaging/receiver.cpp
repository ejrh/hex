#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"

void replay_messages(const std::string& filename, MessageReceiver& receiver) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.fail())
        throw Error("Could not open file: %s", filename.c_str());

    Deserialiser reader(file);

    while (file.good()) {
        while (file.peek() == '#' || file.peek() == '\n' || file.peek() == '\r')
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (file.eof())
            break;
        Message *update_ptr;
        reader >> update_ptr;
        if (update_ptr == NULL)
            break;
        boost::shared_ptr<Message> update(update_ptr);
        receiver.receive(update);
    }
}
