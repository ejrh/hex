#include "common.h"

#include "hex/messaging/checksum.h"
#include "hex/messaging/message.h"
#include "hex/messaging/serialiser.h"


void MessageChecksum::receive(boost::shared_ptr<Message> msg) {
    std::ostringstream stream;
    Serialiser writer(stream);
    writer << msg.get();
    std::string buffer(stream.str());

    boost::crc_32_type result(checksum);
    result.process_bytes(buffer.c_str(), buffer.size());
    checksum = result.checksum();
}
