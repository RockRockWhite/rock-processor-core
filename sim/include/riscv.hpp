#pragma once

#include <cstdint>
#include <string>

#define word_t uint32_t

namespace riscv
{
    int32_t get_gpr_index(std::string name);
    std::string get_gpr_abi_name_by_index(int index);
}
