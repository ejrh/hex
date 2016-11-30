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

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS);
    SDL_Delay(1000);

    Client client(&dispatch_queue);
    client.connect(std::string("localhost:9999"));

    bool running = true;
    while (running) {
        SDL_Event evt;

        if (SDL_WaitEventTimeout(&evt, 1000)) {
            if (evt.type == SDL_QUIT)
                running = false;
        }

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
