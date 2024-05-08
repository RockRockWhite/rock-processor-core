#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>
#include <functional>

#include "testbench.h"
#include "VMemory.h"
#include "VMemory__Dpi.h"
#include "fake_memory.h"

using top_module_t = VMemory;

std::function<uint32_t(uint32_t)> memory_read_handler_func = [](uint32_t addr) -> uint32_t
{ throw std::runtime_error("memory_read_handler_func must be assigned a function"); };

std::function<void(uint32_t, uint32_t)> memory_write_handler_func = [](uint32_t addr, uint32_t data)
{ throw std::runtime_error("memory_write_handler_func must be assigned a function"); };

int memory_read(int addr)
{
    std::cout << "memory_read: " << addr << std::endl;
    return memory_read_handler_func(addr);
}

void memory_write(int addr, int data)
{
    std::cout << "memory_write: " << addr << " " << data << std::endl;
    memory_write_handler_func(addr, data);
}

int main(int argc, char **argv)
{
    const int clock_period = 10;
    testbench_t<top_module_t> tb{argc, argv, "Memory.vcd", [](top_module_t *dut)
                                 { return &dut->clk; },
                                 clock_period * 65};

    // init fake memory
    memory_t mem;

    // handler
    memory_read_handler_func = [&mem](uint32_t addr) -> uint32_t
    {
        uint32_t data;
        mem.read(addr, (uint8_t *)&data, 4);
        return data;
    };

    memory_write_handler_func = [&mem](uint32_t addr, uint32_t data)
    {
        mem.write(addr, (uint8_t *)&data, 4);
    };

    // test memory
    int time = 0;
    for (int i = 0; i != 32; i++)
    {
        tb.add_event(
            time, [i](top_module_t *dut)
            {
                dut->address = i * 4;
                dut->write_enable = 1;
                dut->data_write = i; },
            false);
        time += clock_period;
    }

    for (int i = 0; i != 32; i++)
    {
        tb.add_event(
            time, [i](top_module_t *dut)
            {
                dut->address = i * 4;
                dut->write_enable = 0; },
            false);
        tb.add_event(
            time, [i, &mem](top_module_t *dut)
            {
                uint32_t data;
                mem.read(i * 4, (uint8_t *)&data, 4);

                std::cout << "i:" << i << " data: " << data << " data_read" << dut->data_read << std::endl;
                 assert(dut->data_read == data); },
            true);
        time += clock_period;
    }

    tb.sim_and_dump_wave();

    return 0;
}