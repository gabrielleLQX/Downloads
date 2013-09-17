#include <stdint.h>
#include "flash_config.h"

//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned long uint32_t;


#define RESULT_OK                   0
#define RESULT_ERROR                1
#define RESULT_OVERRIDE_DEVICE      2
#define RESULT_ERASE_DONE           3
#define RESULT_CUSTOM_FIRST         100
#define RESULT_CUSTOM_LAST          200

#define FLAG_ERASE_ONLY             0x1

// These are functions you MUST implement -------------------------------

#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[]);
#else
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags);
#endif

uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer);

uint32_t FlashErase(void *block_start,
                    uint32_t block_size);

// These are functions you MAY implement --------------------------------

uint32_t FlashChecksum(void const *begin,
                       uint32_t count);

uint32_t FlashSignoff(void);

#define OPTIONAL_CHECKSUM _Pragma("required=FlashChecksumEntry") __root
#define OPTIONAL_SIGNOFF  _Pragma("required=FlashSignoffEntry") __root
void FlashChecksumEntry();
void FlashSignoffEntry();

// These are functions you may call -------------------------------------
uint16_t Crc16(uint8_t const *p, uint32_t len);
