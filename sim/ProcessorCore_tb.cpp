#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>

#include "testbench.h"
#include "VProcessorCore.h"
#include "fake_memory.h"

using top_module_t = VProcessorCore;

int main(int argc, char **argv)
{
    const int clock_period = 10;
    testbench_t<top_module_t> tb{argc, argv, "ProcessorCore.vcd", [](top_module_t *dut)
                                 { return &dut->clk; },
                                 500, clock_period};

    // init fake memory
    memory_t mem;
    mem.load_file("../rom/test.rom", 0);

    // test counter
    for (int i = 0; i != 32; i++)
    {
        tb.add_event(
            clock_period * i, [i, &mem](top_module_t *dut)
            { 
                uint32_t inst;
                mem.read(dut->pc_test, (uint8_t *)&inst, 4);
                dut->instruction_test = inst; },
            true);
    }

    tb.sim_and_dump_wave();

    return 0;
}