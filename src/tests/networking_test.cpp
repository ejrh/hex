#include "common.h"

#include "hex/basics/error.h"
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
    PrintingMessageReceiver receiver;
    Server server(SERVER_PORT, &receiver);

    server.start();

    bool running = true;
    while (running) {
        SDL_Delay(60000);
        trace("running");
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
