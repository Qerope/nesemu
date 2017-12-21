#include "cpu.h"

uint16_t Cpu::read16(uint16_t addr) {
    return bus->read(addr) | bus->read(addr + 1) << 8;
}

void Cpu::set_zn(uint8_t v) {
    p = (p & ~(Z | N)) | (v == 0 ? Z : 0) | (v & N);
}

void Cpu::reset() {
    pc = read16(0xFFFC);
    sp = 0xFD;
    p = 0x24;
}

int Cpu::step() {
    uint8_t op = bus->read(pc++);
}
