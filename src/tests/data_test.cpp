#include "common.h"

#include "hex/basics/error.h"
#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"


void run() {
    Resources resources;
    ResourceLoader loader(&resources, NULL);

    loader.load("data/resources.txt");
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
