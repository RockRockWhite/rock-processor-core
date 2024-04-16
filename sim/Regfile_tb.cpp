#include <cstdio>
#include <cassert>
#include <climits>

#include "testbench.h"
#include "VRegfile.h"

int main(int argc, char **argv)
{
    const int clock_period = 10;
    testbench_t<VRegfile> tb{argc, argv, "Regfile.vcd", [](VRegfile *dut)
                             { return &dut->clk; },
                             1000, clock_period};

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

    // test write to zero register
    tb.add_event(
        705, [](VRegfile *dut)
        {
            dut->rd = 0;
            dut->write_data = 114514;
            dut->write_enable = 1; },
        false);

    tb.add_event(
        715, [](VRegfile *dut)
        { dut->rs1 = 0; },
        false);

    tb.add_event(
        715, [](VRegfile *dut)
        { assert(dut->read_data1 == 0); },
        true);
    // tb.add_event(
    //     330 + clock_period * 33, [](VRegfile *dut)
    //     {
    //         assert(dut->read_data1 == 0); },
    //     false);

    tb.sim_and_dump_wave();

    return 0;
}