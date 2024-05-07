#pragma once

#include <string>
#include <stdexcept>
#include <format>
#include <getopt.h>
#include <cassert>

struct args_t
{
    std::string img_file = "";
    bool batch = false;
    static args_t build(int argc, char **argv);

private:
    args_t() {}
};
