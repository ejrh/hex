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
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
