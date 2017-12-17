#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s [--nestest] rom.nes\n", argv[0]);
        return 2;
    }
    bool nestest = false;
    const char *path = argv[argc - 1];
    for (int i = 1; i < argc - 1; i++)
        if (!strcmp(argv[i], "--nestest"))
            nestest = true;
    (void)nestest;
    return 0;
}
