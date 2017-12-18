#pragma once
#include <cstdint>

#include "cartridge.h"

struct Bus {
    uint8_t ram[0x800] = {};
    Cartridge *cart = nullptr;

    uint8_t read(uint16_t a) {
        if (a < 0x2000)
            return ram[a & 0x7FF];
        if (a >= 0x8000 && cart && !cart->prg.empty())
            return cart->prg[(a - 0x8000) % cart->prg.size()];
        return 0;
    }

    void write(uint16_t a, uint8_t v) {
        if (a < 0x2000)
            ram[a & 0x7FF] = v;
    }
};
