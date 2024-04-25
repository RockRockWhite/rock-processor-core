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
    const int total_time = 10000;
    int test_time = 0;
    testbench_t<top_module_t> tb{argc, argv, "ALU.vcd", [](top_module_t *dut)
                                 { return nullptr; },
                                 0, clock_period};

    // test add
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 0; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a + dut->b)); },
            true);

        test_time += clock_period;
    }

    // test sll
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % 32;
            dut->alu_select = 1; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a << dut->b)); },
            true);

        test_time += clock_period;
    }

    // test slt
    tb.add_event(
        test_time, [](top_module_t *dut)
        { dut->a = -1; dut->b = 0; dut->alu_select = 2; },
        false);
    tb.add_event(
        test_time, [](top_module_t *dut)
        { assert(dut->alu_result == 1); },
        true);
    test_time += clock_period;
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 2; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (int32_t(dut->a) < int32_t(dut->b))); },
            true);

        test_time += clock_period;
    }

    // test xor
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 4; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a ^ dut->b)); },
            true);

        test_time += clock_period;
    }

    // test srl
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % 32;
            dut->alu_select = 5; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a >> dut->b)); },
            true);

        test_time += clock_period;
    }

    // test or
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 6; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a | dut->b)); },
            true);

        test_time += clock_period;
    }

    // test and
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 7; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a & dut->b)); },
            true);

        test_time += clock_period;
    }

    // test sub
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 12; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (dut->a - dut->b)); },
            true);

        test_time += clock_period;
    }

    // test sra
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % 32;
            dut->alu_select = 13; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == (int32_t(dut->a) >> int32_t(dut->b))); },
            true);

        test_time += clock_period;
    }

    // test bsel
    for (int i = 0; i != total_time; i++)
    {
        tb.add_event(
            test_time, [](top_module_t *dut)
            { dut->a = rand() % UINT_MAX;
            dut->b = rand() % UINT_MAX;
            dut->alu_select = 15; },
            false);

        tb.add_event(
            test_time, [](top_module_t *dut)
            { assert(dut->alu_result == dut->b); },
            true);

        test_time += clock_period;
    }

    // terminate
    tb.add_event(
        test_time, [&](top_module_t *dut)
        { tb.ebreak = true; },
        false);

    tb.sim_and_dump_wave();

    return 0;
}