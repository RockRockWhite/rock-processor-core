#include <cstdio>
#include <cassert>
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "regtest_constr.h"
#include "Vregtest.h"
#include "nvboard.h"

int main(int argc, char **argv)
{

    VerilatedContext *ctx = new VerilatedContext;
    ctx->commandArgs(argc, argv);

    Verilated::traceEverOn(true);
    VerilatedVcdC *trace = new VerilatedVcdC;

    Vregtest *dut = new Vregtest{ctx};
    dut->trace(trace, 5);

    trace->open("Vregtest.vcd");

    // init nvboard
    nvboard_bind_all_pins(dut);
    nvboard_init();

    while (!ctx->gotFinish())
    {
        ctx->timeInc(1);
        nvboard_update();

        dut->eval();
        trace->dump(ctx->time());
    }

    nvboard_quit();

    trace->close();
    delete dut;
    delete ctx;
    return 0;
}