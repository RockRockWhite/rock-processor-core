#include <iostream>
#include <memory>
#include "riscv.hpp"

struct watchpoint_t
{
    int id;
    word_t last_value;
    std::string expression;
};
