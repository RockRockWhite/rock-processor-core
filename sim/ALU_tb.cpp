#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>

#include "testbench.h"
#include "VALU.h"

using top_module_t = VALU;

int main(int argc, char **argv)
{
    const int clock_period = 10;
    testbench_t<top_module_t> tb{argc, argv, "ALU.vcd", [](top_module_t *dut)
                                 { return nullptr; },
                                 1000, clock_period};

    // wire all the registers
    for (int i = 0; i != 32; i++)
    {
        tb.add_event(
            clock_period * i, [i](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX; },
            false);

        tb.add_event(
            clock_period * i, [i](top_module_t *dut)
            { assert(dut->alu_result == dut->a + dut->b); },
            true);
    }

    tb.sim_and_dump_wave();

    return 0;
}