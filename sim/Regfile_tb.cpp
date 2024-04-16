#include <cstdio>
#include <cassert>
#include <climits>

#include "testbench.h"
#include "VRegfile.h"

int main(int argc, char **argv)
{
    const int clock_period = 10;
    testbench_t<VRegfile> tb{argc, argv, "Regfile.vcd", clock_period};

    // wire all the registers
    for (int i = 0; i != 32; i++)
    {
        // disable write
        tb.add_event(
            clock_period * i + 3, [i](VRegfile *dut)
            { dut->write_enable = 0; },
            false);

        tb.add_event(
            clock_period * i + 5, [i](VRegfile *dut)
            { 
                dut->rd = i;
                dut->write_data = i; 
                dut->write_enable = 1; },
            false);
    }

    // disable write
    tb.add_event(
        clock_period * 32 + 3, [](VRegfile *dut)
        { dut->write_enable = 0; },
        false);

    // read all the registers
    for (int i = 0; i != 32; i++)
    {
        tb.add_event(
            330 + clock_period * i, [i](VRegfile *dut)
            { 
                dut->rs1 = i;
                dut->rs2 = 31 - i; },
            false);

        // verify the read data
        tb.add_event(
            330 + clock_period * i, [i](VRegfile *dut)
            {
                assert(dut->read_data1 == i);
                assert(dut->read_data2 == 31 - i ); },
            true);
    }

    tb.sim_and_dump_wave(1000);

    return 0;
}