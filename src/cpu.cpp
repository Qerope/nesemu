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

void Cpu::push(uint8_t v) {
    bus->write(0x100 + sp--, v);
}

uint8_t Cpu::pop() {
    return bus->read(0x100 + ++sp);
}

void Cpu::adc(uint8_t m) {
    unsigned sum = a + m + (p & C);
    bool v = (~(a ^ m) & (a ^ sum) & 0x80) != 0;
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

    // branches: xxy10000
    if ((op & 0x1F) == 0x10) {
        static const uint8_t mask[4] = {N, V, C, Z};
        bool want = op & 0x20;
        int8_t off = (int8_t)bus->read(pc++);
        bool flag = (p & mask[op >> 6]) != 0;
        if (flag == want)
            pc += off;
        return 0;
    }

    switch (op) {
    case 0x4C: pc = read16(pc); return 0;  // JMP abs
    case 0x6C: {
        uint16_t ptr = read16(pc);
        // the 6502 does not carry into the high byte when the pointer ends in $FF
        pc = bus->read(ptr) | bus->read((ptr & 0xFF00) | ((ptr + 1) & 0xFF)) << 8;
        return 0;
    }
    case 0x20:
        push((pc + 1) >> 8);
        push((pc + 1) & 0xFF);
        pc = read16(pc);
        return 0;
    case 0x60:
        pc = pop();
        pc |= pop() << 8;
        pc++;
        return 0;
    case 0x48: push(a); return 0;
    case 0x68: a = pop(); set_zn(a); return 0;
    case 0x08: push(p); return 0;
    case 0x28: p = pop(); return 0;
    case 0xAA: x = a; set_zn(x); return 0;
    case 0xA8: y = a; set_zn(y); return 0;
    case 0x8A: a = x; set_zn(a); return 0;
    case 0x98: a = y; set_zn(a); return 0;
    case 0xBA: x = sp; set_zn(x); return 0;
    case 0x9A: sp = x; return 0;
    case 0xE8: x++; set_zn(x); return 0;
    case 0xCA: x--; set_zn(x); return 0;
    case 0xC8: y++; set_zn(y); return 0;
    case 0x88: y--; set_zn(y); return 0;
    case 0x18: p &= ~C; return 0;
    case 0x38: p |= C; return 0;
    case 0x58: p &= ~I; return 0;
    case 0x78: p |= I; return 0;
    case 0xB8: p &= ~V; return 0;
    case 0xD8: p &= ~D; return 0;
    case 0xF8: p |= D; return 0;
    case 0xEA: return 0;
    }

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
        case 7: adc(m ^ 0xFF); break;
        }
        return 0;
    }

    if (cc == 2) {
        static const int modes[8] = {IMM, ZP, ACC, ABS, 0, ZPX, 0, ABX};
        int mode = modes[bbb];
        // STX/LDX index with Y instead of X
        if ((aaa == 4 || aaa == 5) && mode == ZPX) mode = ZPY;
        if (aaa == 5 && mode == ABX) mode = ABY;
        if (mode == ACC) {
            uint8_t &r = a;
            uint8_t c = p & C;
            switch (aaa) {
            case 0: p = (p & ~C) | (r >> 7); r <<= 1; break;
            case 1: p = (p & ~C) | (r >> 7); r = (r << 1) | c; break;
            case 2: p = (p & ~C) | (r & 1); r >>= 1; break;
            case 3: p = (p & ~C) | (r & 1); r = (r >> 1) | (c << 7); break;
            }
            set_zn(r);
            return 0;
        }
        uint16_t addr = operand_addr(mode);
        uint8_t m = bus->read(addr);
        uint8_t c = p & C;
        switch (aaa) {
        case 0: p = (p & ~C) | (m >> 7); m <<= 1; break;
        case 1: p = (p & ~C) | (m >> 7); m = (m << 1) | c; break;
        case 2: p = (p & ~C) | (m & 1); m >>= 1; break;
        case 3: p = (p & ~C) | (m & 1); m = (m >> 1) | (c << 7); break;
        case 4: bus->write(addr, x); return 0;
        case 5: x = m; set_zn(x); return 0;
        case 6: m--; break;
        case 7: m++; break;
        }
        bus->write(addr, m);
        set_zn(m);
        return 0;
    }

    if (cc == 0) {
        static const int modes[8] = {IMM, ZP, 0, ABS, 0, ZPX, 0, ABX};
        uint16_t addr = operand_addr(modes[bbb]);
        uint8_t m = bus->read(addr);
        switch (aaa) {
        case 1: p = (p & ~(Z | N | V)) | ((a & m) == 0 ? Z : 0) | (m & (N | V)); break;
        case 4: bus->write(addr, y); break;
        case 5: y = m; set_zn(y); break;
        case 6: compare(y, m); break;
        case 7: compare(x, m); break;
        }
        return 0;
    }
    return 0;
}
