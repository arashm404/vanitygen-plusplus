START_TEST(test_hex_enc)
{
    struct {
        const char* input;
        size_t input_len;
        const char* expect_output;
        size_t expect_len;
    } tests[] = {
        { "\x01", 1, "\x30\x31", 2 },
        { "\x11", 1, "\x31\x31", 2 },
        { "\xff", 1, "\x66\x66", 2 },
        { "\x12\xab", 2, "\x31\x32\x61\x62", 4 },
    };

    size_t n = sizeof(tests) / sizeof(tests[0]);

    for (size_t i = 0; i < n; i++) {
        char got[1024];
        size_t len = 1024;

        hex_enc(got, &len, tests[i].input, tests[i].input_len);

        ck_assert_int_eq(len, tests[i].expect_len);
        ck_assert_mem_eq(got, tests[i].expect_output, len);
    }
}
END_TEST

START_TEST(test_hex_dec)
{
    struct {
        const char* input;
        size_t input_len;
        const char* expect_output;
        size_t expect_len;
    } tests[] = {
        { "\x30\x31", 2, "\x01", 1 },
        { "\x31\x31", 2, "\x11", 1 },
        { "\x46\x46", 2, "\xff", 1 },
        { "\x66\x66", 2, "\xff", 1 },
        { "\x31\x32\x41\x42", 4, "\x12\xab", 2 },
        { "\x31\x32\x61\x62", 4, "\x12\xab", 2 },
    };

    size_t n = sizeof(tests) / sizeof(tests[0]);

    for (size_t i = 0; i < n; i++) {
        char got[1024];
        size_t len = 1024;

        hex_dec(got, &len, tests[i].input, tests[i].input_len);

        ck_assert_int_eq(len, tests[i].expect_len);
        ck_assert_mem_eq(got, tests[i].expect_output, len);
    }
}
END_TEST

START_TEST(test_eth_pubkey2addr)
{
    struct {
        const char* input_hex;
        int addr_format;
        const char* expect_output_hex;
    } tests[] = {
        { "0477eb4560b9535593f074704e9d0e593c4c09b4f8914971ac53766a4ddd0126e15fae8641a3c07d11a78b14dcd1f0f407781c8feac953c23efdfe71a82ccb9a1f",
            VCF_PUBKEY,
            "c660a638f696f8d22d8d593f19dcdbe1bb21716e" },
        { "0452b5bcf0ba1cdf9c4aaaa1463658f1e830b968f07fefe80d8e416758c20c727b83d30cdee31b85a8c92a6555fc00d6d2b0f572755f1880d011d9c5d56f4e9605",
            VCF_PUBKEY,
            "ef5a3f547f84d811998c505f0c5b7a8b74f5b79d" },
        { "0477eb4560b9535593f074704e9d0e593c4c09b4f8914971ac53766a4ddd0126e15fae8641a3c07d11a78b14dcd1f0f407781c8feac953c23efdfe71a82ccb9a1f",
            VCF_CONTRACT,
            "0e1bb5eb3ccfcacf6df3267c2a15705a7ba453b0" },
        { "0452b5bcf0ba1cdf9c4aaaa1463658f1e830b968f07fefe80d8e416758c20c727b83d30cdee31b85a8c92a6555fc00d6d2b0f572755f1880d011d9c5d56f4e9605",
            VCF_CONTRACT,
            "b42efaaf71a364905fd5fe95604dbb63bb2c2674" },
    };

    size_t n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        size_t addr_len = 20;
        char got[addr_len];

        char input[65];
        memcpy(input, from_hex(tests[i].input_hex), sizeof(input));

        char expect_output[addr_len];
        memcpy(expect_output, from_hex(tests[i].expect_output_hex), sizeof(expect_output));

        eth_pubkey2addr((const unsigned char*)input, tests[i].addr_format, (unsigned char*)got);

        ck_assert_mem_eq(got, expect_output, addr_len);
    }
}
END_TEST

START_TEST(test_eth_encode_checksum_addr)
{
    struct {
        const char* input_hex;
        const char* expect_output;
    } tests[] = {
        /* All caps */
        { "52908400098527886e0f7030069857d2e4169ee7", "52908400098527886E0F7030069857D2E4169EE7" },
        /* All Lower */
        { "de709f2102306220921060314715629080e2fb77", "de709f2102306220921060314715629080e2fb77" },
        /* Normal */
        { "5aaeb6053f3e94c9b9a09f33669435e7ef1beaed", "5aAeb6053F3E94C9b9A09f33669435E7Ef1BeAed" },
    };

    size_t n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        size_t checksum_addr = 40;
        char got[checksum_addr];

        size_t input_len = 20;
        char input[input_len];
        memcpy(input, from_hex(tests[i].input_hex), sizeof(input));

        eth_encode_checksum_addr((void*)input, input_len, (char*)got, checksum_addr);

        ck_assert_mem_eq(got, tests[i].expect_output, checksum_addr);
    }
}
END_TEST
