#include "common.h"

#include "hexutil/messaging/checksum.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/serialiser.h"


void MessageChecksum::receive(Message *msg) {
    std::ostringstream stream;
    Serialiser writer(stream);
    writer << msg;
    std::string buffer(stream.str());

    boost::crc_32_type result(checksum);
    result.process_bytes(buffer.c_str(), buffer.size());
    checksum = result.checksum();
}
