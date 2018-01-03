#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"


namespace hex {

void replay_messages(const std::string& filename, MessageReceiver& receiver) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.fail())
        throw Error() << "Could not open file: " << filename;

    replay_messages(file, receiver, filename);
}

void replay_messages(std::istream& input, MessageReceiver& receiver, const std::string& input_name) {
    Deserialiser reader(input);

    try {
        while (input.good()) {
            reader.skip_whitespace();
            if (input.eof())
                break;
            Message *update_ptr;
            reader >> update_ptr;
            if (update_ptr == NULL)
                throw Error() << "Error attempting to deserialise message";
            reader.end_record();
            boost::shared_ptr<Message> update(update_ptr);
            receiver.receive(update);
        }
    } catch (Error err) {
        int line_no = reader.line();
        int col_no = reader.column();
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in %s:%d:%d: %s") % input_name % line_no % col_no % err.what();
    }
}

};
