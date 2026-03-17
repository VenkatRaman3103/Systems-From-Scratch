#ifndef HASH_H
#define HASH_H

#include <stddef.h>

void sha1_hash(const unsigned char *data, size_t len, unsigned char *out);
void hash_to_hex(const unsigned char *hash, char *hex);

#endif
