#include <string>
#include <functional>
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "VProcessorCore.h"
#include "VProcessorCore__Dpi.h"

#include "fake_memory.h"

void ebreak();

enum cpu_state_t
{
    CPU_RUNNING,
    CPU_STOP,
    CPU_END,
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

    cpu_t(std::string trace_file);
    ~cpu_t();

    void tick_and_dump_wave();
};
