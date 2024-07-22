


#include <string.h>
#include "flash_map_backend/flash_map_backend.h"
#include "sam.h"

// SAMD/SAME low-level flash driver code

#define wait_ready()                                                                              \
    while (NVMCTRL->STATUS.bit.READY == 0)                                                        \
        ;


static int samx_open(void * hw) {
	return 0;
}

static void samx_close(void * hw) {
}

static int samx_read(void * hw, uint32_t src, void * dest, uint32_t len) {
    wait_ready();
    memcpy(dest, (void *)src, len);
	return 0;
}

static int samx_write(void * hw, uint32_t dest, const void * src, uint32_t len) {
    uint32_t dest_a = dest & ~0x03;  // 4-byte aligned
    int ofs = dest - dest_a;         // Offset between actual address and aligned address
    uint32_t qwbuf[4];
    int qwbuf_c = ofs;
    uint8_t * qwbuf_p = (uint8_t *)qwbuf;
    const uint8_t * src_p = (const uint8_t *)src;

    // Set manual mode
    wait_ready();
	NVMCTRL->CTRLA.bit.WMODE = NVMCTRL_CTRLA_WMODE_MAN;

    // Clear page buffer
    wait_ready();
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_PBC;

    while (len > 0) {
    	samx_read(hw, dest_a, qwbuf, 16);  // Read 4 words into buffer
    	do {  // Copy bytes from src into the buf until 16 bytes or
    		*qwbuf_p = *src_p;
    		qwbuf_p++;
    		src_p++;
    		qwbuf_c++;
    		len--;
    	} while ((qwbuf_c < 16) && (len > 0));

    	// Copy the qwbuf to the page buffer
    	for (int i = 0; i < 4; i++) {
    		((uint32_t *)dest_a)[i] = qwbuf[i];
    	}

    	// Trigger the Write-Quad-Word command at the required address
        wait_ready();
        NVMCTRL->ADDR.reg = (uint32_t)dest_a;
        NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_WQW;

        dest_a += 16;
        qwbuf_c = 0;
        qwbuf_p = (uint8_t *)qwbuf;  // Point back to the start of the QWbuffer.
    }

    return 0;
}

static int samx_erase_sector(void * hw, uint32_t addr) {
    wait_ready();

    // Execute "EB" Erase Block
    NVMCTRL->ADDR.reg = addr;
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EB;
    wait_ready();
    return 0;
}

static int samx_erase_sectors(void * hw, uint32_t start, uint32_t len) {
    for (uint32_t addr = start; addr < start + len; addr += NVMCTRL_BLOCK_SIZE) {
        samx_erase_sector(hw, addr);
    }
	return 0;
}

static uint8_t samx_erased_val(void * hw) {
	return 0xFF;
}

static uint32_t samx_align(void * hw) {
	return 1;
}

static int samx_get_sectors(void * hw, uint32_t base, uint32_t len, uint32_t *count, struct flash_sector *sectors) {
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
