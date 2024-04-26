#include <cpu.hpp>

#include <stdexcept>

#include <iostream>

// ebreak DPI handler
std::function<void()> cpu_t::ebreak_handler_func = []()
{ throw std::runtime_error("ebreak_handler_func must be assigned a function"); };
void ebreak()
{
    cpu_t::ebreak_handler_func();
}

cpu_t::cpu_t(std::string trace_file)
{
    this->ctx = new VerilatedContext;

    Verilated::traceEverOn(true);
    this->trace = new VerilatedVcdC;

    this->dut = new VProcessorCore{this->ctx};
    dut->clk = 0;

    this->dut->trace(this->trace, 5);
    this->trace->open(trace_file.c_str());

    this->state = CPU_RUNNING;

    // set ebreak handler
    cpu_t::ebreak_handler_func = [this]()
    {
        this->state = CPU_STOP;
    };
}

cpu_t::~cpu_t()
{
    trace->close();
    delete dut;
    delete ctx;
}

void cpu_t::tick_and_dump_wave()
{
    // update memory
    uint32_t inst;
    this->memory.read(this->dut->pc_test, (uint8_t *)&inst, 4);
    this->dut->instruction_test = inst;

    // tick
    dut->clk = (dut->clk == 0) ? 1 : 0;
    // eval
    dut->eval();
    // dump waveform
    trace->dump(ctx->time());
    // update time
    ctx->timeInc(1);

    // tick
    dut->clk = (dut->clk == 0) ? 1 : 0;
    // eval
    dut->eval();
    // dump waveform
    trace->dump(ctx->time());
    // update time
    ctx->timeInc(1);
}