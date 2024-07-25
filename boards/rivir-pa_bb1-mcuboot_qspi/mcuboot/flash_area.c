// Copyright © RIVIR rivir.space 2024

#include "bootutil/bootutil_log.h"
#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

#include "uf2.h"

static struct flash_area flash_areas[FLASH_AREA_ID_MAX] =
{
	{
		.fa_id = FLASH_AREA_BOOTLOADER,
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00000000,
		.fa_size = 0x0000C000,   // 48 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_SCRATCH,
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x0000C000,  // 48 K
		.fa_size = 0x00004000,  // 16 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
		.fa_device_id = FLASH_DEV_INTERNAL,
		.fa_off  = 0x00010000,  // 64 K
		.fa_size = 0x000F0000,  // 1024 K - 64 K = 960 K
	},

	{
		.fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
		.fa_device_id = FLASH_DEV_QSPI,
		.fa_off  = 0x00400000,  // 4 MB (LFS used first 4 MB)
		.fa_size = 0x000F0000,  // 1 MB allocated, but must be the same size as primary image area
	},
};

static int fa_index_by_image[IMAGES_MAX * 2] = {
	2, 3
};


struct flash_hw {
	const struct flash_driver * driver;
	void * hw;
};

extern const struct flash_driver samx_flash_driver;
extern const struct flash_driver samd_qspiflash_driver;

const struct flash_hw flash_hw[] = {
	{ .driver = &samx_flash_driver, .hw = 0 },
	{ .driver = &samd_qspiflash_driver, .hw = 0 },
};


int flash_area_open(uint8_t id, const struct flash_area **_fa)
{
	if (id >= FLASH_AREA_ID_MAX)
		return -1;

	struct flash_area * fa = &flash_areas[id];
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	int res = 0;

	fa->fa_open_counter++;
	if (fa->fa_open_counter == 1) {  // First time: open the flash
		res = fd->open(hw);
	}

	*_fa = fa;
	return res;
}

void flash_area_close(const struct flash_area * _fa)
{
	struct flash_area * fa = (struct flash_area *)_fa;
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;

	fa->fa_open_counter--;
	if (fa->fa_open_counter <= 0) {  // The last time we are closing it
		fd->close(hw);

		fa->fa_open_counter = 0;
	}

	return;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	int res;

	MCUBOOT_LOG_DBG("fa_rd: %d, %08lx %ld", fa->fa_device_id, fa->fa_off + off, len);

	res = fd->read(hw, fa->fa_off + off, dst, len);
	return res;
}

int flash_area_write(const struct flash_area *fa, uint32_t off,
                     const void *src, uint32_t len)
{
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	int res;

	MCUBOOT_LOG_DBG("fa_wr: %d, %08lx %ld", fa->fa_device_id, fa->fa_off + off, len);

	res = fd->write(hw, fa->fa_off + off, src, len);
	return res;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	int res;

	MCUBOOT_LOG_DBG("fa_er: %d, %08lx %ld", fa->fa_device_id, fa->fa_off + off, len);

	res = fd->erase_sectors(hw, fa->fa_off + off, len);
	return res;
}

uint8_t flash_area_erased_val(const struct flash_area *fa)
{
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	uint8_t res;

	res = fd->erased_val(hw);
	return res;
}

uint32_t flash_area_align(const struct flash_area *fa)
{
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	uint32_t res;

	res = fd->align(hw);
	return res;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
	struct flash_area * fa = &flash_areas[fa_id];
	const struct flash_driver * fd = flash_hw[fa->fa_device_id].driver;
	void * hw = flash_hw[fa->fa_device_id].hw;
	int res;

	res = fd->get_sectors(hw, fa->fa_off, fa->fa_size, count, sectors);
	return res;
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
