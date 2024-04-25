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
    uint64_t load_img(const char *img_file, uint64_t address)
    {
        FILE *fd = fopen(img_file, "rb");

        if (fd == nullptr)
        {
            throw runtime_error(std::format("Failed to open file: {}", img_file));
        }

        uint8_t byte;
        uint64_t offset = 0;
        while (fread(&byte, 1, 1, fd) == 1)
        {
            this->write(address + offset, (uint8_t *)&byte, 1);
            offset++;
        }

        printf("Loaded %s to memory at 0x%lx, size\n", img_file, address, offset);

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