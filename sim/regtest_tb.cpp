#include <cstdio>
#include <cassert>
#include <climits>

#include "testbench.h"
#include "Vregtest.h"

int main(int argc, char **argv)
{
    testbench_t<Vregtest> sim{argc, argv, "regtest.vcd"};

    sim.add_event(
        101, [](Vregtest *dut)
        { dut->d = 123; },
        false);

    sim.add_event(
        110, [](Vregtest *dut)
        { 
            assert(dut->q == 123);
            dut->d = 456; },
        true);
    sim.sim_and_dump_wave(1000);

    return 0;
}