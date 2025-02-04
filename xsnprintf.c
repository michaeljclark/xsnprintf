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

#include <stddef.h>

#include "xsnprintf.h"

static const char *hexdigits = "0123456789abcdef";

static size_t xvappend_char(char * restrict out, size_t o, size_t n, char c)
{
    if (out && o < n) {
        out[o] = c;
    }
    return o + 1;
}

static size_t xvappend_hex_u32(char * restrict out, size_t o, size_t n,
    uint val)
{
    size_t dig = (32 - __builtin_clz(val) + 3) / 4;
    for (size_t i = dig-1; i < dig; i--) {
       o = xvappend_char(out, o, n, hexdigits[(val >> (i * 4)) & 0xf]);
    }
    return o;
}

static size_t xvappend_hex_u64(char * restrict out, size_t o, size_t n,
    ullong val)
{
    size_t dig = (64 - __builtin_clzll(val) + 3) / 4;
    for (size_t i = dig-1; i < dig; i--) {
       o = xvappend_char(out, o, n, hexdigits[(val >> (i * 4)) & 0xf]);
    }
    return o;
}

int xvsnprintf(char * restrict out, size_t n, const char* fmt, va_list vl)
{
    int w = -1, s = 0, c;
    size_t o = 0, l;
    const char *v;
    char t[20];
    for( ; *fmt; fmt++) {
        if (w >= 0) {
            switch(*fmt) {
            case 'l':
                w = w < 2 ? w + 1 : w;
                break;
            case 'd':
                s = 1;
            case 'u':
                if (w == 0 || sizeof(long) == 4) {
                    int val = va_arg(vl, int);
                    if (s && val < 0) {
                        val = -val;
                        o = xvappend_char(out, o, n, '-');
                    }
                    l = utoa_u32(val, t + 10);
                }
                else if (w == 2 || sizeof(long) == 8) {
                    llong val = va_arg(vl, llong);
                    if (s && val < 0) {
                        val = -val;
                        o = xvappend_char(out, o, n, '-');
                    }
                    l = utoa_u64(val, t + 0);
                }
                l = n - o < l ? n - o : l;
                memcpy(out + o, t + sizeof(t) - l, l);
                o += l;
                w = -1;
                s = 0;
                break;
            case 'p':
                o = xvappend_char(out, o, n, '0');
                o = xvappend_char(out, o, n, 'x');
                w = 2;
            case 'x':
                if (w == 0 || sizeof(long) == 4) {
                    int val = va_arg(vl, int);
                    o = xvappend_hex_u32(out, o, n, val);
                }
                else if (w == 2 || sizeof(long) == 8) {
                    llong val = va_arg(vl, llong);
                    o = xvappend_hex_u64(out, o, n, val);
                }
                w = -1;
                break;
            case 's':
                v = va_arg(vl, const char *);
                l = strlen(v);
                memcpy(out + o, v, n - o < l ? n - o : l);
                o += l;
                w = -1;
                break;
            case 'c':
                c = va_arg(vl, int);
                o = xvappend_char(out, o, n, (char)c);
                w = -1;
                break;
            }
        }
        else if(*fmt == '%') {
            w = 0;
        }
        else {
            o = xvappend_char(out, o, n, *fmt);
        }
    }
    if (out) {
        out[o < n ? o : n ? n - 1 : 0] = 0;
    }
    return o;
}
