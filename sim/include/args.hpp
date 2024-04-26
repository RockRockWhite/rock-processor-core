#pragma once

#include <string>
#include <stdexcept>
#include <format>
#include <getopt.h>
#include <cassert>

class args_t
{
private:
    args_t() {}

public:
    std::string img_file = "";
    static args_t build(int argc, char **argv);
};
