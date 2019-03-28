#include <stdint.h>
#include <stddef.h>

struct footer{
    uint32_t len;
    uint32_t packed_len;
    uint32_t crc32;
    uint32_t pack_type;
    char marker[4];
};

size_t dvpl_check(uint8_t* dvpl, size_t size);
size_t dvpl_pack_max(size_t size, uint8_t type);

uint8_t dvpl_pack(uint8_t* dvpl, size_t* dvpl_size, uint8_t* data, size_t size, uint8_t type);
uint8_t dvpl_unpack(uint8_t* dvpl, size_t dvpl_size, uint8_t* data);
