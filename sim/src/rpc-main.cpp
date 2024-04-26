#include <iostream>
#include <string>
#include <getopt.h>
#include <cstdio>
#include <cassert>
#include <climits>
#include <cmath>
#include <functional>

#include "args.hpp"
#include "sdb.hpp"

int main(int argc, char **argv)
{
    try
    {
        // build args
        auto args = args_t::build(argc, argv);

        sdb_t sdb("./waveform/rpc.vcd", args.img_file);
        sdb.main_loop();

        // const int clock_period = 10;
        // testbench_t<top_module_t> tb{0, nullptr, "./waveform/rpc.vcd", [](top_module_t *dut)
        //                              { return &dut->clk; },
        //                              200};

        // // ebreak handler
        // ebreak_handler_func = [&tb]()
        // {
        //     tb.ebreak = true;

        //     std::cout << "HIT GOOD TRAP" << std::endl;
        // };

        // // init fake memory
        // memory_t mem;
        // mem.load_img(args.img_file, 0x80000000);

        // // init pc

        // // test counter
        // for (int i = 0; i != 32; i++)
        // {
        //     tb.add_event(
        //         clock_period * i, [i, &mem](top_module_t *dut)
        //         {
        //         uint32_t inst;
        //         mem.read(dut->pc_test, (uint8_t *)&inst, 4);
        //         dut->instruction_test = inst; },
        //         true);
        // }

        // tb.sim_and_dump_wave();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}