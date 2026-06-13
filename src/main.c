#include <stdio.h>

#include "vip.h"

int main(int argc, char* argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        return 1;
    }

    char* filename = argc == 2 ? argv[1] : NULL;
    run_vip(filename);

    return 0;
}
