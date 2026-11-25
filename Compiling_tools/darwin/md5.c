//Copyright>        OpenCourant
//Copyright>        Copyright (C) 2026 OpenCourant contributors
//Copyright>
//Copyright>        This program is free software: you can redistribute it and/or modify
//Copyright>        it under the terms of the GNU Affero General Public License as published by
//Copyright>        the Free Software Foundation, either version 3 of the License, or
//Copyright>        (at your option) any later version.
//Copyright>
//Copyright>        This program is distributed in the hope that it will be useful,
//Copyright>        but WITHOUT ANY WARRANTY; without even the implied warranty of
//Copyright>        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//Copyright>        GNU Affero General Public License for more details.
//Copyright>
//Copyright>        You should have received a copy of the GNU Affero General Public License
//Copyright>        along with this program.  If not, see <https://www.gnu.org/licenses/>.
//Copyright>
//Copyright>
//Copyright>        This file is part of OpenCourant, a fork of OpenRadioss.
//Copyright>        See COPYRIGHT.md at the root of the repository for the full
//Copyright>        copyright and attribution statement.
/*
 * MD5 message digest (RFC 1321) for the macOS (darwin) build.
 *
 * extlib ships libmd5 only as prebuilt Linux and Windows archives. This
 * file implements the same API declared in extlib/md5/include/md5.h
 * (md5_init / md5_append / md5_finish on md5_state_t) so that the macOS
 * build can compile it directly into Starter and Engine.
 *
 * Written from the algorithm description in RFC 1321. It is only linked
 * into builds whose CMake_Compilers/cmake_darwin_*.txt adds it.
 */

#include <string.h>
#include "md5.h"

#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define STEP(f, a, b, c, d, x, s, t) \
    (a) += f((b), (c), (d)) + (x) + (md5_word_t)(t); \
    (a) = ROTL((a), (s)) + (b)

static void md5_process(md5_state_t *pms, const md5_byte_t *block)
{
    md5_word_t a = pms->abcd[0];
    md5_word_t b = pms->abcd[1];
    md5_word_t c = pms->abcd[2];
    md5_word_t d = pms->abcd[3];
    md5_word_t X[16];
    int i;

    /* Load the block as 16 little-endian 32-bit words. */
    for (i = 0; i < 16; ++i) {
        X[i] = (md5_word_t)block[4 * i]
             | ((md5_word_t)block[4 * i + 1] << 8)
             | ((md5_word_t)block[4 * i + 2] << 16)
             | ((md5_word_t)block[4 * i + 3] << 24);
    }

    /* Round 1 */
    STEP(F, a, b, c, d, X[ 0],  7, 0xd76aa478);
    STEP(F, d, a, b, c, X[ 1], 12, 0xe8c7b756);
    STEP(F, c, d, a, b, X[ 2], 17, 0x242070db);
    STEP(F, b, c, d, a, X[ 3], 22, 0xc1bdceee);
    STEP(F, a, b, c, d, X[ 4],  7, 0xf57c0faf);
    STEP(F, d, a, b, c, X[ 5], 12, 0x4787c62a);
    STEP(F, c, d, a, b, X[ 6], 17, 0xa8304613);
    STEP(F, b, c, d, a, X[ 7], 22, 0xfd469501);
    STEP(F, a, b, c, d, X[ 8],  7, 0x698098d8);
    STEP(F, d, a, b, c, X[ 9], 12, 0x8b44f7af);
    STEP(F, c, d, a, b, X[10], 17, 0xffff5bb1);
    STEP(F, b, c, d, a, X[11], 22, 0x895cd7be);
    STEP(F, a, b, c, d, X[12],  7, 0x6b901122);
    STEP(F, d, a, b, c, X[13], 12, 0xfd987193);
    STEP(F, c, d, a, b, X[14], 17, 0xa679438e);
    STEP(F, b, c, d, a, X[15], 22, 0x49b40821);

    /* Round 2 */
    STEP(G, a, b, c, d, X[ 1],  5, 0xf61e2562);
    STEP(G, d, a, b, c, X[ 6],  9, 0xc040b340);
    STEP(G, c, d, a, b, X[11], 14, 0x265e5a51);
    STEP(G, b, c, d, a, X[ 0], 20, 0xe9b6c7aa);
    STEP(G, a, b, c, d, X[ 5],  5, 0xd62f105d);
    STEP(G, d, a, b, c, X[10],  9, 0x02441453);
    STEP(G, c, d, a, b, X[15], 14, 0xd8a1e681);
    STEP(G, b, c, d, a, X[ 4], 20, 0xe7d3fbc8);
    STEP(G, a, b, c, d, X[ 9],  5, 0x21e1cde6);
    STEP(G, d, a, b, c, X[14],  9, 0xc33707d6);
    STEP(G, c, d, a, b, X[ 3], 14, 0xf4d50d87);
    STEP(G, b, c, d, a, X[ 8], 20, 0x455a14ed);
    STEP(G, a, b, c, d, X[13],  5, 0xa9e3e905);
    STEP(G, d, a, b, c, X[ 2],  9, 0xfcefa3f8);
    STEP(G, c, d, a, b, X[ 7], 14, 0x676f02d9);
    STEP(G, b, c, d, a, X[12], 20, 0x8d2a4c8a);

    /* Round 3 */
    STEP(H, a, b, c, d, X[ 5],  4, 0xfffa3942);
    STEP(H, d, a, b, c, X[ 8], 11, 0x8771f681);
    STEP(H, c, d, a, b, X[11], 16, 0x6d9d6122);
    STEP(H, b, c, d, a, X[14], 23, 0xfde5380c);
    STEP(H, a, b, c, d, X[ 1],  4, 0xa4beea44);
    STEP(H, d, a, b, c, X[ 4], 11, 0x4bdecfa9);
    STEP(H, c, d, a, b, X[ 7], 16, 0xf6bb4b60);
    STEP(H, b, c, d, a, X[10], 23, 0xbebfbc70);
    STEP(H, a, b, c, d, X[13],  4, 0x289b7ec6);
    STEP(H, d, a, b, c, X[ 0], 11, 0xeaa127fa);
    STEP(H, c, d, a, b, X[ 3], 16, 0xd4ef3085);
    STEP(H, b, c, d, a, X[ 6], 23, 0x04881d05);
    STEP(H, a, b, c, d, X[ 9],  4, 0xd9d4d039);
    STEP(H, d, a, b, c, X[12], 11, 0xe6db99e5);
    STEP(H, c, d, a, b, X[15], 16, 0x1fa27cf8);
    STEP(H, b, c, d, a, X[ 2], 23, 0xc4ac5665);

    /* Round 4 */
    STEP(I, a, b, c, d, X[ 0],  6, 0xf4292244);
    STEP(I, d, a, b, c, X[ 7], 10, 0x432aff97);
    STEP(I, c, d, a, b, X[14], 15, 0xab9423a7);
    STEP(I, b, c, d, a, X[ 5], 21, 0xfc93a039);
    STEP(I, a, b, c, d, X[12],  6, 0x655b59c3);
    STEP(I, d, a, b, c, X[ 3], 10, 0x8f0ccc92);
    STEP(I, c, d, a, b, X[10], 15, 0xffeff47d);
    STEP(I, b, c, d, a, X[ 1], 21, 0x85845dd1);
    STEP(I, a, b, c, d, X[ 8],  6, 0x6fa87e4f);
    STEP(I, d, a, b, c, X[15], 10, 0xfe2ce6e0);
    STEP(I, c, d, a, b, X[ 6], 15, 0xa3014314);
    STEP(I, b, c, d, a, X[13], 21, 0x4e0811a1);
    STEP(I, a, b, c, d, X[ 4],  6, 0xf7537e82);
    STEP(I, d, a, b, c, X[11], 10, 0xbd3af235);
    STEP(I, c, d, a, b, X[ 2], 15, 0x2ad7d2bb);
    STEP(I, b, c, d, a, X[ 9], 21, 0xeb86d391);

    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = 0xefcdab89;
    pms->abcd[2] = 0x98badcfe;
    pms->abcd[3] = 0x10325476;
}

void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0)
        return;

    /* Update the 64-bit message length in bits. */
    pms->count[1] += (md5_word_t)nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
        pms->count[1]++;

    /* Complete a partial block left over from a previous call. */
    if (offset) {
        int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

        memcpy(pms->buf + offset, p, copy);
        if (offset + copy < 64)
            return;
        p += copy;
        left -= copy;
        md5_process(pms, pms->buf);
    }

    /* Process full blocks directly from the input. */
    for (; left >= 64; p += 64, left -= 64)
        md5_process(pms, p);

    /* Keep the remainder for the next call. */
    if (left)
        memcpy(pms->buf, p, left);
}

void md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = { 0x80 };
    md5_byte_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
        data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64, then append the length. */
    md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
        digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}
