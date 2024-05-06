// Copyright © RIVIR rivir.space 2024

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"


static struct flash_area flash_partitions[FLASH_AREA_ID_MAX] =
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
		.fa_size = 0x00070000,  // 256 K - 64 K = 196 K
	},

	{
		.fa_id = FLASH_AREA_SPARE,
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00008000,  // 32 K
		.fa_size = 0x00007000,  // 28 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00050000,  // 64 K
		.fa_size = 0x00030000,  // 256 K - 64 K = 196 K
	},
};

int flash_area_open(uint8_t id, const struct flash_area **fa)
{
	if (id >= FLASH_AREA_ID_MAX)
		return -1;

	struct flash_area * fa_active = &flash_partitions[id];
	fa_active->fa_open_counter++;

	if (fa_active->fa_open_counter > 1) {  // Already open
		return 0;
	}

	// Do whatever to open the flash
	*fa = fa_active;
	return 0;
}

void flash_area_close(const struct flash_area *fa)
{
	struct flash_area * fa_active = (struct flash_area *)fa;
	fa_active->fa_open_counter--;

	if (fa_active->fa_open_counter > 0) {  // Still open somewhere
		return;
	}

	// Do whatever to close the flash
	fa_active->fa_open_counter = 0;
	return;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
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
	return 256;
}

int flash_area_get_sectors(int fa_id, uint32_t *count,
                           struct flash_sector *sectors)
{
	return 0;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
	return 0;
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}

int flash_area_id_from_image_offset(uint32_t offset)
{
	return 0;
}
