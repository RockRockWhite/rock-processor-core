#include "cpu.hpp"
#include <string>

class sdb_t
{
protected:
    cpu_t cpu;

public:
    sdb_t(std::string trace_file, std::string img_file);

    void main_loop();
};