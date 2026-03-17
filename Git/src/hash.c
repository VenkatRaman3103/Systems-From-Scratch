#include <ctype.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int hex_char_to_int(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

void hex_to_hash(const char *hex, unsigned char *hash_out) {
    for (int i = 0; i < 20; i++) {
        int hi = hex_char_to_int(hex[i * 2]);
        int lo = hex_char_to_int(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) {
            memset(hash_out, 0, 20);
            return;
        }
        hash_out[i] = (hi << 4) | lo;
    }
}

void hash_to_hex(const unsigned char *hash, char *hex_out) {
    for (int i = 0; i < 20; i++) {
        sprintf(hex_out + i * 2, "%02x", hash[i]);
    }
}

void sha1_hash(const unsigned char *data, size_t len, unsigned char *out) {
    SHA1(data, len, out);
}
