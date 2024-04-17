#include <unordered_map>
#include <string>
#include <format>
#include <stdexcept>
#include <cstdio>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>

using std::unordered_map, std::runtime_error, std::format;

class memory_t
{
protected:
    unordered_map<uint64_t, uint64_t> memory;

public:
    uint64_t load_file(const char *filename, uint64_t address)
    {
        FILE *fd = fopen(filename, "r");

        if (fd == nullptr)
        {
            throw runtime_error(std::format("Failed to open file: {}", filename));
        }

        uint32_t instruction;

        uint64_t offset = 0;
        while (fscanf(fd, "%x", &instruction) != EOF)
        {
            this->write(address + sizeof(instruction) * offset, (uint8_t *)&instruction, sizeof(instruction));
            offset++;
        }

        fclose(fd);
        return offset;
    }

    void write(uint64_t address, uint8_t *buf, uint64_t size)
    {
        for (uint64_t i = 0; i < size; i++)
        {
            memory[address + i] = buf[i];
        }
    }

    void read(uint64_t address, uint8_t *buf, uint64_t size)
    {
        for (uint64_t i = 0; i < size; i++)
        {
            if (memory.find(address + i) == memory.end())
            {
                buf[i] = 0;
            }

            buf[i] = memory[address + i];
        }
    }

    void dump();
};