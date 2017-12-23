#include "cpu.h"

enum { IMM, ZP, ZPX, ZPY, ABS, ABX, ABY, IZX, IZY, ACC };

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

uint16_t Cpu::operand_addr(int mode) {
    switch (mode) {
    case IMM: return pc++;
    case ZP: return bus->read(pc++);
    case ZPX: return (bus->read(pc++) + x) & 0xFF;
    case ZPY: return (bus->read(pc++) + y) & 0xFF;
    case ABS: {
        uint16_t a = read16(pc);
        pc += 2;
        return a;
    }
    case ABX: {
        uint16_t a = read16(pc);
        pc += 2;
        return a + x;
    }
    case ABY: {
        uint16_t a = read16(pc);
        pc += 2;
        return a + y;
    }
    case IZX: {
        uint8_t z = bus->read(pc++) + x;
        return bus->read(z) | bus->read((uint8_t)(z + 1)) << 8;
    }
    case IZY: {
        uint8_t z = bus->read(pc++);
        uint16_t a = bus->read(z) | bus->read((uint8_t)(z + 1)) << 8;
        return a + y;
    }
    }
    return 0;
}

int Cpu::step() {
    uint8_t op = bus->read(pc++);
    int cc = op & 3, bbb = (op >> 2) & 7, aaa = op >> 5;
}
