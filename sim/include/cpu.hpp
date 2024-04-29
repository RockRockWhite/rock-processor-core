#pragma once

#include <string>
#include <functional>
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "VProcessorCore.h"
#include "VProcessorCore__Dpi.h"
#include "VProcessorCore___024root.h"
#include "fake_memory.h"

void ebreak();

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
    uint32_t *gpr[32];
    uint32_t *pc;

    cpu_t(std::string trace_file);
    ~cpu_t();

    void init_gpr();

    void tick_and_dump_wave();
    uint32_t get_gpr(uint32_t index);
    uint32_t get_pc();
};
