/*
 * Vanitygen, vanity bitcoin address generator
 * Copyright (C) 2011 <samr7@cs.washington.edu>
 *
 * Vanitygen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version. 
 *
 * Vanitygen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Vanitygen.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdio.h>
#include <stdint.h>

#include <openssl/bn.h>
#include <openssl/ec.h>

#include <stddef.h>    // for size_t
#if defined(__GNUC__) || defined(__clang__)
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

extern int GRSFlag;
extern int TRXFlag;

extern const char *vg_b58_alphabet;
extern const signed char vg_b58_reverse_map[256];

extern void fdumphex(FILE *fp, const unsigned char *restrict src, size_t len);
extern void fdumpbn(FILE *fp, const BIGNUM *restrict bn);
extern void dumphex(const unsigned char *restrict src, size_t len);
extern void dumpbn(const BIGNUM *restrict bn);

extern void vg_b58_encode_check(void *restrict buf, size_t len, char *restrict result);
extern int vg_b58_decode_check(const char *restrict input, void *restrict buf, size_t len);

extern void vg_encode_address(const EC_POINT *restrict ppoint, const EC_GROUP *restrict pgroup,
			      int addrtype, int addrformat, char *restrict result);
extern void vg_encode_address_compressed(const EC_POINT *restrict ppoint, const EC_GROUP *restrict pgroup,
			      int addrtype, char *restrict result);
extern void vg_encode_script_address(const EC_POINT *restrict ppoint,
				     const EC_GROUP *restrict pgroup,
				     int addrtype, char *restrict result);
extern void vg_encode_privkey(const EC_KEY *restrict pkey, int privtype, char *restrict result);
extern void vg_encode_privkey_compressed(const EC_KEY *restrict pkey, int privtype, char *restrict result);
extern int vg_set_privkey(const BIGNUM *restrict bnpriv, EC_KEY *restrict pkey);
extern int vg_decode_privkey(const char *restrict b58encoded,
			     EC_KEY *restrict pkey, int *restrict addrtype);

enum {
	VG_PROTKEY_DEFAULT = -1,
	VG_PROTKEY_BRIEF_PBKDF2_4096_HMAC_SHA256_AES_256_CBC = 0,
	VG_PROTKEY_PKCS_PBKDF2_4096_HMAC_SHA256_AES_256_CBC = 16,
};

#define VG_PROTKEY_MAX_B58 128

extern int vg_protect_encode_privkey(char *restrict out,
				     const EC_KEY *restrict pkey, int keytype,
				     int parameter_group,
				     const char *restrict pass);
extern int vg_protect_decode_privkey(EC_KEY *restrict pkey, int *restrict keytype,
				     const char *restrict encoded, const char *restrict pass);

extern int vg_pkcs8_encode_privkey(char *restrict out, int outlen,
				   const EC_KEY *restrict pkey,
				   const char *restrict pass);
extern int vg_pkcs8_decode_privkey(EC_KEY *restrict pkey, const char *restrict pem_in,
				   const char *restrict pass);

extern int vg_decode_privkey_any(EC_KEY *restrict pkey, int *restrict addrtype,
				 const char *restrict input, const char *restrict pass);

extern int vg_read_password(char *restrict buf, size_t size);
extern int vg_check_password_complexity(const char *restrict pass, int verbose);

extern int vg_read_file(FILE *restrict fp, char ***restrict result, int *restrict rescount);

extern void vg_print_alicoin_help_msg(void);
extern int vg_get_altcoin(char *restrict altcoin, int *restrict addrtype, int *restrict privtype, char **restrict hrp);

extern int count_processors(void);

extern int hex_dec(void *restrict bin, size_t *restrict binszp, const char *restrict hex, size_t hexsz);
extern int hex_enc(char *restrict hex, size_t *restrict hexszp, const void *restrict data, size_t binsz);

extern void eth_pubkey2addr(const unsigned char *restrict pubkey_buf, int addrformat, unsigned char *restrict out_buf);

extern void eth_encode_checksum_addr(const void *restrict input, int inlen, char *restrict output, int outlen);

extern void copy_nbits(unsigned char *restrict dst, unsigned char *restrict src, int nbits);
