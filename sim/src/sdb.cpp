#include "sdb.hpp"

sdb_t::sdb_t(std::string trace_file, std::string img_file) : cpu(trace_file)
{
    this->cpu.memory.load_img(img_file, 0x80000000);
}

void sdb_t::main_loop()
{
    while (this->cpu.ctx->time() < 200)
    {
        this->cpu.tick_and_dump_wave();

        if (this->cpu.state == CPU_STOP)
        {
            std::cout << "HIT GOOD TRAP" << std::endl;
            break;
        }
    }
}