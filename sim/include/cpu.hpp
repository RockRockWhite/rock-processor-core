#pragma once

#include <string>
#include <functional>
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "VProcessorCore.h"
#include "VProcessorCore__Dpi.h"
#include "VProcessorCore___024root.h"
#include "fake_memory.h"

#include "riscv.hpp"

void ebreak();
int memory_read(int addr);
void memory_write(int addr, int data);

enum cpu_state_t
{
    CPU_RUNNING,
    CPU_BREAKPOINT,
    CPU_HLT,
    CPU_ABORT,
    CPU_QUIT
};

class cpu_t
{
protected:
    VerilatedVcdC *trace;

public:
    static std::function<void()> ebreak_handler_func;

    VerilatedContext *ctx;
    VProcessorCore *dut;
    memory_t memory;
    cpu_state_t state;
    word_t *gpr[32];
    word_t *pc;
    word_t *instruction;

    cpu_t(std::string trace_file);
    ~cpu_t();

    void init_gpr();

    void tick_and_dump_wave();
    word_t get_gpr(word_t index);
    word_t get_pc();
    word_t get_instruction();
};
