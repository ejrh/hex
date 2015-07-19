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
