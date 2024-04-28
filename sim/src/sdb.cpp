#include <iostream>
#include <sstream>
#include <format>
#include <functional>
#include <unordered_map>
#include "sdb.hpp"
#include "utils/disasm.h"

std::shared_ptr<cpu_t> sdb::cpu = nullptr;

extern void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
static int cmd_q(std::vector<std::string> &tokens)
{
    sdb::cpu->state = CPU_END;
    return 0;
}

static void exec_once()
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
}

static int cmd_si(std::vector<std::string> &tokens)
{
    // if no argument, default exec one instruction
    uint64_t exec_cnt = tokens.size() != 1 ? std::stoi(tokens[1]) : 1;

    // check if the exec_instruction_count is valid
    if (exec_cnt == 0)
    {
        std::cout << std::format("Invalid instruction count: {}", exec_cnt) << std::endl;
        return 0;
    }

    for (uint64_t i = 0; i != exec_cnt; i++)
    {
        exec_once();
    }

    return 0;
}

std::unordered_map<std::string, std::function<int(std::vector<std::string> &)>> cmd_table = {
    // Exec single instruction
    {"si", cmd_si},
    // Exit sdb,
    {"q", cmd_q},
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

        // check cpu state
        if (sdb::cpu->state == CPU_STOP)
        {
            std::cout << "HIT GOOD TRAP" << std::endl;
            break;
        }
        if (sdb::cpu->state == CPU_END)
        {
            std::cout << "quit" << std::endl;
            break;
        }
    }
}
