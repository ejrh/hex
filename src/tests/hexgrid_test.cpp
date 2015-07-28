#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"

void test_point(const char *name, Point point) {
    printf("%s: (%d,%d)\n", name, point.x, point.y);
    Point neighbours[6];
    printf("Get neighbours:\n");
    get_neighbours(point, neighbours);
    for (int i = 0; i < 6; i++) {
        printf("    %d -> (%d,%d)\n", i, neighbours[i].x, neighbours[i].y);
    }
    printf("Get direction:");
    for (int i = 0; i < 6; i++) {
        int dir = get_direction(point, neighbours[i]);
        printf("  %d", dir);
    }
    printf("\n");
    printf("Distance between:");
    for (int i = 0; i < 6; i++) {
        int dist = distance_between(point, neighbours[i]);
        printf("  %d", dist);
    }
    printf("\n");
    printf("Get circle:");
    for (int i = 0; i < 5; i++) {
        int num_scanlines = i*2 + 1;
        std::vector<int> scanlines(num_scanlines);
        get_circle(point, i, scanlines);
        printf("    %d ->", i);
        for (int j = 0; j < num_scanlines; j++)
            printf("  %d", scanlines[j]);
        printf("\n");
    }
}

void run() {
    Point even_point(6,6);
    test_point("Event point", even_point);

    Point odd_point(7,7);
    test_point("Odd point", odd_point);
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
