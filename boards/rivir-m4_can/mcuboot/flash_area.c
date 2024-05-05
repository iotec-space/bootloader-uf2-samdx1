#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

int flash_area_open(uint8_t id, const struct flash_area **fa)
{
	return 0;
}

void flash_area_close(const struct flash_area *fa)
{
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
