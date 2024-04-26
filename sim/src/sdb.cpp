#include <iostream>
#include <sstream>
#include <format>
#include <functional>
#include <unordered_map>
#include "sdb.hpp"

std::shared_ptr<cpu_t> sdb::cpu = nullptr;

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

    for (int i = 0; i != exec_cnt; i++)
    {
        sdb::cpu->tick_and_dump_wave();
    }

    return 0;
}

std::unordered_map<std::string, std::function<int(std::vector<std::string> &)>> cmd_table = {
    // Exec single instruction
    {"si", cmd_si},
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
    }

}
