#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>

#include "testbench.h"
#include "VBranchComparator.h"

using top_module_t = VBranchComparator;

int main(int argc, char **argv)
{
    const int clock_period = 10;
    const int total_time = 300100;
    int test_time = 0;
    testbench_t<top_module_t> tb{argc, argv, "BranchComparator.vcd", [](top_module_t *dut)
                                 { return nullptr; },
                                 total_time, clock_period};

    for (int i = 0; i < 10000; i++)
    {
        // test equal
        tb.add_event(
            test_time, [](top_module_t *dut)
            { 
            dut->is_unsigned = 0;
            dut->data1 = rand() % INT_MAX;
            dut->data2 = dut->data1; },
            false);
        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->branch_equal == (dut->data1 == dut->data2)); },
            true);
        test_time += clock_period;
    }

    for (int i = 0; i < 10000; i++)
    {
        // test signed
        tb.add_event(
            test_time, [](top_module_t *dut)
            { 
            dut->is_unsigned = 0;
            dut->data1 = rand() % INT_MAX;
            dut->data2 = rand() % INT_MAX; },
            false);
        tb.add_event(
            test_time, [](top_module_t *dut)
            { 
            assert(dut->branch_equal == (dut->data1 == dut->data2));
            assert(dut->branch_less_than == (int(dut->data1) < int(dut->data2))); },
            true);
        test_time += clock_period;
    }

    for (int i = 0; i < 10000; i++)
    {
        // test unsigned
        tb.add_event(
            test_time, [](top_module_t *dut)
            { 
            dut->is_unsigned = 1;
            dut->data1 = rand() % UINT_MAX;
            dut->data2 = rand() % UINT_MAX; },
            false);
        tb.add_event(
            test_time, [](top_module_t *dut)
            { 
            assert(dut->branch_equal == (dut->data1 == dut->data2));
            assert(dut->branch_less_than == (unsigned(dut->data1) <unsigned(dut->data2))); },
            true);
        test_time += clock_period;
    }

    tb.sim_and_dump_wave();

    return 0;
}