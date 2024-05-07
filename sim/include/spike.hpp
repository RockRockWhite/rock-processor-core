#pragma once

#include <riscv.hpp>
#include <dlfcn.h>
#include <cassert>

enum direction_t
{
    TO_DUT,
    TO_REF
};

struct cpu_state_t
{
    word_t gpr[32];
    word_t pc;
};

const char so_path[] = "./tools/spike/build/libspike.so";
const int port = 5678;

void (*ref_difftest_memcpy)(word_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

void init_spike()
{
    void *handle = dlopen(so_path, RTLD_LAZY);
    assert(handle);

    void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
    assert(ref_difftest_init);

    ref_difftest_memcpy = (void (*)(word_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
    assert(ref_difftest_memcpy);

    ref_difftest_regcpy = (void (*)(void *, bool))dlsym(handle, "difftest_regcpy");
    assert(ref_difftest_regcpy);

    ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
    assert(ref_difftest_exec);

    ref_difftest_raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
    assert(ref_difftest_raise_intr);

    ref_difftest_init(port);
}