#include "difftest.hpp"
#include "spike.hpp"
#include <stdexcept>
#include <format>
#include <iostream>

namespace difftest
{

    bool inited = false;
    bool skip_ref = false;

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

    bool exec(uint64_t n)
    {
        if (!inited)
        {
            init();
        }

        // skip ref difftest
        if (skip_ref)
        {
            skip_ref = false;
            return true;
        }

        ref_difftest_exec(n);
        return false;
    }

    uint64_t load_img(std::string img_file, uint64_t address)
    {
        if (!inited)
        {
            init();
        }

        FILE *fd = fopen(img_file.c_str(), "rb");

        if (fd == nullptr)
        {
            throw std::runtime_error(std::format("Failed to open file: {}", img_file));
        }

        uint8_t byte;
        uint64_t offset = 0;
        while (fread(&byte, 1, 1, fd) == 1)
        {

            ref_difftest_memcpy(address + offset, (byte_t *)&byte, 1, direction_t::TO_REF);
            offset++;
        }

        fclose(fd);
        return offset;
    }

    void skip_ref_difftest()
    {
        skip_ref = true;
    }
}