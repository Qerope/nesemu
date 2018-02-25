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
    Cartridge cart;
    if (!cart.load(path))
        return 1;
    Bus bus;
    bus.cart = &cart;
    Cpu cpu(&bus);
    cpu.reset();
    if (nestest) {
        cpu.pc = 0xC000;
    }
    for (int i = 0; i < 9000; i++) {
        printf("%04X A:%02X X:%02X Y:%02X P:%02X SP:%02X", cpu.pc, cpu.a, cpu.x, cpu.y, cpu.p, cpu.sp);
        printf(" CYC:%llu", (unsigned long long)cpu.cycles);
        printf("\n");
        cpu.step();
    }
    (void)nestest;
    return 0;
}
