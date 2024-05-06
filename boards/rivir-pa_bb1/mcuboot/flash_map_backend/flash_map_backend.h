// Copyright © RIVIR rivir.space

#ifndef __FLASH_MAP_BACKEND_H
#define __FLASH_MAP_BACKEND_H


#include <inttypes.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif


struct flash_area
{
  uint8_t     fa_id;         /* The slot/scratch ID */
  uint8_t     fa_device_id;  /* The device ID (usually there's only one) */
  uint16_t    pad16;         /* Padding */
  uint32_t    fa_off;        /* The flash offset from the beginning */
  uint32_t    fa_size;       /* The size of this sector */

  /* Implementation-specific fields */
  int         fa_open_counter;
};

/* Structure describing a sector within a flash area. */

struct flash_sector
{
  uint32_t fs_off;   /* Offset of this sector, from the start of its flash area (not device). */
  uint32_t fs_size;  /* Size of this sector, in bytes. */
};


static inline uint8_t flash_area_get_id(const struct flash_area *fa)
{
  return fa->fa_id;
}


static inline uint8_t flash_area_get_device_id(const struct flash_area *fa)
{
  return fa->fa_device_id;
}

static inline uint32_t flash_area_get_off(const struct flash_area *fa)
{
  return fa->fa_off;
}

static inline uint32_t flash_area_get_size(const struct flash_area *fa)
{
  return fa->fa_size;
}


int flash_area_get_sector(const struct flash_area *fap, off_t off, struct flash_sector *fs);

static inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
{
    return fs->fs_off;
}

static inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
{
    return fs->fs_size;
}


int flash_area_open(uint8_t id, const struct flash_area **fa);
void flash_area_close(const struct flash_area *fa);
int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len);
int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len);
int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len);
uint32_t flash_area_align(const struct flash_area *fa);
uint8_t flash_area_erased_val(const struct flash_area *fa);
int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors);
int flash_area_id_from_multi_image_slot(int image_index, int slot);
int flash_area_id_from_image_slot(int slot);
int flash_area_id_to_multi_image_slot(int image_index, int area_id);
int flash_area_id_from_image_offset(uint32_t offset);


#ifdef __cplusplus
}
#endif

#endif /* __FLASH_MAP_BACKEND_H */
