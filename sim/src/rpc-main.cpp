#include <iostream>
#include <string>

#include "args.hpp"
#include "sdb.hpp"

int main(int argc, char **argv)
{
    try
    {
        // build args
        auto args = args_t::build(argc, argv);

        sdb::init("./waveform/rpc.vcd", args.img_file);
        return sdb::main_loop(args.batch);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
}