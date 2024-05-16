


#include <string.h>
#include "flash_map_backend/flash_map_backend.h"
#include "sam.h"

// SAMD/SAME low-level flash driver code

#define wait_ready()                                                                              \
    while (NVMCTRL->STATUS.bit.READY == 0)                                                        \
        ;


int samx_open(void * hw) {
	return 0;
}

void samx_close(void * hw) {
}

int samx_read(void * hw, uint32_t addr, void * dest, uint32_t len) {
    wait_ready();
    memcpy(dest, (void *)addr, len);
	return 0;
}

int samx_write(void * hw, uint32_t addr, const void * src, uint32_t len) {
    wait_ready();
    return 1;
}

int samx_erase_sector(void * hw, uint32_t addr) {
    wait_ready();

    // Execute "EB" Erase Block
    NVMCTRL->ADDR.reg = addr;
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EB;
    wait_ready();
    return 0;
}

int samx_erase_sectors(void * hw, uint32_t start, uint32_t len) {
    for (uint32_t addr = start; addr < start + len; addr += NVMCTRL_BLOCK_SIZE) {
        samx_erase_sector(hw, addr);
    }
	return 0;
}

uint8_t samx_erased_val(void * hw) {
	return 0xFF;
}

uint32_t samx_align(void * hw) {
	return 1;
}

int samx_get_sectors(void * hw, uint32_t base, uint32_t len, uint32_t *count, struct flash_sector *sectors) {
	uint32_t addr;
	uint32_t i = 0;

	for (i = 0, addr = base; addr < base + len; i++, addr += NVMCTRL_BLOCK_SIZE)
	{
	    sectors[i].fs_off = addr - base;
	    sectors[i].fs_size = NVMCTRL_BLOCK_SIZE;
	}
	*count = i;

	return 0;
}

const struct flash_driver samx_flash_driver = {
	.open = samx_open,
	.close = samx_close,
	.read = samx_read,
	.write = samx_write,
	.erase_sector = samx_erase_sector,
	.erase_sectors = samx_erase_sectors,
	.erased_val = samx_erased_val,
	.align = samx_align,
	.get_sectors = samx_get_sectors
};
