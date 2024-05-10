#pragma once
#include <riscv.hpp>

namespace difftest
{
    word_t get_gpr(int index);
    void set_gpr(int index, word_t value);
    word_t get_pc();
    void set_pc(word_t value);

    void memory_read(word_t addr, byte_t *buf, size_t n);
    void memory_write(word_t addr, byte_t *buf, size_t n);

    // return true if the difftest is skipped
    bool exec(uint64_t n);

    uint64_t load_img(std::string img_file, uint64_t address);

    void skip_ref_difftest();
}