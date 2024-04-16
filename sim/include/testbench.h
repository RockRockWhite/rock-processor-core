#include "verilated.h"
#include "verilated_vcd_c.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>

using std::vector, std::function, std::unordered_map, std::reference_wrapper, std::optional, std::nullopt;

template <typename T>
class event_t
{
public:
    function<void(T *)> action;
    bool post_eval;
};

template <typename T>
class testbench_t
{
protected:
    VerilatedContext *ctx;
    T *dut;
    VerilatedVcdC *trace;
    unordered_map<int, vector<event_t<T>>> events;
    int clock_period = 10;

public:
    testbench_t(int argc, char **argv, const char *trace_file, int clock_period = 10)
    {
        this->clock_period = clock_period;

        this->ctx = new VerilatedContext;
        this->ctx->commandArgs(argc, argv);

        Verilated::traceEverOn(true);
        this->trace = new VerilatedVcdC;

        this->dut = new T{this->ctx};
        this->dut->trace(this->trace, 5);

        this->trace->open(trace_file);
    }

    ~testbench_t()
    {
        trace->close();
        delete dut;
        delete ctx;
    }

    void add_event(int time, function<void(T *)> action, bool post_eval)
    {
        // 判断key exists
        if (events.find(time) == events.end())
        {
            events[time] = vector<event_t<T>>();
        }
        events[time].push_back({action, post_eval});
    }

    void sim_and_dump_wave(int total_time)
    {
        // initialize clock
        dut->clk = 0;

        // run simulation
        for (int time = 0; time != total_time; time++)
        {
            if (time % (this->clock_period / 2) == 0)
            {
                // update clock
                dut->clk = dut->clk == 0 ? 1 : 0;
            }

            vector<event_t<T>> EMPTY;
            auto current_time_events = [&]() -> optional<reference_wrapper<vector<event_t<T>>>>
            {
                if (this->events.find(time) == this->events.end())
                {
                    return nullopt;
                }
                return this->events[time];
            }();

            // pre-eval events
            if (current_time_events.has_value())
            {
                for (auto &event : current_time_events.value().get())
                {
                    if (!event.post_eval)
                    {
                        event.action(dut);
                    }
                }
            }

            dut->eval();

            // post-eval events
            if (current_time_events.has_value())
            {
                for (auto &event : current_time_events.value().get())
                {
                    if (event.post_eval)
                    {
                        event.action(dut);
                    }
                }
            }

            // dump waveform
            trace->dump(ctx->time());

            // update time
            ctx->timeInc(1);
        }
    }
};