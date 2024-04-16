#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>

#include "testbench.h"
#include "VProcessorCore.h"

using top_module_t = VProcessorCore;

int main(int argc, char **argv)
{
    const int clock_period = 10;
    testbench_t<top_module_t> tb{argc, argv, "ProcessorCore.vcd", [](top_module_t *dut)
                                 { return &dut->clk; },
                                 500, clock_period};

    // test counter
    for (int i = 0; i != 32; i++)
    {
        tb.add_event(
            clock_period * i, [i](top_module_t *dut)
            { dut->instruction_test = 0x07B00093; },
            true);
    }

    tb.sim_and_dump_wave();

    return 0;
}