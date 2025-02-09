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
    const uchar utodb_u32[32] = {
         9,  9,  9,  8,  8,  8,  7,  7,
         7,  6,  6,  6,  6,  5,  5,  5,
         4,  4,  4,  3,  3,  3,  3,  2,
         2,  2,  1,  1,  1,  1,  1,  1,
    };
    const uint utodd_u32[10] = {
        999999999u,  99999999u,
          9999999u,    999999u,
            99999u,      9999u,
              999u,        99u,
                9u,         0u,
    };
    uint x1, x2, a1, a2, a3, b1, b2;
    uint dig = n == 0 ? 1 : utodb_u32[__builtin_clz(n)];
    dig += n > utodd_u32[9 - dig];
    x1 = n % 1000000; x2 = n / 1000000;
    switch (dig) {
    case 9:
        b2 = x2 / 100 % 100;
        *s++ = dd[b2 * 2 + 1];
        goto l8;
    case 7:
        b1 = x2 % 100;
        *s++ = dd[b1 * 2 + 1];
        goto l6;
    case 5:
        a3 = x1 / 10000;
        *s++ = dd[a3 * 2 + 1];
        goto l4;
    case 3:
        a2 = x1 / 100 % 100;
        *s++ = dd[a2 * 2 + 1];
        goto l2;
    case 1:
        a1 = x1 % 100;
        *s++ = dd[a1 * 2 + 1];
        break;
    case 10:
        b2 = x2 / 100 % 100;
        *s++ = dd[b2 * 2];
        *s++ = dd[b2 * 2 + 1];
        /* fallthrough */
    case 8:  l8:
        b1 = x2 % 100;
        *s++ = dd[b1 * 2];
        *s++ = dd[b1 * 2 + 1];
        /* fallthrough */
    case 6:  l6:
        a3 = x1 / 10000;
        *s++ = dd[a3 * 2];
        *s++ = dd[a3 * 2 + 1];
        /* fallthrough */
    case 4:  l4:
        a2 = x1 / 100 % 100;
        *s++ = dd[a2 * 2];
        *s++ = dd[a2 * 2 + 1];
        /* fallthrough */
    case 2:  l2:
        a1 = x1 % 100;
        *s++ = dd[a1 * 2];
        *s++ = dd[a1 * 2 + 1];
        break;
    }
    *s++ = '\0';
    return dig;
}

static inline size_t utoa_u64(ullong n, char *s)
{
    const uchar utodb_u64[64] = {
        19, 18, 18, 18, 18, 17, 17, 17,
        16, 16, 16, 15, 15, 15, 15, 14,
        14, 14, 13, 13, 13, 12, 12, 12,
        12, 11, 11, 11, 10, 10, 10,  9,
         9,  9,  9,  8,  8,  8,  7,  7,
         7,  6,  6,  6,  6,  5,  5,  5,
         4,  4,  4,  3,  3,  3,  3,  2,
         2,  2,  1,  1,  1,  1,  1,  1,
    };
    const ullong utodd_u64[20] = {
        9999999999999999999llu,  999999999999999999llu,
          99999999999999999llu,    9999999999999999llu,
            999999999999999llu,      99999999999999llu,
              9999999999999llu,        999999999999llu,
                99999999999llu,          9999999999llu,
                  999999999llu,            99999999llu,
                    9999999llu,              999999llu,
                      99999llu,                9999llu,
                        999llu,                  99llu,
                          9llu,                   0llu,
    };
    ullong x1, x2;
    uint y1, y2, z1, z2, a1, a2, a3, b1, b2, b3, c1, c2, c3, d1;
    uint dig = n == 0 ? 1 : utodb_u64[__builtin_clzll(n)];
    dig += n > utodd_u64[19 - dig];
    x1 = n  % 1000000000000ll; x2 = n  / 1000000000000ll;
    y1 = x1 % 1000000ll;       y2 = x1 / 1000000ll;
    z1 = x2 % 1000000ll;       z2 = x2 / 1000000ll;
    switch (dig) {
    case 19:
        d1 = z2 % 100;
        *s++ = dd[d1 * 2 + 1];
        goto l18;
    case 17:
        c3 = z1 / 10000;
        *s++ = dd[c3 * 2 + 1];
        goto l16;
    case 15:
        c2 = z1 / 100 % 100;
        *s++ = dd[c2 * 2 + 1];
        goto l14;
    case 13:
        c1 = z1 % 100;
        *s++ = dd[c1 * 2 + 1];
        goto l12;
    case 11:
        b3 = y2 / 10000;
        *s++ = dd[b3 * 2 + 1];
        goto l10;
    case 9:
        b2 = y2 / 100 % 100;
        *s++ = dd[b2 * 2 + 1];
        goto l8;
    case 7:
        b1 = y2 % 100;
        *s++ = dd[b1 * 2 + 1];
        goto l6;
    case 5:
        a3 = y1 / 10000;
        *s++ = dd[a3 * 2 + 1];
        goto l4;
    case 3:
        a2 = y1 / 100 % 100;
        *s++ = dd[a2 * 2 + 1];
        goto l2;
    case 1:
        a1 = y1 % 100;
        *s++ = dd[a1 * 2 + 1];
        break;
    case 20:
        d1 = z2 % 100;
        *s++ = dd[d1 * 2];
        *s++ = dd[d1 * 2 + 1];
        /* fallthrough */
    case 18: l18:
        c3 = z1 / 10000;
        *s++ = dd[c3 * 2];
        *s++ = dd[c3 * 2 + 1];
        /* fallthrough */
    case 16: l16:
        c2 = z1 / 100 % 100;
        *s++ = dd[c2 * 2];
        *s++ = dd[c2 * 2 + 1];
        /* fallthrough */
    case 14: l14:
        c1 = z1 % 100;
        *s++ = dd[c1 * 2];
        *s++ = dd[c1 * 2 + 1];
        /* fallthrough */
    case 12: l12:
        b3 = y2 / 10000;
        *s++ = dd[b3 * 2];
        *s++ = dd[b3 * 2 + 1];
        /* fallthrough */
    case 10: l10:
        b2 = y2 / 100 % 100;
        *s++ = dd[b2 * 2];
        *s++ = dd[b2 * 2 + 1];
        /* fallthrough */
    case 8:  l8:
        b1 = y2 % 100;
        *s++ = dd[b1 * 2];
        *s++ = dd[b1 * 2 + 1];
        /* fallthrough */
    case 6:  l6:
        a3 = y1 / 10000;
        *s++ = dd[a3 * 2];
        *s++ = dd[a3 * 2 + 1];
        /* fallthrough */
    case 4:  l4:
        a2 = y1 / 100 % 100;
        *s++ = dd[a2 * 2];
        *s++ = dd[a2 * 2 + 1];
        /* fallthrough */
    case 2:  l2:
        a1 = y1 % 100;
        *s++ = dd[a1 * 2];
        *s++ = dd[a1 * 2 + 1];
        break;
    }
    *s++ = '\0';
    return dig;
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
