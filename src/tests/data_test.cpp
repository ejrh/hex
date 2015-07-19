#include "common.h"

#include "hex/basics/error.h"
#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"

void run() {
    Resources resources;

    load_resources("data/tile_views.txt", resources, NULL);
    load_resources("data/unit_views.txt", resources, NULL);
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
