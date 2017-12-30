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

void Cpu::adc(uint8_t m) {
    unsigned sum = a + m + (p & C);
    bool v = (a ^ sum) & 0x80;
    p = (p & ~(C | V)) | (sum > 0xFF ? C : 0) | (v ? V : 0);
    a = sum;
    set_zn(a);
}

void Cpu::compare(uint8_t reg, uint8_t m) {
    p = (p & ~C) | (reg >= m ? C : 0);
    set_zn(reg - m);
}

int Cpu::step() {
    uint8_t op = bus->read(pc++);
    int cc = op & 3, bbb = (op >> 2) & 7, aaa = op >> 5;
    case 0xAA: x = a; set_zn(x); return 0;
    case 0xA8: y = a; set_zn(y); return 0;
    case 0x8A: a = x; set_zn(a); return 0;
    case 0x98: a = y; set_zn(a); return 0;
    case 0xBA: x = sp; set_zn(x); return 0;
    case 0x9A: sp = x; return 0;

    if (cc == 1) {
        static const int modes[8] = {IZX, ZP, IMM, ABS, IZY, ZPX, ABY, ABX};
        int mode = modes[bbb];
        uint16_t addr = operand_addr(mode);
        uint8_t m = bus->read(addr);
        switch (aaa) {
        case 0: a |= m; set_zn(a); break;
        case 1: a &= m; set_zn(a); break;
        case 2: a ^= m; set_zn(a); break;
        case 3: adc(m); break;
        case 4: bus->write(addr, a); break;
        case 5: a = m; set_zn(a); break;
        case 6: compare(a, m); break;
        case 7: adc(~m); break;
        }
        return 0;
    }
    return 0;
}
