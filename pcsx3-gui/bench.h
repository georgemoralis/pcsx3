#pragma once
#include <intrin.h>  

unsigned long long RDTSC_start_clk, RDTSC_end_clk;
double RDTSC_total_clk;

#define MEASURE(x) \
    RDTSC_start_clk = __rdtsc(); \
    {x}; \
    RDTSC_end_clk = __rdtsc(); \
    RDTSC_total_clk = (double)(RDTSC_end_clk-RDTSC_start_clk);