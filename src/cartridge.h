#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Cartridge {
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;
    int mapper = 0;
    bool load(const std::string &path);
};
