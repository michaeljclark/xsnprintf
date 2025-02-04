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

#pragma once

#include <stdarg.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef signed long long llong;
typedef unsigned long long ullong;

static const char dd[200] = {
    0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x33, 0x30, 0x34,
    0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38, 0x30, 0x39,
    0x31, 0x30, 0x31, 0x31, 0x31, 0x32, 0x31, 0x33, 0x31, 0x34,
    0x31, 0x35, 0x31, 0x36, 0x31, 0x37, 0x31, 0x38, 0x31, 0x39,
    0x32, 0x30, 0x32, 0x31, 0x32, 0x32, 0x32, 0x33, 0x32, 0x34,
    0x32, 0x35, 0x32, 0x36, 0x32, 0x37, 0x32, 0x38, 0x32, 0x39,
    0x33, 0x30, 0x33, 0x31, 0x33, 0x32, 0x33, 0x33, 0x33, 0x34,
    0x33, 0x35, 0x33, 0x36, 0x33, 0x37, 0x33, 0x38, 0x33, 0x39,
    0x34, 0x30, 0x34, 0x31, 0x34, 0x32, 0x34, 0x33, 0x34, 0x34,
    0x34, 0x35, 0x34, 0x36, 0x34, 0x37, 0x34, 0x38, 0x34, 0x39,
    0x35, 0x30, 0x35, 0x31, 0x35, 0x32, 0x35, 0x33, 0x35, 0x34,
    0x35, 0x35, 0x35, 0x36, 0x35, 0x37, 0x35, 0x38, 0x35, 0x39,
    0x36, 0x30, 0x36, 0x31, 0x36, 0x32, 0x36, 0x33, 0x36, 0x34,
    0x36, 0x35, 0x36, 0x36, 0x36, 0x37, 0x36, 0x38, 0x36, 0x39,
    0x37, 0x30, 0x37, 0x31, 0x37, 0x32, 0x37, 0x33, 0x37, 0x34,
    0x37, 0x35, 0x37, 0x36, 0x37, 0x37, 0x37, 0x38, 0x37, 0x39,
    0x38, 0x30, 0x38, 0x31, 0x38, 0x32, 0x38, 0x33, 0x38, 0x34,
    0x38, 0x35, 0x38, 0x36, 0x38, 0x37, 0x38, 0x38, 0x38, 0x39,
    0x39, 0x30, 0x39, 0x31, 0x39, 0x32, 0x39, 0x33, 0x39, 0x34,
    0x39, 0x35, 0x39, 0x36, 0x39, 0x37, 0x39, 0x38, 0x39, 0x39
};

static inline size_t utoa_u32(uint n, char *s)
{
    static const uchar utod_u32[32] = {
         10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  8,  6,  6,  6,
         6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2
    };
    char *e, *r;
    uint x1, x2, a1, a2, a3, b1, b2;
    memset(s, '0', 10);
    if (n == 0) return 1;
    uint dig = utod_u32[__builtin_clz(n)];
    x1 = n % 1000000; x2 = n / 1000000;
    switch (dig) {
    case 10: b2 = x2 / 100 % 100; s[0] = dd[b2*2]; s[1] = dd[b2*2+1];
    case 8:  b1 = x2 % 100;       s[2] = dd[b1*2]; s[3] = dd[b1*2+1];
    case 6:  a3 = x1 / 10000;     s[4] = dd[a3*2]; s[5] = dd[a3*2+1];
    case 4:  a2 = x1 / 100 % 100; s[6] = dd[a2*2]; s[7] = dd[a2*2+1];
    case 2:  a1 = x1 % 100;       s[8] = dd[a1*2]; s[9] = dd[a1*2+1];
    }
    e = s + 10; r = e - dig;
    while (r < e && *r == '0') r++;
    return e - r;
}

static inline size_t utoa_u64(ullong n, char *s)
{
    static const uchar utod_u64[64] = {
        20, 20, 20, 20, 20, 18, 18, 18, 18, 18, 18, 16, 16, 16, 16, 16,
        16, 16, 14, 14, 14, 14, 14, 14, 14, 12, 12, 12, 12, 12, 12, 10,
        10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  8,  6,  6,  6,
        6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2
    };
    char *e, *r;
    ullong x1, x2;
    uint y1, y2, z1, z2, a1, a2, a3, b1, b2, b3, c1, c2, c3, d1;
    memset(s, '0', 20);
    if (n == 0) return 1;
    uint dig = utod_u64[__builtin_clzll(n)];
    x1 = n  % 1000000000000ll; x2 = n  / 1000000000000ll;
    y1 = x1 % 1000000ll;       y2 = x1 / 1000000ll;
    z1 = x2 % 1000000ll;       z2 = x2 / 1000000ll;
    switch (dig) {
    case 20: d1 = z2 % 100;       s[0]  = dd[d1*2];  s[1] = dd[d1*2+1];
    case 18: c3 = z1 / 10000;     s[2]  = dd[c3*2];  s[3] = dd[c3*2+1];
    case 16: c2 = z1 / 100 % 100; s[4]  = dd[c2*2];  s[5] = dd[c2*2+1];
    case 14: c1 = z1 % 100;       s[6]  = dd[c1*2];  s[7] = dd[c1*2+1];
    case 12: b3 = y2 / 10000;     s[8]  = dd[b3*2];  s[9] = dd[b3*2+1];
    case 10: b2 = y2 / 100 % 100; s[10] = dd[b2*2]; s[11] = dd[b2*2+1];
    case 8:  b1 = y2 % 100;       s[12] = dd[b1*2]; s[13] = dd[b1*2+1];
    case 6:  a3 = y1 / 10000;     s[14] = dd[a3*2]; s[15] = dd[a3*2+1];
    case 4:  a2 = y1 / 100 % 100; s[16] = dd[a2*2]; s[17] = dd[a2*2+1];
    case 2:  a1 = y1 % 100;       s[18] = dd[a1*2]; s[19] = dd[a1*2+1];
    }
    e = s + 20; r = e - dig;
    while (r < e && *r == '0') r++;
    return e - r;
}

int xvsnprintf(char * restrict out, size_t n, const char* fmt, va_list vl);

static inline int xsnprintf(char *out, size_t n, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int res = xvsnprintf(out, n, fmt, vl);
    va_end(vl);
    return res;
}
