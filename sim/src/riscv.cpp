#include "riscv.hpp"

namespace riscv
{
    int32_t get_gpr_index(std::string name)
    {
        // abi name
        for (int i = 0; i < 32; i++)
        {
            if (gpr_abi_names[i] == name)
            {
                return i;
            }
        }

        return -1;
    }
}