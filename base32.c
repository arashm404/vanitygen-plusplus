/**
 * base32 (de)coder implementation as specified by RFC4648.
 *
 * Copyright (c) 2010 Adrien Kunysz
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
 **/

#include <assert.h>  // assert()
#include <limits.h>  // CHAR_BIT
#include <string.h>  // for memset

#include "base32.h"

/* Precomputed maps for RFC4648 base32 */
static const int octet_map[8]   = {0, 0, 1, 1, 2, 3, 3, 4};
static const int offset_map[8]  = { 3, -2,  1, -4,  3, -2,  1, -4};

/**
 * Let this be a sequence of plain data before encoding:
 *
 *  01234567 01234567 01234567 01234567 01234567
 * +--------+--------+--------+--------+--------+
 * |< 0 >< 1| >< 2 ><|.3 >< 4.|>< 5 ><.|6 >< 7 >|
 * +--------+--------+--------+--------+--------+
 *
 * There are 5 octets of 8 bits each in each sequence.
 * There are 8 blocks of 5 bits each in each sequence.
 *
 * You probably want to refer to that graph when reading the algorithms in this
 * file. We use "octet" instead of "byte" intentionnally as we really work with
 * 8 bits quantities. This implementation will probably not work properly on
 * systems that don't have exactly 8 bits per (unsigned) char.
 **/

static size_t min(size_t x, size_t y)
{
    return x < y ? x : y;
}

static const unsigned char PADDING_CHAR = 0; // '=';

/**
 * Pad the given buffer with len padding characters.
 */
static void pad(unsigned char *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
        buf[i] = PADDING_CHAR;
}

/**
 * This convert a 5 bits value into a base32 character.
 * Only the 5 least significant bits are used.
 */
static unsigned char encode_char(unsigned char c)
{
    static unsigned char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    return base32[c & 0x1F];  // 0001 1111
}

/**
 * Decode given character into a 5 bits value.
 * Returns -1 iff the argument given was an invalid base32 character
 * or a padding character.
 */
static int decode_char(unsigned char c)
{
    char retval = -1;

    if (c >= 'A' && c <= 'Z')
        retval = c - 'A';
    if (c >= '2' && c <= '7')
        retval = c - '2' + 26;

    assert(retval == -1 || ((retval & 0x1F) == retval));

    return  retval;
}

/**
 * Encode a sequence. A sequence is no longer than 5 octets by definition.
 * Thus passing a length greater than 5 to this function is an error. Encoding
 * sequences shorter than 5 octets is supported and padding will be added to the
 * output as per the specification.
 */
static void encode_sequence(const unsigned char *plain, int len, unsigned char *coded)
{
    assert(CHAR_BIT == 8);
    assert(len >= 0 && len <= 5);

    for (int block = 0; block < 8; ++block) {
        int octet = octet_map[block];
        if (octet >= len) {
            /* pad remaining output */
            memset(coded + block, PADDING_CHAR, 8 - block);
            return;
        }
        int junk = offset_map[block];
        unsigned char part = (junk > 0)
            ? (plain[octet] >> junk)
            : (plain[octet] << -junk);

        if (junk < 0 && octet + 1 < len) {
            /* combine bits from next octet */
            part |= (plain[octet + 1] >> (8 + junk));
        }
        coded[block] = encode_char(part);
    }
}

void base32_encode(const unsigned char *plain, size_t len, unsigned char *coded)
{
    // All the hard work is done in encode_sequence(),
    // here we just need to feed it the data sequence by sequence.
    size_t i, j;
    for (i = 0, j = 0; i < len; i += 5, j += 8) {
        encode_sequence(&plain[i], min(len - i, 5), &coded[j]);
    }
}

static int decode_sequence(const unsigned char *coded, unsigned char *plain)
{
    assert(CHAR_BIT == 8);
    assert(coded && plain);

    /* initialize plain to zero for up to 5 octets */
    memset(plain, 0, 5);

    for (int block = 0; block < 8; ++block) {
        int c = decode_char(coded[block]);
        if (c < 0) {
            /* invalid or padding - return bytes written so far */
            return octet_map[block];
        }
        int offset = offset_map[block];
        int octet = octet_map[block];
        plain[octet] |= (unsigned char)(c << offset);
        if (offset < 0 && octet + 1 < 5) {
            plain[octet + 1] |= (unsigned char)(c >> (8 + offset));
        }
    }
    return 5;
}

size_t base32_decode(const unsigned char *coded, unsigned char *plain)
{
    size_t written = 0;
    size_t i, j;
    for (i = 0, j = 0; ; i += 8, j += 5) {
        int n = decode_sequence(&coded[i], &plain[j]);
        written += n;
        if (n < 5)
            return written;
    }
}
