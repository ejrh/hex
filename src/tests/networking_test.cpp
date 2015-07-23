#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/event_pusher.h"
#include "hex/messaging/receiver.h"
#include "hex/networking/networking.h"

#define SERVER_PORT 9999

class PrintingMessageReceiver: public MessageReceiver {
public:
    void receive(boost::shared_ptr<Message> msg) {
        std::cout << "Received: ";
        Serialiser s(std::cout);
        s << msg.get();
        std::cout << std::endl;
    }
};

void run() {
    EventPusher event_pusher;
    Server server(SERVER_PORT, &event_pusher);
    PrintingMessageReceiver printer;

    server.start();

    SDL_Delay(1000);

    Client client(&event_pusher);
    client.connect(std::string("localhost:9999"));

    bool running = true;
    while (running) {
        SDL_Event evt;

        if (SDL_WaitEventTimeout(&evt, 30000)) {
            if (evt.type == SDL_QUIT
                || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE))
                running = false;

            if (evt.type == event_pusher.event_type) {
                boost::shared_ptr<Message> msg = event_pusher.get_message(evt);
                printer.receive(msg);
            }
        }
    }

    server.stop();
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
