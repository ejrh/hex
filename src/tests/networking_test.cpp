#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/queue.h"
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
        BOOST_LOG_TRIVIAL(info) << "Received: " << msg_str;
    }
};

void run() {
    MessageQueue dispatch_queue(1000);
    Server server(SERVER_PORT, &dispatch_queue);
    PrintingMessageReceiver printer;

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

            dispatch_queue.flush(&printer);
        }
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
