#include "difftest.hpp"
#include "spike.hpp"

namespace difftest
{

    bool inited = false;

    static void init()
    {
        if (inited)
        {
            return;
        }
        inited = true;
        init_spike();
    }

    word_t get_gpr(int index)
    {
        if (!inited)
        {
            init();
        }
        cpu_state_t state;
        ref_difftest_regcpy(&state, direction_t::TO_DUT);

        return state.gpr[index];
    }

    void set_gpr(int index, word_t value)
    {
        if (!inited)
        {
            init();
        }

        cpu_state_t state;
        ref_difftest_regcpy(&state, direction_t::TO_DUT);

        state.gpr[index] = value;
        ref_difftest_regcpy(&state, direction_t::TO_REF);
    }

    word_t get_pc()
    {
        if (!inited)
        {
            init();
        }

        cpu_state_t state;
        ref_difftest_regcpy(&state, direction_t::TO_DUT);

        return state.pc;
    }

    void set_pc(word_t value)
    {
        if (!inited)
        {
            init();
        }

        cpu_state_t state;
        ref_difftest_regcpy(&state, direction_t::TO_DUT);

        state.pc = value;
        ref_difftest_regcpy(&state, direction_t::TO_REF);
    }

    void memory_read(word_t addr, byte_t *buf, size_t n)
    {
        if (!inited)
        {
            init();
        }

        ref_difftest_memcpy(addr, buf, n, direction_t::TO_DUT);
    }

    void memory_write(word_t addr, byte_t *buf, size_t n)
    {
        if (!inited)
        {
            init();
        }

        ref_difftest_memcpy(addr, buf, n, direction_t::TO_REF);
    }

    void exec(uint64_t n)
    {
        if (!inited)
        {
            init();
        }

        ref_difftest_exec(n);
    }
}