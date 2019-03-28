#include "potato.h"
#include <stdbool.h>
#include <lz4hc.h>
#include <memory.h>

static uint32_t crc32b(unsigned char *message, size_t size) {
   int i, j;
   uint32_t byte, crc, mask;

   crc = 0xFFFFFFFF;
   for(size_t i=0; i<size; i++){
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
   }
   return ~crc;
}

//EN: Returns unpacked DVPL size on successful. 0 on fail.
//RU: Возвращает рамер распакованного DVPL при успешном выполнении. 0 при ошибке.
size_t dvpl_check(uint8_t* dvpl, size_t size){
    if(size>=sizeof(struct footer)){
	struct footer* ft=dvpl+size-sizeof(struct footer);
	if(!memcmp(ft->marker, "DVPL", 4)){
		if(ft->packed_len==size-sizeof(struct footer)&&ft->crc32==crc32b(dvpl, size-sizeof(struct footer))){
			return ft->len;
		}
        }
    }
	return 0;
}

//EN: Returns 0 on successful. Warning! No buffer size checking!
//RU: Возвращает 0 при успешном выполнении. ВНИМАНИЕ! Размер буфера не проверяется!
uint8_t dvpl_unpack(uint8_t* dvpl, size_t dvpl_size, uint8_t* data){
	struct footer* ft=dvpl+dvpl_size-sizeof(struct footer);
	switch(ft->pack_type){
	case 0://RAW
		memcpy(data, dvpl, ft->len);
		break;
	case 1://LZ4
	case 2://LZ4HC
		LZ4_decompress_safe(dvpl, data, ft->packed_len, ft->len);
		break;
	default:
		return LIBPOTATO_NOT_IMPLEMENTED;
	}
}

//EN: Returns max DVPL buffer size for given size of data.
//RU: Возвращает максимальный размер буфера DVPL для данных заданного размера.
size_t dvpl_pack_max(size_t size, uint8_t type){
    switch(type){
        case 0:
            return size+sizeof(struct footer);
        case 1:
        case 2:
            return LZ4_compressBound(size)+sizeof(struct footer);
        default:
            return 0;
    }
}

//EN: Returns 0 on successful and places REAL DVPL size to dvpl_size.
//RU: Возвращает 0 при успешном выполнении и записывет РЕАЛЬНЫЙ размер DVPL.
uint8_t dvpl_pack(uint8_t* dvpl, size_t* dvpl_size, uint8_t* data, size_t size, uint8_t type){
    size_t compressed_size;

    switch(type) {
    case 0://RAW
        if(size+sizeof(struct footer)>*dvpl_size){
			return LIBPOTATO_ERR_NO_MEM;
        }

        memcpy(dvpl, data, size);
    case 2://LZ4HC
    {
        compressed_size=LZ4_compressBound(size);

        if(compressed_size+sizeof(struct footer)>*dvpl_size){
			return LIBPOTATO_ERR_NO_MEM;
        }
        compressed_size=LZ4_compress_HC(data, dvpl, size, *dvpl_size, LZ4HC_CLEVEL_MAX);
        if(compressed_size<=0){
            return 2;
        }
        break;
    }
    case 1://LZ4
        compressed_size=LZ4_compressBound(size);

        if(compressed_size+sizeof(struct footer)>*dvpl_size){
			return LIBPOTATO_ERR_NO_MEM;
        }

        compressed_size=LZ4_compress_default(data, dvpl, size, *dvpl_size);
        if(compressed_size<=0){
            return 2;
        }
        break;
    default:
	return LIBPOTATO_NOT_IMPLEMENTED;
    }
    *dvpl_size=compressed_size+sizeof(struct footer);

    struct footer* ft=dvpl+compressed_size;
    ft->crc32=crc32b(dvpl, compressed_size);
    ft->packed_len=compressed_size;
    ft->len=size;
    ft->pack_type=2;
    memcpy(ft->marker, "DVPL", 4);

    return 0;
}
