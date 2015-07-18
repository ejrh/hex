#include "common.h"

#include "hex/basics/error.h"
#include "hex/networking/networking.h"

#define SERVER_PORT 9999

void run() {
    trace("begin");
    boost::asio::io_service io_service;

    Server server(SERVER_PORT, io_service);
    trace("run");
    io_service.run();

    //Client client1(io_service);
    //client1.connect("localhost", SERVER_PORT);

    //Client client2(io_service);
    //client2.connect("localhost", SERVER_PORT);

    //bool running = true;
    //while (running) {
    //    SDL_Delay(1000);
    //    trace("running");
    //}

    trace("end");
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
