#include <cassert>
#include <cstdio>

#include "cpu.h"

static Cpu make(Bus &bus, Cartridge &cart) {
    cart.prg.assign(16384, 0xEA);
    bus.cart = &cart;
    Cpu cpu(&bus);
    cpu.pc = 0x8000;
    return cpu;
}

static void run(Cartridge &cart, std::initializer_list<uint8_t> code) {
    int i = 0;
    for (uint8_t b : code)
        cart.prg[i++] = b;
}

int main() {
    Bus bus;
    Cartridge cart;
    Cpu cpu = make(bus, cart);

    // LDA #$50; ADC #$50 -> $A0, overflow set, carry clear
    run(cart, {0xA9, 0x50, 0x69, 0x50});
    cpu.step();
    cpu.step();
    assert(cpu.a == 0xA0);
    assert(cpu.p & V);
    assert(!(cpu.p & C));
    assert(cpu.p & N);

    // SEC; LDA #$50; SBC #$F0 -> $60, carry clear (borrow)
    cpu = make(bus, cart);
    run(cart, {0x38, 0xA9, 0x50, 0xE9, 0xF0});
    cpu.step();
    cpu.step();
    cpu.step();
    assert(cpu.a == 0x60);
    assert(!(cpu.p & C));

    // JMP ($10FF) reads the high byte from $1000, not $1100
    cpu = make(bus, cart);
    run(cart, {0x6C, 0xFF, 0x00});
    bus.ram[0x00FF] = 0x34;
    bus.ram[0x0000] = 0x12;
    cpu.step();
    assert(cpu.pc == 0x1234);

    printf("ok\n");
    return 0;
}
