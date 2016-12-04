#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/queue.h"
#include "hexutil/messaging/logger.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/builtin_messages.h"
#include "hexutil/networking/networking.h"


#define SERVER_PORT 9999

void run() {
    register_builtin_messages();

    MessageQueue dispatch_queue(1000);
    Server server(SERVER_PORT, &dispatch_queue);
    MessageLogger logger("Received: ");

    server.start();

    Client client(&dispatch_queue);
    client.connect(std::string("localhost:9999"));

    for (int i = 0; i < 5; i++) {
        boost::this_thread::sleep(boost::posix_time::seconds(1));

        dispatch_queue.flush(&logger);
    }

    server.stop();
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
