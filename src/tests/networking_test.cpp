#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/event_pusher.h"
#include "hex/messaging/receiver.h"
#include "hex/networking/networking.h"


#define SERVER_PORT 9999

class PrintingMessageReceiver: public MessageReceiver {
public:
    void receive(boost::shared_ptr<Message> msg) {
        std::ostringstream ss;
        Serialiser writer(ss);
        writer << msg.get();
        std::string msg_str(ss.str());
        trace("Received: %s", msg_str.c_str());
    }
};

void run() {
    EventPusher event_pusher;
    Server server(SERVER_PORT, &event_pusher);
    PrintingMessageReceiver printer;

    server.start();

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS);
    SDL_Delay(1000);

    Client client(&event_pusher);
    client.connect(std::string("localhost:9999"));

    bool running = true;
    while (running) {
        SDL_Event evt;

        if (SDL_WaitEventTimeout(&evt, 1000)) {
            if (evt.type == SDL_QUIT)
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
