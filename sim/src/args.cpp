#include "args.hpp"

args_t args_t::build(int argc, char **argv)
{
    args_t args;

    static option args_options[] = {
        {"img", required_argument, 0, 'i'},
        {"batch", no_argument, 0, 'b'},
        {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "i:b", args_options, 0)) != -1)
    {
        switch (opt)
        {
        case 'i':
            args.img_file = optarg;
            break;
        case 'b':
            args.batch = true;
            break;
        default:
            throw std::runtime_error(std::format("Usage: {} --img <img_file> [-b|--batch]", argv[0]));
        }
    }

    return args;
}
