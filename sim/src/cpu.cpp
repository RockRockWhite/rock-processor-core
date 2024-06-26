#include <cpu.hpp>
#include <stdexcept>
#include <iostream>
#include "difftest.hpp"
#include "utils/timer.hpp"

// ebreak DPI handler
std::function<void()> cpu_t::ebreak_handler_func = []()
{ throw std::runtime_error("ebreak_handler_func must be assigned a function"); };
void ebreak()
{
    cpu_t::ebreak_handler_func();
}

std::function<word_t(word_t)> memory_read_handler_func = [](word_t addr) -> word_t
{ throw std::runtime_error("memory_read_handler_func must be assigned a function"); };
std::function<void(word_t, word_t, word_t)> memory_write_handler_func = [](word_t addr, word_t data, word_t size)
{ throw std::runtime_error("memory_write_handler_func must be assigned a function"); };

int memory_read(int addr)
{
    return memory_read_handler_func(addr);
}

void memory_write(int addr, int data, int size)
{
    memory_write_handler_func(addr, data, size);
}

cpu_t::cpu_t(std::string trace_file)
{
    this->ctx = new VerilatedContext;

    Verilated::traceEverOn(true);
    this->trace = new VerilatedVcdC;

    this->dut = new VProcessorCore{this->ctx};
    this->dut->clk = 0;

    this->dut->trace(this->trace, 5);
    this->trace->open(trace_file.c_str());

    this->state = CPU_RUNNING;

    // set ebreak handler
    cpu_t::ebreak_handler_func = [this]()
    {
        this->state = CPU_HLT;
    };

    // set memory handler
    memory_read_handler_func = [this](word_t addr) -> word_t
    {
        word_t data;
        this->memory.read(addr, (uint8_t *)&data, 4);

        // handle rtc
        // in verilator, read memory happen just in the ent of the last cycle of the instruction
        // so we need to skip the next ref difftest if read mmio
        static uint64_t us = 0;
        if ((addr & ~0b11) == 0xa0000048)
        {
            us = timer::get_time();
            data = (uint32_t)us;
            difftest::skip_next_ref_difftest();
        }
        if ((addr & ~0b11) == 0xa000004c)
        {
            data = (uint32_t)(us >> 32);
            difftest::skip_next_ref_difftest();
        }

        // std::cout << std::format("memory read: addr=0x{:08x}, data=0x{:08x}\n", addr, data);
        return data;
    };

    memory_write_handler_func = [this](word_t addr, word_t data, word_t size)
    {
        // handle mmio
        // handle serial and skip difftest
        if ((addr & ~0b11) == 0xa00003f8)
        {
            char *char_ptr = (char *)&data;
            std::cerr << *char_ptr;
            difftest::skip_ref_difftest();
            return;
        }

        this->memory.write(addr, (uint8_t *)&data, size);
        // std::cout << std::format("memory write: addr=0x{:08x}, data=0x{:08x} size={}\n", addr, data, size);
    };

    // init gpr and pc
    this->init_gpr();
    this->pc = &this->dut->rootp->ProcessorCore__DOT__pc__DOT__pc_val;
    this->instruction = &this->dut->rootp->ProcessorCore__DOT__instruction;
}

cpu_t::~cpu_t()
{
    trace->close();
    delete dut;
    delete ctx;
}

void cpu_t::init_gpr()
{
#define INIT_GPR(idx) this->gpr[idx] = &this->dut->rootp->ProcessorCore__DOT__regfile__DOT____Vcellout__regfile_block__BRA__##idx##__KET____DOT__regfile__q
    // init gpr from 0 to 31
    INIT_GPR(0);
    INIT_GPR(1);
    INIT_GPR(2);
    INIT_GPR(3);
    INIT_GPR(4);
    INIT_GPR(5);
    INIT_GPR(6);
    INIT_GPR(7);
    INIT_GPR(8);
    INIT_GPR(9);
    INIT_GPR(10);
    INIT_GPR(11);
    INIT_GPR(12);
    INIT_GPR(13);
    INIT_GPR(14);
    INIT_GPR(15);
    INIT_GPR(16);
    INIT_GPR(17);
    INIT_GPR(18);
    INIT_GPR(19);
    INIT_GPR(20);
    INIT_GPR(21);
    INIT_GPR(22);
    INIT_GPR(23);
    INIT_GPR(24);
    INIT_GPR(25);
    INIT_GPR(26);
    INIT_GPR(27);
    INIT_GPR(28);
    INIT_GPR(29);
    INIT_GPR(30);
    INIT_GPR(31);
}

void cpu_t::tick_and_dump_wave()
{
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

word_t cpu_t::get_gpr(word_t index)
{
    return *(this->gpr[index]);
}

word_t cpu_t::get_pc()
{
    return *(this->pc);
}

word_t cpu_t::get_instruction()
{
    return *(this->instruction);
}
