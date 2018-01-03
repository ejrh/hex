#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/logger.h"
#include "hexutil/messaging/receiver.h"


namespace hex {

void MessageLogger::receive(Message *update) {
    std::ostringstream buf;
    Serialiser writer(buf);
    writer << update;
    std::string s = buf.str();
    if (s[s.size() - 1] == '\n')
        s.resize(s.size() - 1);
    BOOST_LOG_TRIVIAL(trace) << prefix << s;
}

};
