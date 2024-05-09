#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>

#include "testbench.h"
#include "VDataSplitter.h"

using top_module_t = VDataSplitter;

int main(int argc, char **argv)
{
    const int clock_period = 10;
    const int total_time = 10000;
    int test_time = 0;
    testbench_t<top_module_t> tb{argc, argv, "DataSplitter.vcd", [](top_module_t *dut)
                                 { return nullptr; },
                                 total_time, clock_period};

    // test passthrough
    tb.add_event(
        test_time, [](top_module_t *dut)
        { dut->option = 0; dut->data_in = rand() % UINT_MAX; },
        false);
    tb.add_event(
        test_time, [](top_module_t *dut)
        { assert(dut->data_out == dut->data_in); },
        true);
    test_time += clock_period;

    // test half word with sign extension
    tb.add_event(
        test_time, [](top_module_t *dut)
        { dut->option = 1; dut->data_in = 0x01ABCDEF; },
        false);
    tb.add_event(
        test_time, [](top_module_t *dut)
        { assert(dut->data_out == 0xFFFFCDEF); },
        true);
    test_time += clock_period;

    // test half word without sign extension
    tb.add_event(
        test_time, [](top_module_t *dut)
        { dut->option = 2; dut->data_in = 0x01ABCDEF; },
        false);
    tb.add_event(
        test_time, [](top_module_t *dut)
        { assert(dut->data_out == 0x0000CDEF); },
        true);
    test_time += clock_period;

    // test one low byte with sign extension
    tb.add_event(
        test_time, [](top_module_t *dut)
        { dut->option = 3; dut->data_in = 0x01ABCDEF; },
        false);
    tb.add_event(
        test_time, [](top_module_t *dut)
        { assert(dut->data_out == 0xFFFFFFEF); },
        true);
    test_time += clock_period;

    // test one low byte without sign extension
    tb.add_event(
        test_time, [](top_module_t *dut)
        { dut->option = 4; dut->data_in = 0x01ABCDEF; },
        false);
    tb.add_event(
        test_time, [](top_module_t *dut)
        { assert(dut->data_out == 0x000000EF); },
        true);

    tb.sim_and_dump_wave();

    return 0;
}