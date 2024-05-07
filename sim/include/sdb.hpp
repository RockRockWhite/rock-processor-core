#pragma once

#include "riscv.hpp"
#include "cpu.hpp"
#include "watchpoint.hpp"
#include <vector>
#include <list>
#include <string>
#include <memory>

class sdb
{
public:
    static std::shared_ptr<cpu_t> cpu;
    static std::list<watchpoint_t> watchpoints;
    static void init(std::string trace_file, std::string img_file);
    static int main_loop(bool batch_mode);
    static word_t reg_str2val(std::string name, bool &ok);
};