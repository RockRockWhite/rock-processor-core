#include <iostream>
#include <sstream>
#include <format>
#include <functional>
#include <unordered_map>
#include "sdb.hpp"
#include "riscv.hpp"
#include "expr.hpp"
#include "utils/disasm.h"

std::shared_ptr<cpu_t> sdb::cpu = nullptr;

extern void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
static int cmd_q(std::vector<std::string> &tokens)
{
    sdb::cpu->state = CPU_END;
    return 0;
}

static cpu_state_t exec_once()
{
    // update instruction
    // fake i memory
    uint32_t pc = sdb::cpu->dut->pc_test;
    uint32_t inst;
    sdb::cpu->memory.read(pc, (uint8_t *)&inst, 4);
    sdb::cpu->dut->instruction_test = inst;

    // disasm
    char inst_disasm[64];

    utils::disassemble(inst_disasm,
                       64, pc, (uint8_t *)&inst, 4);

    std::cout << std::format("0x{:08x}: 0x{:08x}  {}", pc, inst, inst_disasm) << std::endl;

    sdb::cpu->tick_and_dump_wave();

    return sdb::cpu->state;
}

static cpu_state_t exec_n(int64_t n)
{
    int64_t cnt = 0;

    while (true)
    {
        if (n != -1 && cnt == n)
        {
            break;
        }

        if (exec_once() != CPU_RUNNING)
        {
            // if the cpu is not running, break
            break;
        }
        cnt++;
    }

    return sdb::cpu->state;
}

static int cmd_si(std::vector<std::string> &tokens)
{
    // if no argument, default exec one instruction
    uint64_t exec_cnt = tokens.size() != 1 ? std::stoi(tokens[1]) : 1;

    // check if the exec_instruction_count is valid
    if (exec_cnt == 0)
    {
        std::cout << std::format("Invalid instruction count: {}", exec_cnt) << std::endl;
        return -1;
    }

    exec_n(exec_cnt);

    return 0;
}

static int cmd_c(std::vector<std::string> &tokens)
{
    // run until the end
    exec_n(-1);

    return 0;
}

static void reg_display()
{
    // print all the gpr
    for (int i = 0; i < 32; i++)
    {
        std::cout << std::format("{:<3}\t0x{:08x}\t{}", riscv::gpr_abi_names[i], sdb::cpu->get_gpr(i), sdb::cpu->get_gpr(i)) << std::endl;
    }

    // print pc
    std::cout << std::format("{:<3}\t0x{:08x}\t{}", "pc", sdb::cpu->get_pc(), sdb::cpu->get_pc()) << std::endl;
}

static int cmd_info(std::vector<std::string> &tokens)
{
    if (tokens.size() == 2 && tokens[1] == "r")
    {
        // print all registers
        reg_display();
        return 0;
    }

    if (tokens.size() == 2 && tokens[1] == "w")
    {
        return 0;
    }

    std::cout << "Usage: info [r|w]" << std::endl;
    return -1;
}

static int cmd_x(std::vector<std::string> &tokens)
{
    if (tokens.size() == 3)
    {
        int cnt = std::stoi(tokens[1]);
        std::string addr_expr = tokens[2];

        word_t addr;
        try
        {
            addr = expr::expr(addr_expr);
        }
        catch (const std::exception &e)
        {
            std::cerr << std::format("parsing expr error: P{}", e.what()) << '\n';
            return -1;
        }

        // exem the memory
        std::cout << std::format("0x{:08x}:\t", addr);
        for (int i = 0; i < cnt; i++)
        {
            word_t curr_exam_addr = addr + i * 4;
            word_t value;
            sdb::cpu->memory.read(curr_exam_addr, (uint8_t *)&value, 4);
            std::cout << std::format("0x{:08x} ", value);
        }

        std::cout << std::endl;
        return 0;
    }

    std::cout << "Usage: x N EXPR"
              << std::endl;

    return -1;
}

static int cmd_p(std::vector<std::string> &tokens)
{
    if (tokens.size() == 2)
    {
        word_t expr_val;
        try
        {
            expr_val = expr::expr(tokens[1]);
        }
        catch (const std::exception &e)
        {
            std::cerr << std::format("parsing expr error: P{}", e.what()) << '\n';
            return -1;
        }

        std::cout << std::format("0x{:08x} {}", expr_val, expr_val) << std::endl;
        return 0;
    }

    std::cout << "Usage: p EXPR"
              << std::endl;

    return -1;
}

std::unordered_map<std::string, std::function<int(std::vector<std::string> &)>> cmd_table = {
    // Exec single instruction
    {"si", cmd_si},
    // Exec until the end
    {"c", cmd_c},
    // Exit sdb,
    {"q", cmd_q},
    // Print some information
    {"info", cmd_info},
    // Exam memory
    {"x", cmd_x},
    // Print the value of an expression
    {"p", cmd_p},
};

static std::vector<std::string> read_command()
{
    std::vector<std::string> tokens;
    std::cout << "(rpc) ";

    std::string command;
    std::getline(std::cin, command);

    std::stringstream ss(command);

    std::string current_token;
    while (ss >> current_token)
    {
        tokens.push_back(current_token);
    }
    return tokens;
}

void sdb::init(std::string trace_file, std::string img_file)
{
    // init disasm
    utils::init_disasm("riscv32-pc-linux-gnu");
    sdb::cpu = std::make_shared<cpu_t>(trace_file);
    sdb::cpu->memory.load_img(img_file, 0x80000000);
    expr::init_regex();
}

void sdb::main_loop()
{
    while (true)
    {
        // read one command
        auto tokens = read_command();

        auto cmd = cmd_table.find(tokens[0]);
        if (tokens.size() == 0 || cmd == cmd_table.end())
        {
            std::cout << "Invalid command" << std::endl;
            continue;
        }

        // route command
        cmd->second(tokens);

        switch (sdb::cpu->state)
        {
        case CPU_RUNNING:
            continue;
        case CPU_STOP:
            std::cout << "HIT GOOD TRAP" << std::endl;
            return;
        case CPU_END:
            std::cout << "quit" << std::endl;
            return;
        default:
            assert(0);
        }
    }
}

word_t sdb::reg_str2val(std::string name, bool &ok)
{
    // pc register
    if (name == "pc")
    {
        ok = true;
        return sdb::cpu->get_pc();
    }

    // abi name
    for (int i = 0; i < 32; i++)
    {
        int32_t idx = riscv::get_gpr_index(name);

        if (idx != -1)
        {
            ok = true;
            return sdb::cpu->get_gpr(idx);
        }
    }

    // just x with a number
    // like x0, x1
    for (int i = 0; i < 32; i++)
    {
        if (name == std::format("x{}", i))
        {
            ok = true;
            return sdb::cpu->get_gpr(i);
        }
    }

    // wrong register name
    ok = false;
    return 0;
}
