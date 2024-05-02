#include "iringbuf.hpp"

namespace iringbuf
{
    static const int BUFF_SIZE = 16;
    static std::deque<instruction_buffer_t> buf;

    void append(word_t pc, word_t instruction)
    {
        buf.push_back({pc, instruction});
        if (buf.size() > BUFF_SIZE)
        {
            buf.pop_front();
        }
    }

    std::deque<instruction_buffer_t> &get_buf()
    {
        return buf;
    }
}