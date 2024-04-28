#pragma once

#include "riscv.hpp"
#include "cpu.hpp"
#include <vector>
#include <string>
#include <memory>

class sdb
{
public:
    static std::shared_ptr<cpu_t> cpu;
    static void init(std::string trace_file, std::string img_file);
    static void main_loop();
    static word_t reg_str2val(std::string name, bool &ok);
};