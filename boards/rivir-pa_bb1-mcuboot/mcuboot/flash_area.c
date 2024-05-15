// Copyright © RIVIR rivir.space 2024

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

#include "uf2.h"

static struct flash_area flash_areas[FLASH_AREA_ID_MAX] =
{
	{
		.fa_id = FLASH_AREA_BOOTLOADER,
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00000000,
		.fa_size = 0x0000F000,   // 60 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_SCRATCH,
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x0000F000,  // 60 K
		.fa_size = 0x00001000,  // 4 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00010000,  // 64 K
		.fa_size = 0x00070000,  // 512 K - 64 K = 448 K
	},

	{
		.fa_id = FLASH_AREA_SPARE,
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00080000,  // 512 K
		.fa_size = 0x00010000,  // 64 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00090000,  // 576 K
		.fa_size = 0x00070000,  // 512 K - 64 K = 448 K
	},
};

static int fa_index_by_image[IMAGES_MAX * 2] = {
	2, 4
};

int flash_area_open(uint8_t id, const struct flash_area **fa)
{
	if (id >= FLASH_AREA_ID_MAX)
		return -1;

	struct flash_area * fa_active = &flash_areas[id];
	fa_active->fa_open_counter++;

	if (fa_active->fa_open_counter == 1) {  // First time: open the flash
		// Do whatever to open the flash
	}

	*fa = fa_active;
	return 0;
}

void flash_area_close(const struct flash_area *fa)
{
	struct flash_area * fa_active = (struct flash_area *)fa;
	fa_active->fa_open_counter--;

	if (fa_active->fa_open_counter <= 0) {  // The last time we are closing it
		// Do whatever to close the flash
		fa_active->fa_open_counter = 0;
	}

	return;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
	flash_read_words(dst, (void *)(fa->fa_off + off), len / 4);
	return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off,
                     const void *src, uint32_t len)
{
	return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
	return 0;
}

uint8_t flash_area_erased_val(const struct flash_area *fap)
{
	return 0xFF;
}

uint32_t flash_area_align(const struct flash_area *fa)
{
	return 1;
}

#define SECTOR_SIZE 4096
int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
    size_t off;
	uint32_t total_count = 0;
    const struct flash_area *fa = &flash_areas[fa_id];

	for (off = 0; off < fa->fa_size; off += SECTOR_SIZE)
	{
	    sectors[total_count].fs_off = off;
	    sectors[total_count].fs_size = SECTOR_SIZE;
	    total_count++;
	}
    *count = total_count;
    return 0;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
	return fa_index_by_image[image_index * SLOTS_MAX + slot];
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}

int flash_area_id_from_image_offset(uint32_t offset)
{
	return 0;
}
