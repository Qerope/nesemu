#pragma once
#include <cstdint>

#include "bus.h"

enum Flag : uint8_t { C = 1, Z = 2, I = 4, D = 8, B = 16, U = 32, V = 64, N = 128 };

struct Cpu {
    uint8_t a = 0, x = 0, y = 0, sp = 0xFD, p = 0x24;
    uint16_t pc = 0;
    Bus *bus;

    explicit Cpu(Bus *b) : bus(b) {}
    void reset();
    int step();

private:
    uint16_t read16(uint16_t addr);
    uint16_t operand_addr(int mode);
    void set_zn(uint8_t v);
    void push(uint8_t v);
    uint8_t pop();
    void adc(uint8_t m);
    void compare(uint8_t reg, uint8_t m);
};
