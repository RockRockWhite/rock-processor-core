#include "args.hpp"

args_t args_t::build(int argc, char **argv)
{
    args_t args;

    static option args_options[] = {
        {"img", required_argument, 0, 'i'},
        {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "i:", args_options, 0)) != -1)
    {
        switch (opt)
        {
        case 'i':
            args.img_file = optarg;
            break;
        default:
            throw std::runtime_error(std::format("Usage: {} --img <img_file>", argv[0]));
        }
    }

    return args;
}
