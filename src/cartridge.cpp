#include "cartridge.h"

#include <cstdio>

bool Cartridge::load(const std::string &path) {
    FILE *f = fopen(path.c_str(), "rb");
    if (!f) {
        perror(path.c_str());
        return false;
    }
    uint8_t h[16];
    if (fread(h, 1, 16, f) != 16 || h[0] != 'N' || h[1] != 'E' || h[2] != 'S' || h[3] != 0x1A) {
        fprintf(stderr, "%s: not an iNES file\n", path.c_str());
        fclose(f);
        return false;
    }
    mapper = (h[6] >> 4) | (h[7] & 0xF0);
    if (h[6] & 4)
        fseek(f, 512, SEEK_CUR);  // skip trainer
    prg.resize(h[4] * 16384);
    chr.resize(h[5] * 8192);
    if (fread(prg.data(), 1, prg.size(), f) != prg.size() ||
        fread(chr.data(), 1, chr.size(), f) != chr.size()) {
        fprintf(stderr, "%s: truncated\n", path.c_str());
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}
