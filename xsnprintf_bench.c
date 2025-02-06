/*
 * fast integer to string using a combination of tactics.
 * divide and conquer, base 100 table plus Duff's device.
 *
 * Copyright (c) 2025 Michael Clark
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "xsnprintf.h"

typedef unsigned int uint;
typedef unsigned long long ullong;

static void* rnd_setup_u32(size_t count)
{
    uint *arr = malloc(count * sizeof(uint));
    srand(0);
    for (size_t i = 0; i < count; i++) {
        arr[i] = (uint)rand();
    }
    return arr;
}

static void* rnd_setup_u64(size_t count)
{
    ullong *arr = malloc(count * sizeof(ullong));
    srand(0);
    for (size_t i = 0; i < count; i++) {
        arr[i] = (ullong)rand() << 32 | (ullong)rand();
    }
    return arr;
}

static void rnd_teardown(void *ctx)
{
    free(ctx);
}

static size_t bench_utoa_sml_u32(void *ctx, size_t count)
{
    char buf[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += utoa_u32((uint)i, buf);
    }
    return total;
}

static size_t bench_utoa_sml_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += utoa_u64((ullong)i, buf);
    }
    return total;
}

static size_t bench_utoa_big_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += utoa_u64((ullong)i << 31, buf);
    }
    return total;
}

static size_t bench_utoa_rnd_u32(void *ctx, size_t count)
{
    char buf[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += utoa_u32(((uint*)ctx)[i], buf);
    }
    return total;
}

static size_t bench_utoa_rnd_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += utoa_u64(((ullong*)ctx)[i], buf);
    }
    return total;
}

static size_t bench_xsnprintf_sml_u32(void *ctx, size_t count)
{
    char buf[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += xsnprintf(buf, sizeof(buf), "%u", (uint)i);
    }
    return total;
}

static size_t bench_xsnprintf_sml_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += xsnprintf(buf, sizeof(buf), "%llu", (ullong)i);
    }
    return total;
}

static size_t bench_xsnprintf_big_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += xsnprintf(buf, sizeof(buf), "%llu", (ullong)i << 31);
    }
    return total;
}

static size_t bench_xsnprintf_rnd_u32(void *ctx, size_t count)
{
    char buf[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%u", ((uint*)ctx)[i]);
    }
    return total;
}

static size_t bench_xsnprintf_rnd_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%llu", ((ullong*)ctx)[i]);
    }
    return total;
}

static size_t bench_snprintf_sml_u32(void *ctx, size_t count)
{
    char buf[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%u", (uint)i);
    }
    return total;
}

static size_t bench_snprintf_sml_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%llu", (ullong)i);
    }
    return total;
}

static size_t bench_snprintf_big_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%llu", (ullong)i << 31);
    }
    return total;
}

static size_t bench_snprintf_rnd_u32(void *ctx, size_t count)
{
    char buf[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%u", ((uint*)ctx)[i]);
    }
    return total;
}

static size_t bench_snprintf_rnd_u64(void *ctx, size_t count)
{
    char buf[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += snprintf(buf, sizeof(buf), "%llu", ((ullong*)ctx)[i]);
    }
    return total;
}

static void header()
{
    printf("%20s %6s %12s %8s %10s %8s %8s\n",
        "benchmark", "loops", "size", "dur(s)", "time(ns)", "MIPS", "mb/sec");
    printf("%20s %6s %12s %8s %10s %8s %8s\n",
        "--------------------", "------", "------------", "--------",
        "----------", "--------", "--------");
}

typedef void* (*bench_setup_fn)(size_t count);
typedef void (*bench_teardown_fn)(void *ctx);
typedef size_t (*bench_loop_fn)(void *ctx, size_t count);

void bench_exec(const char *name, size_t loops, size_t count,
    bench_setup_fn setup, bench_teardown_fn teardown, bench_loop_fn fn)
{
    clock_t start, end;
    size_t total = 0, sum;
    double s, mbsec;
    void *ctx = NULL;

    if (setup) ctx = setup(count);
    start = clock();
    for (size_t j = 0; j < loops; j++) {
        total += fn(ctx, count);
    }
    end = clock();
    if (teardown) teardown(ctx);

    sum = loops * count;
    s = (1e9 * (end - start)) / ((double)CLOCKS_PER_SEC);
    mbsec = ((double)total / (double)(1<<20)) / (s/1e9);

    printf("%20s %6zu %12zu %8.2f %8.2fns %8.3f %8.3f\n",
        name, loops, total, s/1e9, s/sum, (1e3 * sum/s), mbsec);
}

void run_benchmarks(size_t loops)
{
    header();
    bench_exec("utoa_tny_u32", loops, 100000,
                NULL, NULL, bench_utoa_sml_u32);
    bench_exec("utoa_sml_u32", loops, 100000000,
                NULL, NULL, bench_utoa_sml_u32);
    bench_exec("utoa_rnd_u32", loops, 100000,
                rnd_setup_u32, rnd_teardown, bench_utoa_rnd_u32);
    bench_exec("utoa_sml_u64", loops, 100000000,
                NULL, NULL, bench_utoa_sml_u64);
    bench_exec("utoa_big_u64", loops, 100000000,
                NULL, NULL, bench_utoa_big_u64);
    bench_exec("utoa_rnd_u64", loops, 100000,
                rnd_setup_u64, rnd_teardown, bench_utoa_rnd_u64);
    bench_exec("xsnprintf_tny_u32", loops, 10000,
                NULL, NULL, bench_xsnprintf_sml_u32);
    bench_exec("xsnprintf_sml_u32", loops, 100000000,
                NULL, NULL, bench_xsnprintf_sml_u32);
    bench_exec("xsnprintf_rnd_u32", loops, 100000,
                rnd_setup_u32, rnd_teardown, bench_xsnprintf_rnd_u32);
    bench_exec("xsnprintf_sml_u64", loops, 100000000,
                NULL, NULL, bench_xsnprintf_sml_u64);
    bench_exec("xsnprintf_big_u64", loops, 100000000,
                NULL, NULL, bench_xsnprintf_big_u64);
    bench_exec("xsnprintf_rnd_u64", loops, 100000,
                rnd_setup_u64, rnd_teardown, bench_xsnprintf_rnd_u64);
    bench_exec("snprintf_tny_u32", loops, 100000,
                NULL, NULL, bench_snprintf_sml_u32);
    bench_exec("snprintf_sml_u32", loops, 100000000,
                NULL, NULL, bench_snprintf_sml_u32);
    bench_exec("snprintf_rnd_u64", loops, 100000,
                rnd_setup_u32, rnd_teardown, bench_snprintf_rnd_u32);
    bench_exec("snprintf_sml_u64", loops, 100000000,
                NULL, NULL, bench_snprintf_sml_u64);
    bench_exec("snprintf_big_u64", loops, 100000000,
                NULL, NULL, bench_snprintf_big_u64);
    bench_exec("snprintf_rnd_u64", loops, 100000,
                rnd_setup_u64, rnd_teardown, bench_snprintf_rnd_u64);
}

int main(int argc, char **argv)
{
    run_benchmarks(1);
}
