// Copyright © RIVIR rivir.space 2024

#ifndef __SYSFLASH_H
#define __SYSFLASH_H


// Flash area indices

#define FLASH_AREA_BOOTLOADER                    (0)
#define FLASH_AREA_IMAGE_SCRATCH                 (1)
#define FLASH_AREA_IMAGE_PRIMARY(image_index)    ((image_index) + 2)
#define FLASH_AREA_IMAGE_SECONDARY(image_index)  ((image_index) + 3)

#define IMAGES_MAX 1
#define SLOTS_MAX  2
#define FLASH_AREA_ID_MAX 5


// Flash device indices

#define FLASH_DEV_INTERNAL 0
#define FLASH_DEV_QSPI     1

#endif
