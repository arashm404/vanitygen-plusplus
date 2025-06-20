#include "util.h"
#include "pattern.h"
#include "segwit_addr.h"
#include <check.h>

#define FROM_HEX_MAXLEN 512

static inline int hexval(unsigned char c) {
    // Convert ASCII hex digit to 0–15, returns 0 for invalid
    c |= 32; // map 'A'-'F' to 'a'-'f'
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

const uint8_t *from_hex(const char *str) {
    static uint8_t buf[FROM_HEX_MAXLEN];
    const char *p = str;
    uint8_t *q = buf;
    size_t len = strlen(str) >> 1;
    if (len > FROM_HEX_MAXLEN) len = FROM_HEX_MAXLEN;
    for (size_t i = 0; i < len; i++) {
        int hi = hexval((unsigned char)*p++);
        int lo = hexval((unsigned char)*p++);
        *q++ = (hi << 4) | lo;
    }
    return buf;
}


START_TEST(test_sample)
{
    ck_assert_int_eq(512, 512);
    ck_assert_str_eq("Hello World", "Hello World");
}
END_TEST

#include "util_test.h"
#include "segwit_addr_test.h"
#include "pattern_test.h"

Suite* create_sample_suite(void)
{
    Suite* suite = suite_create("Sample suite");
    TCase* tc;

    tc = tcase_create("sample test case");
    tcase_add_test(tc, test_sample);
    suite_add_tcase(suite, tc);

    tc = tcase_create("util hex test");
    tcase_add_test(tc, test_hex_enc);
    tcase_add_test(tc, test_hex_dec);
    suite_add_tcase(suite, tc);

	tc = tcase_create("util eth test");
	tcase_add_test(tc, test_eth_pubkey2addr);
	tcase_add_test(tc, test_eth_encode_checksum_addr);
	suite_add_tcase(suite, tc);

	tc = tcase_create("segwit addr test");
	tcase_add_test(tc, test_segwit_addr_encode);
	suite_add_tcase(suite, tc);

	tc = tcase_create("pattern func test");
	tcase_add_test(tc, test_get_prefix_ranges);
	suite_add_tcase(suite, tc);

    return suite;
}
