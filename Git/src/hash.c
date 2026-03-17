#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

void sha1_hash(const unsigned char *data, size_t len, unsigned char *out) {
    SHA1(data, len, out);
}

void hash_to_hex(const unsigned char *hash, char *hex) {
    for (int i = 0; i < 20; i++) {
        sprintf(hex + (i * 2), "%02x", hash[i]);
    }
}
