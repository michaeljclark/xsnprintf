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

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "xsnprintf.h"

typedef unsigned int uint;
typedef unsigned long long ullong;

static void test_utoa_u32()
{
    const char* r[11] = {
        "4294967295",
        "429496729",
        "42949672",
        "4294967",
        "429496",
        "42949",
        "4294",
        "429",
        "42",
        "4",
        "0"
    };
    char s[11];
    uint n = 4294967295u;
    for (int i = 0; i < 11; i++) {
        utoa_u32(n, s);
        #if DEBUG
            printf("%s\n", s);
        #endif
        assert(strcmp(r[i], s) == 0);
        n /= 10;
    }
}

static void test_utoa_u64()
{
    const char* r[21] = {
        "18446744073709551615",
        "1844674407370955161",
        "184467440737095516",
        "18446744073709551",
        "1844674407370955",
        "184467440737095",
        "18446744073709",
        "1844674407370",
        "184467440737",
        "18446744073",
        "1844674407",
        "184467440",
        "18446744",
        "1844674",
        "184467",
        "18446",
        "1844",
        "184",
        "18",
        "1",
        "0"
    };
    char s[21];
    ullong n = 18446744073709551615ull;
    for (int i = 0; i < 21; i++) {
        utoa_u64(n, s);
        #if DEBUG
            printf("%s\n", s);
        #endif
        assert(strcmp(r[i], s) == 0);
        n /= 10;
    }
}

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

static size_t test_utoa_rnd_u32(void *ctx, size_t count)
{
    char buf1[11], buf2[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        utoa_u32(((uint*)ctx)[i], buf1);
        snprintf(buf2, sizeof(buf2), "%u", ((uint*)ctx)[i]);
        total += strcmp(buf1, buf2) == 0;
    }
    return total;
}

static size_t test_utoa_rnd_u64(void *ctx, size_t count)
{
    char buf1[21], buf2[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        utoa_u64(((ullong*)ctx)[i], buf1);
        snprintf(buf2, sizeof(buf2), "%llu", ((ullong*)ctx)[i]);
        total += strcmp(buf1, buf2) == 0;
    }
    return total;
}

static size_t test_xsnprintf_rnd_u32(void *ctx, size_t count)
{
    char buf1[11], buf2[11];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        xsnprintf(buf1, sizeof(buf1), "%u", ((uint*)ctx)[i]);
        snprintf(buf2, sizeof(buf2), "%u", ((uint*)ctx)[i]);
        total += strcmp(buf1, buf2) == 0;
    }
    return total;
}

static size_t test_xsnprintf_rnd_u64(void *ctx, size_t count)
{
    char buf1[21], buf2[21];
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        xsnprintf(buf1, sizeof(buf1), "%llu", ((ullong*)ctx)[i]);
        snprintf(buf2, sizeof(buf2), "%llu", ((ullong*)ctx)[i]);
        total += strcmp(buf1, buf2) == 0;
    }
    return total;
}

typedef void* (*test_setup_fn)(size_t count);
typedef void (*test_teardown_fn)(void *ctx);
typedef size_t (*test_loop_fn)(void *ctx, size_t count);

void test_exec(const char *name, size_t count,
    test_setup_fn setup, test_teardown_fn teardown, test_loop_fn fn)
{
    size_t total;
    void *ctx = NULL;

    if (setup) ctx = setup(count);
    total = fn(ctx, count);
    if (teardown) teardown(ctx);
    assert(total == count);
}

void run_tests()
{
    test_utoa_u32();
    test_utoa_u64();
    test_exec("utoa_rnd_u32", 1000000,
                rnd_setup_u32, rnd_teardown, test_utoa_rnd_u32);
    test_exec("utoa_rnd_u64", 1000000,
                rnd_setup_u64, rnd_teardown, test_utoa_rnd_u64);
    test_exec("xsnprintf_rnd_u32", 1000000,
                rnd_setup_u32, rnd_teardown, test_xsnprintf_rnd_u32);
    test_exec("xsnprintf_rnd_u64", 1000000,
                rnd_setup_u64, rnd_teardown, test_xsnprintf_rnd_u64);
}

int main(int argc, char **argv)
{
    run_tests();
}
