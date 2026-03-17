#ifndef HASH_H
#define HASH_H

#include <stddef.h>

void sha1_hash(const unsigned char *data, size_t len, unsigned char *out);
void hex_to_hash(const char *hex, unsigned char *hash_out);
void hash_to_hex(const unsigned char *hash, char *hex_out);

#endif
