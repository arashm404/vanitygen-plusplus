/* From: https://github.com/sipa/bech32/blob/master/ref/c/segwit_addr.c */
/* Copyright (c) 2017, 2021 Pieter Wuille
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bech32.h"
#include "segwit_addr.h"
#if defined(__GNUC__) || defined(__clang__)
#  define INLINE static inline __attribute__((always_inline))
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define INLINE static inline
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

INLINE int segwit_addr_encode(
    char *restrict output,
    const char *restrict hrp,
    int witver,
    const uint8_t *restrict witprog,
    size_t witprog_len
) {
    uint8_t data[65];
    size_t datalen = 0;
    bech32_encoding enc = BECH32_ENCODING_BECH32;
    if (unlikely(witver > 16)) return 0;
    if (unlikely(witver == 0 && witprog_len != 20 && witprog_len != 32)) return 0;
    if (unlikely(witprog_len < 2 || witprog_len > 40)) return 0;
    if (witver > 0) enc = BECH32_ENCODING_BECH32M;
    data[0] = witver;
    convert_bits(data + 1, &datalen, 5, witprog, witprog_len, 8, 1);
    ++datalen;
    return bech32_encode(output, hrp, data, datalen, enc);
}

INLINE int segwit_addr_decode(
    int *restrict witver,
    uint8_t *restrict witdata,
    size_t *restrict witdata_len,
    const char *restrict hrp,
    const char *restrict addr
) {
    uint8_t data[84];
    char hrp_actual[84];
    size_t data_len;
    size_t hrplen = strlen(hrp);
    bech32_encoding enc = bech32_decode(hrp_actual, data, &data_len, addr);
    if (unlikely(enc == BECH32_ENCODING_NONE)) return 0;
    if (unlikely(data_len == 0 || data_len > 65)) return 0;
    if (unlikely(strncmp(hrp_actual, hrp, hrplen) != 0 || hrp_actual[hrplen] != '\0')) return 0;
    if (unlikely(data[0] > 16)) return 0;
    if (unlikely(data[0] == 0 && enc != BECH32_ENCODING_BECH32)) return 0;
    if (unlikely(data[0] > 0 && enc != BECH32_ENCODING_BECH32M)) return 0;
    *witdata_len = 0;
    if (!convert_bits(witdata, witdata_len, 8, data + 1, data_len - 1, 5, 0)) return 0;
    if (*witdata_len < 2 || *witdata_len > 40) return 0;
    if (data[0] == 0 && *witdata_len != 20 && *witdata_len != 32) return 0;
    *witver = data[0];
    return 1;
}
