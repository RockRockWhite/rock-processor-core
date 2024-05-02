#pragma once

#include "riscv.hpp"
#include <deque>

namespace iringbuf
{
    struct instruction_buffer_t
    {
        word_t pc;
        word_t instruction;
    };

    void append(word_t pc, word_t instruction);
    std::deque<instruction_buffer_t> &get_buf();
}