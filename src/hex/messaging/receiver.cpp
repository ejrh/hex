#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"


void replay_messages(const std::string& filename, MessageReceiver& receiver) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.fail())
        throw Error() << "Could not open file: " << filename;

    Deserialiser reader(file);

    int line_no = 1;
    try {
        while (file.good()) {
            while (file.peek() == '#' || file.peek() == '\n' || file.peek() == '\r') {
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    line_no++;
            }
            if (file.eof())
                break;
            Message *update_ptr;
            reader >> update_ptr;
            if (update_ptr == NULL)
                throw Error() << "Error attempting to deserialise message";
            boost::shared_ptr<Message> update(update_ptr);
            receiver.receive(update);
            line_no++;
        }
    } catch (Error err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in %s:%d: %s") % filename % line_no % err.what();
    }
}
